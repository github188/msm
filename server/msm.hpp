/*
 * msm.hpp -- msm
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 20:47:43
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

#ifndef MSM_HPP
#define MSM_HPP
#include <map>
#include <string>
#include <sys/epoll.h>

#include <json/json.h>

#include "member.hpp"
#include "meeting.hpp"
#include "actions.hpp"

#define MAX_EVENTS 20

using namespace std;

class Member;
class Meeting;

class Msm
{
public:
        Msm();
        ~Msm();

public:
        void dump_members();
        void dump_meetings();
        bool have_member(string id);
        Member *get_member(string id);
        bool have_meeting(string id);
        Meeting *get_meeting(string id);

        int login(string id, string name, int);
        int logout(string id);
        int create_meeting(string id, string name);
        int create_meeting(string id, string name, vector<Member *>);
        int destroy_meeting(string id);
        int init();

        int send_test(string);
        int register_member(Member *);
private:

        int unregister_member(string id);
        int register_meeting(Meeting *);
        int unregister_meeting(string id);

        int setnonblocking(int);
        int epoll_pollin(int);
        int epoll_closefd_before(string, int, string);
        int epoll_closefd_after(string, int, string);

        Member *get_member_byfd(int fd);

private:
        int port;
        map<string, Member *>msm_members;
        vector<Meeting *>msm_meetings;
        struct epoll_event ev;
        int epollfd;
};

#endif
