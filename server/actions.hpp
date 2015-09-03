/*
 * actions.hpp -- actions
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 20:52:50
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

#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <json/json.h>
#include <string>
#include <map>
#include "msm.hpp"
using namespace std;

class Msm;

typedef struct {
        Msm *msm;
        Json::Value root;
        int sockfd;
}msm_msg_t;

int login(msm_msg_t);
int logout(msm_msg_t);
int heartbeat(msm_msg_t);
int askmeeting(msm_msg_t);
int adduser(msm_msg_t);
int leavemeeting(msm_msg_t);

extern map<string, int(*)(msm_msg_t)>actions_handle;
#endif
