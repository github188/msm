/*
 * msg.hpp -- msg
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 06:42:07
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

#ifndef MSG_HPP
#define MSG_HPP
#include <json/json.h>
#include <string>
using namespace std;

class Msg
{
public:
        Msg();
        ~Msg();
        Json::Value root;
public:
        int send_msg(int);
        int recv_msg(int);
        int size();
private:
        Json::StyledWriter writer;

};

class MsmReqMsg : public Msg
{
public:
        MsmReqMsg(string to,string action, string reson = "");
        ~MsmReqMsg();

private:
        Json::StyledWriter writer;
};

class MsmRespMsg : public Msg
{
public:
        MsmRespMsg(string to, string action, string result="success", string reson = "");
        ~MsmRespMsg();
private:
        Json::StyledWriter writer;
};

class UserReqMsg : public Msg
{
public:
        UserReqMsg(string from,string name, string action, string reson = "");
        ~UserReqMsg();

private:
        Json::StyledWriter writer;
};

class UserRespMsg : public Msg
{
public:
        UserRespMsg(string from, string name, string action, string result="success", string reson = "");
        ~UserRespMsg();
private:
        Json::StyledWriter writer;
};


#endif
