/*
 * log.hpp -- log
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/23 07:39:14
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

#ifndef LOG_HPP
#define LOG_HPP
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_DEBUG         "DEBUG"
#define LOG_INFO          "INFO"
#define LOG_ERROR         "ERROR"
#define LOG_FATAL         "FATAL"

#define LOG(level, format, ...)                                         \
        do {                                                            \
                char __logbuf[24];                                      \
                get_time(__logbuf, sizeof(__logbuf));                   \
                fprintf(stderr,                                         \
                        "%s %11s(%4d) [%5s] " format "\n",              \
                        __logbuf, __FILE__,__LINE__, level,  ##__VA_ARGS__); \
        } while(0)

static inline void get_time(char *buf, size_t len)
{
        memset(buf, 0, len);
        time_t t= time(NULL);
        tm *now = localtime(&t);
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year+1990,
                now->tm_mon+1, now->tm_mday, now->tm_hour,
                now->tm_min, now->tm_sec);

}

#endif
