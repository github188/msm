/*
 * meeting.cpp -- meeting
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 18:53:39
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
#include "meeting.hpp"
#include "log.hpp"

using namespace std;

int Meeting::set_speaker(string id)
{
        return set_speaker(get_member(id));
}

int Meeting::set_speaker(Member *m)
{
        m_speaker = m;
        return 0;
}

Member *Meeting::get_speaker()
{
        return m_speaker;
}

int Meeting::set_admin(string id)
{
        m_admin = id;
        return 0;
}

#if 0
int Meeting::set_admin(Member *m)
{
        m_admin = m;
        return 0;
}


Member *Meeting::get_admin()
{
        return m_admin;
}
#endif
string Meeting::get_admin()
{
        return m_admin;
}

Member *Meeting::get_member(string id)
{
        vector<Member *>::iterator it;
        for (it = m_members.begin(); it != m_members.end(); ++it) {
                if ((*it)->get_id() == id)
                        return (*it);
        }
        return NULL;
}

bool Meeting::have_member(string id)
{
        vector<Member *>::iterator it;
        for (it = m_members.begin(); it != m_members.end(); ++it) {
                if ((*it)->get_id() == id)
                        return true;
        }
        return false;
}

void Meeting::print_meeting(void)
{
        LOG(LOG_DEBUG, "==================== %10s ====================", m_id.c_str());
        if (m_name.size() > 0)
                LOG(LOG_DEBUG, "%10s: %s","name", m_name.c_str());
        else
                LOG(LOG_DEBUG, "%10s: None", "name");

        if (m_admin.size())
                LOG(LOG_DEBUG, "%10s: %s","admin", m_admin.c_str());
        else
                LOG(LOG_DEBUG, "%10s: None", "admin");

        if (m_speaker)
                LOG(LOG_DEBUG, "%10s: %s","speaker", m_speaker->get_id().c_str());
        else
                LOG(LOG_DEBUG, "%10s: None", "speaker");

        dump_members();
        LOG(LOG_DEBUG, "**************************************************");
}


void Meeting::dump_members(void)
{
        LOG(LOG_DEBUG, "%10s: ", "members");
        vector<Member *>::iterator it;
        for (it = m_members.begin(); it != m_members.end(); ++ it) {
                (*it)->print_member();
        }

}

int Meeting::del_member(string id)
{
        Member *m = get_member(id);
        if (m != NULL) {
                return del_member(m);
        } else {
                return ERR_MEMBER_NOINTHISMEETING;
        }
}

int Meeting::del_member(Member *m)
{
        if (m == NULL)
                return 0;

        if (m->get_meeting() != this) {
                LOG(LOG_ERROR, "%s not in meeting %s", m->get_id().c_str(),
                    get_id().c_str());
                return ERR_MEMBER_NOINTHISMEETING;
        }

        if (!have_member(m->get_id())) {
                return ERR_MEMBER_NOEXIST;
        }

        vector<Member *>::iterator it;
        for (it = m_members.begin(); it != m_members.end(); ++ it) {
                if ((*it)->get_id() == m->get_id()) {
                        vector<Member *>::iterator itt = it;
                        (*itt)->clear_inmeeting();
                        (*itt)->set_meeting(NULL);
                        m_members.erase(itt);
                        return 0;
                }
        }
        return ERR_MEMBER_NOEXIST;
}

int Meeting::add_member(Member *m)
{
        if ( m->get_meeting() != NULL) {
                return ERR_MEMBER_NOINTHISMEETING;
        }

        if (have_member(m->get_id()))
                return 0;

        if (m->get_meeting() == NULL) {
                m_members.push_back(m);
                m->set_meeting(this);
                m->set_inmeeting();
        }
        return 0;
}

int Meeting::set_name(string name)
{
        m_name = name;
        return 0;
}

int Meeting::set_id(string id)
{
        m_id = id;
        return 0;
}

vector<Member *> Meeting::get_members()
{
        return m_members;
}

string Meeting::get_name()
{
        return m_name;
}

string Meeting::get_id()
{
        return m_id;
}

Meeting::Meeting(string id, string name):
        m_id(id),
        m_name(name),
        m_admin(""),
        m_speaker(NULL)
{

}

Meeting::~Meeting()
{
        m_members.clear();
}
