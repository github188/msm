/*
 * test_server.c -- server
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/09/03 01:58:05
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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define PORT 34567
int main(void)
{
        int sockfd;
        struct sockaddr_in addr;

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1) {
                fprintf(stderr, "socket failed.\n");
                return errno;
        }
	int val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
		fprintf(stderr, "setsockopt reuseaddr failed.\n");
		return errno;
	}

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
                fprintf(stderr, "bind failed.\n");
                return errno;
        }


        char buf[1024];
        struct sockaddr_in serveraddr;
        socklen_t socklen = sizeof(serveraddr);
        while (1) {
                memset(buf, 0, sizeof(buf));
                if (recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&serveraddr, &socklen) > 0) {
                        printf("recv: %s", buf);
                } else {
                        printf("recv failed.\n");
                }
        }

        return 0;
}
