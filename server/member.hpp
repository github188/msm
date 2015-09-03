/*
 * members.hpp -- members hpp
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/21 15:52:03
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

#ifndef MEMBER_HPP
#define MEMBER_HPP

#include <string>
#include <map>

#include "meeting.hpp"
#include "msm.hpp"

#define STATUS_UNKNOWN 0x0000
#define STATUS_OFFLINE 0x0001
#define STATUS_ONLINE  0x0002
#define STATUS_MEETING 0x0004


#define MEMBER_NAME_MAX 32
#define MEMBER_ID_MAX   32

using namespace std;
using std::string;

class Meeting;
class Msm;

class Member
{
public:
        Member();
        explicit Member(string name,string id);
        ~Member();


        void print_member(void);

public:
        int get_status(void);
        Meeting *get_meeting(void);
        string get_id(void);
        string get_name(void);
        int get_sockfd(void);
        int get_volum(void);

        int set_status(int);
        int set_meeting(Meeting *);
        int set_id(string);
        int set_name(string);
        int set_sockfd(int);
        int set_volum(int);

        void set_offline();
        void set_online();
        void set_inmeeting();
        void clear_inmeeting();
        bool is_offline();
        bool is_online();
        bool is_inmeeting();
private:
        int m_volum;
        int m_sockfd;
        unsigned m_status;
        string m_id;
        string m_name;
        Meeting *m_meeting;
};



#endif
