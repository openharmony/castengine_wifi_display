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
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXBUF       1024
#define MAXEPOLLSIZE 100

int setnonblocking(int sockfd)
{
    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

void *pthread_handle_message(void *sock_fd)
{
    char recvbuf[MAXBUF + 1];
    char sendbuf[MAXBUF + 1];
    int ret;
    int new_fd;
    struct sockaddr_in client_addr;
    socklen_t cli_len = sizeof(client_addr);
    int *sock_fd_new = (int *)sock_fd;
    new_fd = *sock_fd_new;

    bzero(recvbuf, MAXBUF + 1);
    bzero(sendbuf, MAXBUF + 1);

    do {
        ret = recvfrom(new_fd, recvbuf, MAXBUF, 0, (struct sockaddr *)&client_addr, &cli_len);
        if (ret > 0) {
            printf("socket %d recv from :%s:%d success.msg=[%s]total %d bytes\n", new_fd,
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), recvbuf, ret);
        } else {
            printf("rece msg failed.ret=%d,error code=%d,error msg=%s\n", ret, errno, strerror(errno));
        }
    } while (ret > 0);

    fflush(stdout);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int listener, kdpfd, nfds, n;
    socklen_t len;
    struct sockaddr_in my_addr;
    unsigned int myport;
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    struct rlimit rt;

    myport = 9999;

    pthread_t thread;
    pthread_attr_t attr;

    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit");
        exit(1);
    } else {
        printf("set param success\n");
    }

    if ((listener = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket create failed.\n");
        exit(1);
    } else {
        printf("socket create success\n");
    }

    int opt = SO_REUSEADDR;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    setnonblocking(listener);
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    } else {
        printf("IP bind success\n");
    }

    kdpfd = epoll_create(MAXEPOLLSIZE);
    len = sizeof(struct sockaddr_in);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener;
    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listener);
        return -1;
    } else {
        printf("epoll add success\n");
    }

    while (1) {
        nfds = epoll_wait(kdpfd, events, 10000, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listener) {
                pthread_attr_init(&attr);
                pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

                if (pthread_create(&thread, &attr, pthread_handle_message, (void *)&(events[n].data.fd))) {
                    perror("pthread_creat error!");
                    exit(-1);
                }
            }
        }
    }
    close(listener);
    return 0;
}