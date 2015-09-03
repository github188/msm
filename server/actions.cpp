/*
 * actions.cpp -- actions
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 21:50:02
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

#include "actions.hpp"
#include "msg.hpp"

int login(msm_msg_t msm_msg)
{
        string user_id = msm_msg.root["user_id"].asString();
        string user_name = msm_msg.root["user_name"].asString();
        int sockfd = msm_msg.sockfd;
        return msm_msg.msm->login(user_id, user_name, sockfd);
}

int logout(msm_msg_t msm_msg)
{

        string user_id = msm_msg.root["user_id"].asString();
        Msm *msm = msm_msg.msm;

        MsmReqMsg msg(user_id, "close", "user logout");

        Member *m = msm->get_member(user_id);
        if (m != NULL) {
                int sockfd = m->get_sockfd();
                if (sockfd != -1)
                        msg.send_msg(sockfd);
        }

        return msm->logout(user_id);
}

int heartbeat(msm_msg_t msm_msg)
{
        return 0;
}

int askmeeting(msm_msg_t msm_msg)
{
        string meeting_id = msm_msg.root["meeting_id"].asString();
        string meeting_name = msm_msg.root["meeting_name"].asString();
        string user_id = msm_msg.root["user_id"].asString();
        Msm *msm = msm_msg.msm;

        msm->create_meeting(meeting_id, meeting_name);
        msm->get_meeting(meeting_id)->add_member(msm->get_member(user_id));
        msm->get_meeting(meeting_id)->set_admin(user_id);
        return 0;
}

int adduser(msm_msg_t msm_msg)
{
        string meeting_id = msm_msg.root["meeting_id"].asString();
        string user_id = msm_msg.root["user_id"].asString();
        string users = msm_msg.root["users"].asString();
        Msm *msm = msm_msg.msm;
        if (msm->get_meeting(meeting_id) != NULL) {
                if (msm->get_member(users) == NULL) {
                        Member *m = new Member(users, "Unkonwn");
                        m->set_offline();
                        msm->register_member(m);
                }
                msm->get_meeting(meeting_id)->add_member(msm->get_member(users));
        }

        return 0;
}

int leavemeeting(msm_msg_t msm_msg)
{
        string meeting_id = msm_msg.root["meeting_id"].asString();
        string user_id = msm_msg.root["user_id"].asString();
        Msm *msm = msm_msg.msm;

        Meeting *meeting = msm->get_meeting(meeting_id);
        if (meeting != NULL) {
                Member *m = msm->get_member(user_id);
                if (m->get_id() != meeting->get_admin()) {
                        meeting->del_member(m);
                } else {
                        msm->destroy_meeting(meeting_id);
                }
        }

        return 0;
}
