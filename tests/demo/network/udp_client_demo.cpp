/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE   1024
#define SERV_PORT 9999

void do_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen)
{
    /* connect to server */
    if (connect(sockfd, (struct sockaddr *)pservaddr, servlen) == -1) {
        perror("connect error");
        exit(1);
    }

    while (1) {
        /* read a line and send to server */
        static int index = 0;
        std::string msg = "tcp/udp client send message.index=" + std::to_string(index++);
        write(sockfd, msg.c_str(), msg.length());
        /* receive data from server */
        printf("send msg :%s\r\n", msg.c_str());
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    /* check args */
    if (argc != 2) {
        printf("usage: udpclient <IPaddress>\n");
        exit(1);
    }

    /* init servaddr */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("[%s] is not a valid IPaddress\n", argv[1]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    do_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    return 0;
}