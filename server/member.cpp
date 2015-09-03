/*
 * members.cpp -- members
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 07:38:08
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
#include <unistd.h>
#include "member.hpp"
#include "log.hpp"

using std::string;

int Member::get_volum()
{
        return m_volum;
}

int Member::set_volum(int v)
{
        m_volum = v;
        return 0;
}

int Member::get_sockfd()
{
        return m_sockfd;
}

int Member::set_sockfd(int fd)
{
        m_sockfd = fd;
        return 0;
}

bool Member::is_inmeeting()
{
        if ((m_status & STATUS_MEETING)  && m_meeting != NULL)
                return true;
        else
                return false;
}

bool Member::is_offline()
{
        if ((m_status & STATUS_OFFLINE))
                return true;
        else
                return false;
}

bool Member::is_online()
{
        if ((m_status & STATUS_ONLINE))
                return true;
        else
                return false;
}

void Member::clear_inmeeting()
{
        m_status &= ~(STATUS_MEETING);
}

void Member::set_inmeeting()
{
        m_status |= STATUS_MEETING;
}

void Member::set_offline()
{
        m_status &= ~(STATUS_ONLINE);
        m_status |= STATUS_OFFLINE;
}

void Member::set_online()
{
        m_status &= ~(STATUS_OFFLINE);
        m_status |= STATUS_ONLINE;
}

string Member::get_id(void)
{
        return m_id;
}

string Member::get_name(void)
{
        return m_name;
}

int Member::get_status(void)
{
        return m_status;
}

Meeting *Member::get_meeting(void)
{
        return m_meeting;
}

int Member::set_status(int st)
{
        m_status = st;
        return 0;
}

int Member::set_meeting(Meeting *meeting)
{
        m_meeting = meeting;
        return 0;
}

int Member::set_id(string id)
{
        m_id = id;
        return 0;
}

int Member::set_name(string name)
{
        m_name = name;
        return 0;
}

void Member::print_member(void)
{

        LOG(LOG_DEBUG, "");
        LOG(LOG_DEBUG, "%10s: %s","name", m_name.c_str());
        LOG(LOG_DEBUG, "%10s: %s","id", m_id.c_str());

        if (m_meeting)
                LOG(LOG_DEBUG, "%10s: %s","meeting", m_meeting->get_id().c_str());
        else
                LOG(LOG_DEBUG, "%10s: None", "meeting");

        LOG(LOG_DEBUG, "%10s: %d","sockfd", m_sockfd);
        LOG(LOG_DEBUG, "%10s: %d","volum", m_volum);

        string st;
        if (is_offline())
                st += "offline ";
        if (is_online())
                st += "online ";
        if (is_inmeeting())
                st += "meeting ";
        LOG(LOG_DEBUG, "%10s: %s","status", st.c_str());
}

Member::Member()
{

}

Member::Member(string id, string name):
        m_volum(1),
        m_sockfd(-1),
        m_status(STATUS_UNKNOWN),
        m_id(id),
        m_name(name),
        m_meeting(NULL)
{

}

Member::~Member()
{
        m_status = STATUS_UNKNOWN;
        m_meeting = NULL;
        if (m_sockfd != -1)
                close(m_sockfd);
}
