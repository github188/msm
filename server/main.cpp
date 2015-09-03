/*
 * main.cpp -- main
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/09/03 02:51:29
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
#include <stdlib.h>
#include <time.h>


#include <iostream>
#include <string>
#include <map>
#include "msm.hpp"
#include "log.hpp"
#include "actions.hpp"

#define ARRAY_SIZE(a) ((int)(sizeof(a)/(sizeof(a[0]))))

typedef struct {
        string id;
        string name;
}member;

typedef struct {
        string id;
        string name;
}meeting;

member a[] = {
        {"1", "member1"},
        {"2", "member2"},
        {"3", "member3"},
        {"4", "member4"},
        {"5", "member5"},
        {"6", "member6"},
        {"7", "member7"},
};

meeting b[] = {
        {"m1", "meeting1"},
        {"m2", "meeting2"},
        {"m3", "meeting3"},
};

using namespace std;
using std::string;


void test2()
{
        Msm *msm = new Msm;

        msm->init();
}
map<string, int(*)(msm_msg_t)>actions_handle;
int main(int argc, char *argv[])
{
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("login", &login));
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("logout", &logout));
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("heartbeat", &heartbeat));
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("askmeeting", &askmeeting));
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("adduser", &adduser));
        actions_handle.insert(pair<string, int(*)(msm_msg_t)>("leavemeeting", &leavemeeting));



        test2();
        return 0;
}
