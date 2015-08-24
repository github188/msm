/*
 * msm.cpp -- msm
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 21:12:54
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <json/json.h>
#include <zmq.h>

#include "msm.hpp"
#include "log.hpp"
#include "msg.hpp"

int Msm::epoll_pollin(int fd)
{
        LOG(LOG_DEBUG, "socket: %d EPOLLIN event.", fd);

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        // int rc = recv(events[n].data.fd, buf, sizeof(buf)-1, 0);


        struct msghdr msghdr;
        struct iovec iov;

        memset(&msghdr, 0, sizeof(msghdr));
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);

        msghdr.msg_iov = &iov;
        msghdr.msg_iovlen = 1;

        int rc = recvmsg(fd, &msghdr, 0);

        if (rc == -1) {
                LOG(LOG_ERROR, "socket: %d recvmsg failed: %s", fd, strerror(errno));
                return -1;
        } else if (rc == 0) {
                LOG(LOG_ERROR, "socket: %d recvmsg 0, socket is closed by user.", fd);
                if (get_member_byfd(fd) != NULL) {
                        logout(get_member_byfd(fd)->get_id());
                }
                return -1;
        }
        LOG(LOG_DEBUG, "socket: %d recvmsg:\n%s", fd, buf);
        Json::Value root;
        Json::Reader reader;
        if (reader.parse(buf, root, false) == false) {
                LOG(LOG_DEBUG, "json parse failed.");
                return -1;
        }

        if (epoll_closefd_before(root["action"].asString(), fd, root["user_id"].asString()) < 0)
                return -1;

        msm_msg_t msm_msg;
        msm_msg.sockfd = fd;
        msm_msg.root = root;
        msm_msg.msm = this;

        actions_handle.find(root["action"].asString())->second(msm_msg);

#if 0
        if (root["action"] == "login") {

                login(root["user_id"].asString(), root["user_name"].asString(), fd);
                // send_test(root["from"].asString());
        } else if (root["action"] == "logout") {

                logout(root["user_id"].asString());

        } else if (root["action"] == "heartbeat") {


        } else if (root["action"] == "askmeeting") {


                create_meeting(root["meeting_id"].asString(), root["meeting_name"].asString());
                get_meeting(root["meeting_id"].asString())->add_member(get_member(root["user_id"].asString()));
                get_meeting(root["meeting_id"].asString())->set_admin(root["user_id"].asString());

        } else if (root["action"] == "adduser") {
                if (get_meeting(root["meeting_id"].asString()) != NULL) {
                        if (get_member(root["users"].asString()) == NULL) {
                                Member *m = new Member(root["users"].asString(), "Unkown");
                                m->set_offline();
                                register_member(m);
                        }
                        get_meeting(root["meeting_id"].asString())->add_member(get_member(root["users"].asString()));
                }
        } else if (root["action"] == "leavemeeting") {
                Meeting *meeting = get_meeting(root["meeting_id"].asString());
                if (meeting != NULL) {
                        Member *m = get_member(root["user_id"].asString());
                        if (m->get_id() != meeting->get_admin()) {
                                meeting->del_member(m);
                        } else {
                                destroy_meeting(meeting->get_id());
                        }
                }
        } else {


        }
#endif
        epoll_closefd_after(root["action"].asString(), fd, root["user_id"].asString());
        dump_members();
        dump_meetings();
        return 0;
}


int Msm::epoll_closefd_before(string action, int fd, string id)
{
        // send response
        MsmRespMsg msg(id, action);
        msg.send_msg(fd);

        if (action == "login")
                return 0;

        if (get_member(id) == NULL || (get_member(id)->is_offline())) {
                MsmReqMsg msg(id, "close", "user not login");
                msg.send_msg(fd);

                LOG(LOG_ERROR, "%s not log in.", id.c_str());
                if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
                        LOG(LOG_ERROR, "epoll_ctl del %d failed: %s", fd, strerror(errno));
                        return errno;
                }
                close(fd);
                LOG(LOG_INFO, "close sockfd: %d", fd);
                return -1;
        }

        return 0;
}

int Msm::epoll_closefd_after(string action, int fd, string id)
{
        if (get_member(id) && get_member(id)->get_sockfd() != fd) {
                if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
                        LOG(LOG_ERROR, "epoll_ctl del %d failed: %s", fd, strerror(errno));
                        return errno;
                }
                close(fd);
                LOG(LOG_INFO, "close sockfd: %d", fd);
                return -1;
        }

        return 0;
}

int Msm::setnonblocking(int fd)
{
        int flags;
        if ((flags = fcntl(fd, F_GETFL, 0)) < 0){
                LOG(LOG_FATAL, "fcntl F_GETFL failed.");
                return -1;
        }

        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
                LOG(LOG_FATAL, "fcntl F_SETFL failed.");
                return -2;
        }
        return 0;
}

int Msm::init()
{
#if 1
        int listen_fd;
        struct sockaddr_in addr;
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0) {
                LOG(LOG_FATAL, "create socket failed: %s", strerror(errno));
                return -1;
        }
        int val = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val)) < 0) {
                LOG(LOG_FATAL, "setsockopt failed: %s", strerror(errno));
                return -2;
        }

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                LOG(LOG_FATAL, "bind failed: %s", strerror(errno));
                return -2;
        }

        if (listen(listen_fd, 10) == -1) {
                LOG(LOG_FATAL, "listen failed: %s", strerror(errno));
                return -3;
        }

        struct epoll_event events[MAX_EVENTS];
        int  nfds, n, conn_fd;
        struct sockaddr_in peer_addr;
        socklen_t addrlen;

        epollfd = epoll_create(128);
        if (epollfd == -1) {
                LOG(LOG_FATAL, "epoll_create failed: %s", strerror(errno));
                return -4;
        }

        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = listen_fd;

        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1){
                LOG(LOG_FATAL, "epoll_ctl: listen_fd failed: %s", strerror(errno));
                return -5;
        }

        LOG(LOG_DEBUG, "msm init success.");
        for (;;) {
                nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
                if (nfds == -1) {
                        LOG(LOG_ERROR, "epoll_wait failed: %s", strerror(errno));
                        continue;
                }
                for (n = 0; n < nfds; ++n) {
                        if (events[n].data.fd == listen_fd) {
                                conn_fd = accept(listen_fd, (struct sockaddr *)&peer_addr,&addrlen);
                                if (conn_fd == -1) {
                                        LOG(LOG_FATAL, "accept failed: %s", strerror(errno));
                                        return -6;
                                }
                                // getpeername(conn_fd, (struct sockaddr *)&peer_addr, &addrlen);
                                // LOG(LOG_DEBUG, "ip: %s\nport: %d", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                                setnonblocking(conn_fd);
                                ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                                ev.data.fd = conn_fd;
                                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &ev) == -1) {
                                        LOG(LOG_FATAL, "epoll_ctl conn_fd failed: %s", strerror(errno));
                                        return -7;
                                }
                                LOG(LOG_DEBUG, "accept new socket: %d", conn_fd);

                        } else {
                                if (events[n].events & EPOLLIN) {
                                        epoll_pollin(events[n].data.fd);
                                } else if (events[n].events & EPOLLOUT) {
                                        // LOG(LOG_DEBUG, "socket: %d EPOLLOUT event.", events[n].data.fd);
                                } else {
                                        // LOG(LOG_DEBUG, "socket: %d Unknown event.", events[n].data.fd);
                                }
                        }
                }
        }
#else

        void *context = zmq_ctx_new();
        if (context == NULL) {
                LOG(LOG_FATAL, "zmq_ctx_new failed.");
                return -1;
        }

        void *resp = zmq_socket(context, ZMQ_REP);
        if (resp == NULL) {
                LOG(LOG_FATAL, "zmq_socket failed.");
                return -2;
        }

        int rc = zmq_bind(resp, "tcp://*:2003");
        if (rc != 0) {
                LOG(LOG_FATAL, "zmq_bind failed.");
                return -3;
        }
        LOG(LOG_INFO, "msm init success.");


        while (1) {
                memset(buf, 0, sizeof(buf));
                rc = zmq_recv(resp, buf, sizeof(buf), 0);
                if (rc == -1) {
                        LOG(LOG_ERROR, "zmq_recv failed: %s.",
                            zmq_strerror(errno));
                        continue;
                }

                // LOG(LOG_DEBUG,  "recv: %s", buf);

                Json::Value root;
                Json::StyledWriter writer;

                root["type"] = Json::Value("return");
                root["errMsg"] = Json::Value("success");

                std::string response = writer.write(root);
                zmq_send(resp, response.c_str(), response.size(), 0);
                LOG(LOG_INFO, "send: %s", response.c_str());
        }

        zmq_close(resp);
        zmq_ctx_destroy(context);
#endif
        return 0;
}

int Msm::create_meeting(string id, string name)
{
        vector<Member *>mem;
        return create_meeting(id, name, mem);
}

int Msm::create_meeting(string id, string name, vector<Member *>mem)
{
        if (have_meeting(id)) {
                LOG(LOG_ERROR, "meeting %s have exist.", id.c_str());
                return -1;
        }

        Meeting *m = new Meeting(id, name);

        vector<Member *>::iterator it;
        for (it = mem.begin(); it != mem.end(); ++ it) {
                if (m->add_member((*it)) == 0) {
                        (*it)->set_meeting(m);
                        (*it)->set_inmeeting();
                }
        }

        register_meeting(m);
        return 0;
}
// 1. offline user --> delete
// 2. online user --> leave meeting
int Msm::destroy_meeting(string id)
{
        if ( !have_meeting(id)) {
                LOG(LOG_ERROR, "meeting %s not exist.", id.c_str());
                return -1;
        }

        return unregister_meeting(id);
}

int Msm::register_meeting(Meeting *m)
{
        msm_meetings.push_back(m);
        return 0;
}

int Msm::unregister_meeting(string id)
{

        vector<Member *> mems = get_meeting(id)->get_members();
        vector<Member *>::iterator it;
        for (it = mems.begin(); it != mems.end(); ++it) {
                vector<Member *>::iterator itt = it;
                if ((*itt)->is_offline()) {
                        get_meeting(id)->del_member((*itt));
                        logout((*itt)->get_id());
                } else {
                        get_meeting(id)->del_member((*itt));
                }

        }

        vector<Meeting *>::iterator mit;
        for (mit = msm_meetings.begin(); mit != msm_meetings.end(); ++mit) {
                vector<Meeting *>::iterator mitt = mit;
                if ((*mit)->get_id() == id) {
                        delete *mitt;
                        msm_meetings.erase(mitt);
                        return 0;
                }
        }


        return 0;
}

Meeting *Msm::get_meeting(string id)
{
        vector<Meeting *>::iterator it;
        for (it = msm_meetings.begin(); it != msm_meetings.end(); ++it) {
                if ((*it)->get_id() == id)
                        return (*it);
        }
        return NULL;
}

bool Msm::have_meeting(string id)
{
        vector<Meeting *>::iterator it;
        for (it = msm_meetings.begin(); it != msm_meetings.end(); ++it) {
                if ((*it)->get_id() == id)
                        return true;
        }
        return false;
}

// 1.user not in meeting
// 2.user in meeting and not is admin -->set offline
// 3.user in meeting and is admin --> destroy meeting
int Msm::logout(string id)
{
        if (!have_member(id)) {
                LOG(LOG_ERROR, "%s not login.", id.c_str());
                return -1;
        }

        Member *m = get_member(id);
        int fd = m->get_sockfd();
        if (fd != -1) {
                if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
                        LOG(LOG_ERROR, "epoll_ctl del %d failed: %s", fd, strerror(errno));
                }
                close(fd);
                m->set_sockfd(-1);
                LOG(LOG_INFO, "close sockfd: %d", fd);
        }

        if (m->get_meeting() != NULL) {
                if (m->get_meeting()->get_admin() == id) {
                        destroy_meeting(m->get_meeting()->get_id());
                } else {
                        m->set_offline();
                }
        }
        return unregister_member(id);
}
int Msm::send_test(string id)
{
        MsmReqMsg msg(id, "test");
        return msg.send_msg(get_member(id)->get_sockfd());
}

// 1. user login first
// 2. old user not in meeting and online  --> send msg to he and replace he
// 3. old user not in meeting and offline --> replace he and online
// 4. old user in meeting and is not admin and online --> send msg to
// he and replace he and send meeting info to new user and online and
// into meeting and tell other user
// 5. old user in meeting and is admin and online -->  same 4
// 6. old user in meeting and is not admin and offline --> 4 (don't
// send msg to he)
// 7. old user in meeting and is admin and offline --> same 4(dont't
// sned msg to he)
int Msm::login(string id, string name, int sockfd)
{
        Member *m = new Member(id, name);
        Meeting *meeting = NULL;
        if (have_member(id)) {
                Member *m_old = get_member(id);
                m->set_volum(m_old->get_volum());
                m->set_status(m_old->get_status());

                // close old sockfd
                int old_fd;
                old_fd = m_old->get_sockfd();
                // send msg to old user
                if (m_old->is_online()) {
                        MsmReqMsg msg(id, "close", "duplicate user");
                        msg.send_msg(old_fd);
                }
                #if 1
                if (old_fd != -1 && old_fd != sockfd) {
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, old_fd, &ev) == -1) {
                                LOG(LOG_ERROR, "epoll_ctl del %d failed: %s", old_fd, strerror(errno));
                                return errno;
                        }
                        close(old_fd);
                        m_old->set_sockfd(-1);
                        LOG(LOG_INFO, "close sockfd: %d", old_fd);
                }
                #endif
                // delete old user in meeting
                meeting = m_old->get_meeting();
                if (meeting != NULL) {
                        meeting->del_member(m_old);
                }

                // delete struct old user
                logout(m_old->get_id());
        }

        register_member(m);
        m->set_sockfd(sockfd);
        m->set_online();
        if (meeting != NULL) {
                /// TODO: send meeting info to user
                meeting->add_member(m);
        }

        LOG(LOG_DEBUG, "%s login", id.c_str());
        return 0;
}

int Msm::unregister_member(string id)
{
        map<string, Member *>::iterator it;
        it = msm_members.find(id);
        if (it != msm_members.end()) {
                it->second->set_sockfd(-1);
                if (it->second->get_meeting() == NULL) {
                        Member *t = it->second;
                        delete t;
                        msm_members.erase(it);
                        LOG(LOG_DEBUG, "%s logout and delete", id.c_str());
                        return 0;
                }
        }

        return 0;
}

int Msm::register_member(Member *m)
{
        msm_members.insert(pair<string, Member *>(m->get_id(), m));
        return 0;
}
Member *Msm::get_member_byfd(int fd)
{
        map<string, Member *>::iterator it;
        for (it = msm_members.begin(); it != msm_members.end(); it++) {
                if (it->second->get_sockfd() == fd) {
                        return it->second;
                }
        }
        return NULL;
}
Member *Msm::get_member(string id)
{
        map<string, Member *>::iterator it;
        it = msm_members.find(id);
        if (it == msm_members.end()) {
                return NULL;
        } else {
                return it->second;
        }
}

bool Msm::have_member(string id)
{
        map<string, Member *>::iterator it;
        it = msm_members.find(id);
        if (it == msm_members.end())
                return false;
        else
                return true;

}

void Msm::dump_members()
{
        map<string, Member *>::iterator it;
        for (it = msm_members.begin(); it != msm_members.end(); ++it) {
                (it->second)->print_member();
        }
}

void Msm::dump_meetings()
{
        vector<Meeting *>::iterator it;
        for (it = msm_meetings.begin(); it != msm_meetings.end(); ++it) {
                (*it)->print_meeting();
        }
}

Msm::Msm():
        port(2003)
{

}

Msm::~Msm()
{

}
