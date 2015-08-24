/*
 * msg.cpp -- msg
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 21:01:37
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
#include <string.h>
#include <errno.h>
#include "msg.hpp"
#include "log.hpp"

int Msg::send_msg(int sockfd)
{
        string msg = writer.write(root);
        int ret;
        char buf[1024];

        struct msghdr msghdr;
        struct iovec iov;

        memset(&msghdr, 0, sizeof(msghdr));
        memset(buf, 0, sizeof(buf));

        memcpy(buf, msg.c_str(), sizeof(buf)-1);
        iov.iov_base = buf;
        iov.iov_len = strlen(buf) + 1;

        msghdr.msg_iov = &iov;
        msghdr.msg_iovlen = 1;

        ret = sendmsg(sockfd, &msghdr, 0);
        if (ret != (int)iov.iov_len) {
                LOG(LOG_ERROR, "send_msg sock: %d failed: %s", sockfd, strerror(errno));
                return errno;
        } else if (ret == 0) {
                LOG(LOG_ERROR, "send_msg sock: %d send size 0.", sockfd);
        } else {
                LOG(LOG_DEBUG, "send_msg sock: %d success:\n%s", sockfd, buf);
        }

        return ret;
}

int Msg::recv_msg(int sockfd)
{
        return 0;
}
int Msg::size()
{
        string msg = writer.write(root);
        return msg.size();
}

Msg::Msg()
{

}
Msg::~Msg()
{

}

MsmReqMsg::MsmReqMsg(string to, string action, string reson) : Msg()
{
        root["from"] = Json::Value("msm");
        root["to"] = Json::Value(to);
        root["type"] = Json::Value("request");
        root["action"] = Json::Value(action);
        if (reson.size() > 0)
                root["reson"] = Json::Value(reson);
}

MsmReqMsg::~MsmReqMsg()
{

}

MsmRespMsg::MsmRespMsg(string to, string action, string result, string reson) : Msg()
{
        root["from"] = Json::Value("msm");
        root["to"] = Json::Value(to);
        root["type"] = Json::Value("response");
        root["action"] = Json::Value(action);
        root["result"] = Json::Value(result);
        if (reson.size() > 0)
                root["reson"] = Json::Value(reson);
}

MsmRespMsg::~MsmRespMsg()
{

}


UserReqMsg::UserReqMsg(string from, string name, string action, string reson) : Msg()
{
        root["from"] = Json::Value(from);
        root["to"] = Json::Value("msm");
        root["user_id"] = Json::Value(from);
        root["user_name"] = Json::Value(name);
        root["type"] = Json::Value("request");
        root["action"] = Json::Value(action);
        if (reson.size() > 0)
                root["reson"] = Json::Value(reson);
}

UserReqMsg::~UserReqMsg()
{

}

UserRespMsg::UserRespMsg(string from, string name, string action, string result, string reson) : Msg()
{
        root["from"] = Json::Value(from);
        root["to"] = Json::Value("msm");
        root["user_id"] = Json::Value(from);
        root["user_name"] = Json::Value(name);
        root["type"] = Json::Value("response");
        root["action"] = Json::Value(action);
        root["result"] = Json::Value(result);
        if (reson.size() > 0)
                root["reson"] = Json::Value(reson);
}

UserRespMsg::~UserRespMsg()
{

}
