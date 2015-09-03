/*
 * main.cpp -- client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 21:50:21
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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <signal.h>
#include <poll.h>

#include <json/json.h>
#include <pthread.h>
#include <zmq.h>

#include <time.h>

#include "msg.hpp"
#include "log.hpp"

#define PORT 2003
#define IP   "127.0.0.1"

#define ARRAY_SIZE(a) (int)(sizeof(a) / sizeof(a[0]))
#define TEST2_USER 1
#define USER1 1
#define USER2 2
#define TEST2

#define TEST2_MEETING(x)                                        \
        do {                                                    \
                if (x == USER1) {                               \
                        ask_meeting(USER1, "m1", "meeting1");   \
                } else {                                        \
                        ask_meeting(USER2, "m2", "meeting2");   \
                }                                               \
        } while(0)                                              \

volatile bool g_run = true;
int user_id = 1;
using namespace std;

// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
        string id;
        string name;
}test_t;

test_t t[] = {
        {"id_1", "test1"},
        {"id_2", "test2"},
        {"id_3", "test3"},
        {"id_4", "test4"},
        {"id_5", "test5"},
        {"id_6", "test6"},
        {"id_7", "test7"},
};

int recv_msg(int sockfd, char buf[], size_t len)
{
        if (buf == NULL || sockfd < 0)
                return -1;

        int ret;
        struct iovec iov;
        struct msghdr msg;

        memset(buf, 0, len);
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        ret = recvmsg(sockfd, &msg, 0);
        if (ret == -1) {
                LOG(LOG_ERROR, "recv_msg sock: %d failed: %s", sockfd, strerror(errno));
                return -1;
        }

        if (ret == 0) {
                LOG(LOG_DEBUG, "recv_msg %d size 0, socket is close by server", sockfd);
                // exit(-1);
                return -1;
        }

        LOG(LOG_DEBUG,"%lu recvmsg:\n%s", pthread_self(), buf);
        return 0;
}

int send_msg(int sockfd, string req)
{
        int ret;
        char buf[1024];

        struct msghdr msg;
        struct iovec iov;

        memset(&msg, 0, sizeof(msg));

        memcpy(buf, req.c_str(), sizeof(buf)-1 );
        iov.iov_base = buf;
        iov.iov_len = strlen(buf) + 1;

        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        ret = sendmsg(sockfd, &msg, 0);

        // ret =send(sockfd, req.c_str(), req.size(), 0);
        if (ret != (int)iov.iov_len) {
                LOG(LOG_ERROR, "send_msg sock: %d failed: %s", sockfd, strerror(errno));
                return -1;
        }
        LOG(LOG_DEBUG, "%lu send_msg:\n%s",pthread_self(), req.c_str());
        return 0;
}
#ifdef TEST2
int socket_send(string req)
{
        int sockfd;
        struct sockaddr_in serveraddr;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                return -1;
        }

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
                LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                return -2;
        }

        if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
                LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                return -3;
        }

        send_msg(sockfd, req);

        char buf[1024];
        recv_msg(sockfd,buf, sizeof(buf));

        close(sockfd);

        return 0;
}
#endif

int login_req(int sockfd, int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "login");
        return msg.send_msg(sockfd);
}

int logout_req(int sockfd, int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "logout");
        return msg.send_msg(sockfd);
}

int heartbeat_req(int sockfd, int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "heartbeat");
        return msg.send_msg(sockfd);
}
#ifdef TEST2

int logout(int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "logout");
        return socket_send(msg.root.toStyledString());
}

int test(int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "test");
        return socket_send(msg.root.toStyledString());
}

int heartbeat(int idx)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "heartbeat");
        return socket_send(msg.root.toStyledString());
}

int ask_meeting(int idx, string meeting_id, string meeting_name)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "askmeeting");
        msg.root["meeting_id"] = Json::Value(meeting_id);
        msg.root["meeting_name"] = Json::Value(meeting_name);
        return socket_send(msg.root.toStyledString());
}

int ask_user_to_meeting(int idx, string meeting_id, int user)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "adduser");
        msg.root["meeting_id"] = Json::Value(meeting_id);
        msg.root["users"] = Json::Value(t[user].id);
        return socket_send(msg.root.toStyledString());
}

int leave_meeting(int idx, string meeting_id)
{
        UserReqMsg msg(t[idx].id, t[idx].name, "leavemeeting");
        msg.root["meeting_id"] = Json::Value(meeting_id);

        return socket_send(msg.root.toStyledString());
}
#endif
void *run(void *arg)
{
#if 1
        int sockfd;
        struct sockaddr_in serveraddr;
        char buf[1024];

        while(1) {
                srandom(time(NULL));
                struct timeval tv;
                tv.tv_sec = random() % 3;
                tv.tv_usec = random() % 1000;

                if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                        return (void *)-1;
                }

                select(sockfd, NULL, NULL, NULL, &tv);
                int idx = random() % ARRAY_SIZE(t);


                memset(&serveraddr, 0, sizeof(serveraddr));
                serveraddr.sin_family = AF_INET;
                serveraddr.sin_port = htons(PORT);
                if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
                        LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                        return (void *)-2;
                }

                if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
                        LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                        return (void *)-3;
                }

                login_req(sockfd, idx);
                recv_msg(sockfd, buf, sizeof(buf));
                recv_msg(sockfd, buf, sizeof(buf));
                tv.tv_sec = random() % 3;
                tv.tv_usec = random() % 1000;
                select(sockfd, NULL, NULL, NULL, &tv);

                heartbeat_req(sockfd, idx);
                recv_msg(sockfd, buf, sizeof(buf));
                tv.tv_sec = random() % 3;
                tv.tv_usec = random() % 1000;
                select(sockfd, NULL, NULL, NULL, &tv);

                logout_req(sockfd, idx);
                recv_msg(sockfd, buf, sizeof(buf));
                tv.tv_sec = random() % 3;
                tv.tv_usec = random() % 1000;
                select(sockfd, NULL, NULL, NULL, &tv);

                close(sockfd);
                sockfd = -1;
        }
#else
        while (1) {
                int ret;
                void *context = zmq_ctx_new();
                if (context == NULL) {
                        return (void *)-1;
                }

                void *req = zmq_socket(context, ZMQ_REQ);
                if (req == NULL) {
                        return (void *)-2;
                }

                ret = zmq_connect(req, "tcp://127.0.0.1:2003");
                if (ret != 0) {
                        return (void *)-3;
                }

                srandom(time(NULL));
                int idx = random() % ARRAY_SIZE(t);
                login_req(req, idx);
                sleep(3);
                heartbeat_req(req, idx);
                sleep(3);
                logout_req(req, idx);
                sleep(2);

                zmq_close(req);
                zmq_ctx_destroy(context);
                context = NULL;
                req = NULL;
        }
#endif

}

#ifdef TEST2
void *listen_run(void *arg)
{
        int sockfd;
        struct sockaddr_in serveraddr;
        char buf[1024];
        struct timeval tv;


        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                exit(-1);
        }

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
                LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                exit(-2);
        }

        if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
                LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                exit(-3);
        }



        login_req(sockfd, user_id);
        recv_msg(sockfd, buf, sizeof(buf));

        while(g_run) {
                recv_msg(sockfd, buf, sizeof(buf));

                Json::Value root;
                Json::Reader reader;
                if (reader.parse(buf, root, false) == false) {
                        LOG(LOG_FATAL, "json parse failed.");
                        // pthread_exit((void *)-1);

                } else {
                        if (root["action"] == "close")  {
                                LOG(LOG_INFO, "%s", root["reson"].asCString());
                                g_run = false;
                                pthread_exit((void *)-1);
                        }

                }

                UserRespMsg msg(t[user_id].id, t[user_id].name, root["action"].asString());
                msg.send_msg(sockfd);

        }
        logout_req(sockfd, user_id);
        recv_msg(sockfd, buf, sizeof(buf));

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);

        close(sockfd);

        return (void *)0;
}

void *send_run(void *arg)
{
        struct timeval tv;

        while(g_run) {
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                select(0, NULL, NULL, NULL, &tv);
                heartbeat(user_id);

                tv.tv_sec = 2;
                tv.tv_usec = 0;
                select(0, NULL, NULL, NULL, &tv);
                test(user_id);

                tv.tv_sec = 2;
                tv.tv_usec = 0;
                select(0, NULL, NULL, NULL, &tv);
                TEST2_MEETING(user_id);
        }

        return (void *)0;
}
#endif
int test1()
{
        pthread_t pids[5];
        int i;
        pthread_setconcurrency(10);
        for (i = 0; i < ARRAY_SIZE(pids); i++) {
                if (pthread_create(&pids[i], NULL, run, NULL) != 0){
                        LOG(LOG_FATAL, "pthread_create failed: %s", strerror(errno));
                        return -1;
                } else {
                        LOG(LOG_DEBUG, "pthread %lu started.\n", (unsigned long)pids[i]);
                }
        }

        for (i = 0; i< ARRAY_SIZE(pids); i++)
                pthread_join(pids[i], NULL);

        return 0;
}
#ifdef TEST2
int test2()
{

        pthread_t listen_pid, send_pid;
        if (pthread_create(&listen_pid, NULL, listen_run, NULL) != 0) {
                LOG(LOG_FATAL, "create listen thread failed: %s", strerror(errno));
                exit (-1);
        }

        if (pthread_create(&send_pid, NULL, send_run, NULL) != 0) {
                LOG(LOG_FATAL, "create send thread failed: %s", strerror(errno));
                exit (-2);
        }


        pthread_join(send_pid, NULL);
        pthread_join(listen_pid, NULL);

        return 0;
}
#endif
void *test3_run(void *arg)
{
        int ret;
        char buf[1024];
        struct pollfd fds[1];
        int sockfd = *(int *)arg;


        fds[0].fd = sockfd;
        fds[0].events = POLLIN;
        while (1) {
                ret = poll(fds, 1, -1);
                if (ret > 0) {
                        if (fds[0].revents & POLLIN) {
                                LOG(LOG_DEBUG, "POLLIN");
                                // pthread_mutex_lock(&mutex);
                                recv_msg(sockfd, buf, sizeof(buf));
                                // pthread_mutex_lock(&mutex);
                                LOG(LOG_DEBUG, "POLLIN done");
                        } else if(fds[0].revents & POLLOUT) {
                                LOG(LOG_DEBUG, "POLLOUT");
                                heartbeat_req(sockfd, 1);
                                LOG(LOG_DEBUG, "POLLOUT done");
                        } else {
                                LOG(LOG_DEBUG, "UNKONWN");
                        }
                }
        }
        return (void *)0;
}

int test3()
{

        int sockfd;
        struct sockaddr_in serveraddr;
        char buf[1024];


        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                exit(-1);
        }


        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
                LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                exit(-2);
        }

        if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
                LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                exit(-3);
        }

        pthread_t pid;
        if (pthread_create(&pid, NULL, test3_run, (void *)&sockfd) != 0) {
                LOG(LOG_FATAL, "pthread_create failed: %s", strerror(errno));
                exit(-1);
        }

        struct timeval tv;
        while (1)  {
                tv.tv_sec = 3;
                tv.tv_usec = 0;
                select(-1, NULL, NULL, NULL, &tv);
                // pthread_mutex_lock(&mutex);
                login_req(sockfd, 1);
                recv_msg(sockfd, buf, sizeof(buf));
                recv_msg(sockfd, buf, sizeof(buf));
                // pthread_mutex_unlock(&mutex);

                tv.tv_sec = 3;
                tv.tv_usec = 0;
                select(-1, NULL, NULL, NULL, &tv);
                // pthread_mutex_lock(&mutex);
                heartbeat_req(sockfd, 1);
                recv_msg(sockfd, buf, sizeof(buf));
                // pthread_mutex_unlock(&mutex);

                tv.tv_sec = 3;
                tv.tv_usec = 0;
                select(-1, NULL, NULL, NULL, &tv);
                // pthread_mutex_lock(&mutex);
                logout_req(sockfd, 1);
                recv_msg(sockfd, buf, sizeof(buf));
                // pthread_mutex_unlock(&mutex);
        }
        pthread_join(pid, NULL);

}

int test4()
{
        int sockfd[3];
        struct sockaddr_in serveraddr[3];
        char buf[3][1024];
        struct timeval tv;

        int i;
        for (i = 0; i < 3; i++) {

        if ((sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                exit(-1);
        }


        memset(&serveraddr[i], 0, sizeof(serveraddr[i]));
        serveraddr[i].sin_family = AF_INET;
        serveraddr[i].sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP, &serveraddr[i].sin_addr) < 0) {
                LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                exit(-2);
        }

        if (connect(sockfd[i], (struct sockaddr *)&serveraddr[i], sizeof(serveraddr[i])) < 0) {
                LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                exit(-3);
        }

                tv.tv_sec = 2;
                tv.tv_usec = 0;
                select(0, NULL, NULL, NULL, &tv);
                login_req(sockfd[i], i);
                recv_msg(sockfd[i], buf[i], sizeof(buf[i]));

        }

#if 0
        if (recv_msg(sockfd[i], buf[i], sizeof(buf[i])) >= 0) {
                Json::Value root;
                Json::Reader reader;

                if (reader.parse(buf[i], root, false) != false) {
                        if (root["action"] == "close") {
                                LOG(LOG_FATAL, "%s", root["reson"].asCString());
                                exit(-1);
                        }
                } else {
                        LOG(LOG_FATAL, "json parse failed.");
                        exit (-1);
                }
        }
#endif

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        ask_meeting(0, "m1", "meeting1");

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        ask_user_to_meeting(0, "m1", 1);
        ask_user_to_meeting(0, "m1", 2);


        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        heartbeat(0);
        heartbeat(1);
        heartbeat(2);


        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(1, "m1");
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(2, "m1");
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        ask_user_to_meeting(0, "m1", 1);
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(0, "m1");



        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        logout_req(sockfd[1], 1);

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        logout_req(sockfd[2], 2);

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        logout_req(sockfd[0], 0);


        close(sockfd[0]);
        close(sockfd[1]);
        close(sockfd[2]);
        return 0;
}

void *test5_run1(void *arg)
{
        int sockfd;
        struct sockaddr_in serveraddr;
        char buf[1024];


        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                LOG(LOG_ERROR, "socket failed: %s", strerror(errno));
                pthread_exit((void *)-1);
        }


        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
                LOG(LOG_ERROR, "inet_pton failed: %s", strerror(errno));
                pthread_exit((void *)-1);
        }

        if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
                LOG(LOG_ERROR, "connect failed: %s", strerror(errno));
                pthread_exit((void *)-1);
        }


        login_req(sockfd, 0);
        recv_msg(sockfd, buf, sizeof(buf));

        while (1) {
                if (recv_msg(sockfd, buf, sizeof(buf)) >= 0) {
                        Json::Value root;
                        Json::Reader reader;

                        if (reader.parse(buf, root, false) != false) {
                                if (root["action"] == "close") {
                                        LOG(LOG_FATAL, "%s", root["reson"].asCString());
                                        close(sockfd);
                                        pthread_exit((void *)-1);
                                }
                        } else {
                                LOG(LOG_FATAL, "json parse failed.");
                                close(sockfd);
                                pthread_exit((void *)-1);
                        }
                }
        }
}


void *test5_run2(void *arg)
{
        struct timeval tv;

        while (1) {
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        ask_meeting(0, "m1", "meeting1");

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        ask_user_to_meeting(0, "m1", 1);
        ask_user_to_meeting(0, "m1", 2);


        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        heartbeat(0);
        heartbeat(1);
        heartbeat(2);


        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(1, "m1");
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(2, "m1");
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        ask_user_to_meeting(0, "m1", 1);
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        leave_meeting(0, "m1");

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select (0, NULL, NULL, NULL, &tv);
        logout(0);
        }
        pthread_exit((void *)0);
}
int test5()
{
        pthread_t pids[2];

        if (pthread_create(&pids[0], NULL, test5_run1, NULL) != 0) {
                LOG(LOG_FATAL, "pthreade_create failed: %s", strerror(errno));
                exit(-1);
        }

        if (pthread_create(&pids[1], NULL, test5_run2, NULL) != 0) {
                LOG(LOG_FATAL, "pthreade_crate failed: %s", strerror(errno));
                exit(-1);
        }

        pthread_join(pids[0], NULL);
        // pthread_join(pids[1], NULL);

        return 0;
}
int main(int argc, char *argv[])
{
        if (argc == 2)
                user_id = 2;
        test5();
        return 0;
}
