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
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "frame/frame.h"
#include "rtp_factory.h"

#define BUF_SIZE 2 * 1024

using namespace OHOS::Sharing;

void StartRtpServer(const std::function<void(const char *buf, size_t len)> &cb)
{
    int listenfd, opt = 1;
    (void)opt;
    struct sockaddr_in serveraddr;

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (listenfd < 0) {
        perror("Create socket fail.");
        return;
    }

    memset((void *)&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind error.");
        return;
    }

    int byte = 0;
    char buf[BUF_SIZE] = {0};
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;

    if (listenfd <= 0) {
        perror("socket fd value err");
        return;
    }

    while (1) {
        memset(buf, 0, BUF_SIZE);
        byte = recvfrom(listenfd, buf, BUF_SIZE, 0, (struct sockaddr *)&clientaddr, &len);
        if (byte == 0) {
            printf("sockfd:%d read over\n", listenfd);
            break;
        }
        if (byte < 0) {
            perror("read failed");
            break;
        }
        cb(buf, byte);
    }
    close(listenfd);
}

int main(int argc, char **argv)
{
    const char *h264file = "/test.h264";
    const char *aacfile = "/test.aac";
    FILE *fh264 = fopen(h264file, "wb");
    FILE *faac = fopen(aacfile, "wb");

    auto unPack = RtpFactory::CreateRtpUnpack();
    unPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
        printf("SetOnRtpUnpack\n");
        if (frame->GetTrackType() == TRACK_VIDEO) {
            printf("h264 data: len: %d=====================================\n", frame->Size());
            auto data = frame->Data();
            auto bytes = frame->Size();
            for (size_t i = 0; i < bytes; i++) {
                printf("%02X ", data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
            printf("\n");
            fwrite(frame->Data(), frame->Size(), 1, fh264);
        } else if (frame->GetTrackType() == TRACK_AUDIO) {
            printf("aac data: len: %d=====================================\n", frame->Size());
            auto data = frame->Data();
            auto bytes = frame->Size();
            for (size_t i = 0; i < bytes; i++) {
                printf("%02X ", data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
            printf("\n");
            fwrite(frame->Data(), frame->Size(), 1, faac);
        }
    });

    StartRtpServer([unPack](const char *buf, size_t len) { unPack->ParseRtp(buf, len); });
    return 0;
}