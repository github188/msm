/*
 * meeting.hpp -- meeting
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 09:27:03
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

#ifndef MEETING_HPP
#define MEETING_HPP
#include <string>
#include <vector>

#include "member.hpp"

#define ERR_MEMBER_NOEXIST -1
#define ERR_MEMBER_INOTHERMEETING -2
#define ERR_MEMBER_NOINTHISMEETING -3


using namespace std;
using std::string;

class Member;

class Meeting
{
public:
        Meeting(string, string);
        ~Meeting();

public:
        string get_id();
        string get_name();
        vector<Member *> get_members();
        Member *get_member(string id);
        // Member *get_admin();
        string get_admin();
        Member *get_speaker();


        int set_id(string);
        int set_name(string);
        int set_admin(string);
        // int set_admin(Member *);
        bool have_member(string id);
        int del_member(string id);
        int add_member(Member *);
        int del_member(Member *);
        int set_speaker(Member *);
        int set_speaker(string);

        void print_meeting(void);

private:
        void dump_members(void);


private:
        string m_id;
        string m_name;
        vector<Member *> m_members;
        // Member *m_admin;
        string m_admin;
        Member *m_speaker;
};

#endif
