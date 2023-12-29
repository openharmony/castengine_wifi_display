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
#include "rtp_def.h"
#include "rtp_factory.h"

#define BUF_SIZE 2 * 1024

using namespace OHOS::Sharing;

void StartRtpServer(int port, const std::function<void(const char *buf, size_t len)> &cb)
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
    serveraddr.sin_port = htons(port);
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
        printf("client IP:%s, port:%d, datalen:%d, info:%s\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port,
               byte, buf);
        cb(buf, byte);
    }
    close(listenfd);
}

int main(int argc, char **argv)
{
    const char *h264file = "/test.h264";
    const char *aacfile = "/test.aac";
    const char *aacrtp = "/aac.rtp";
    FILE *fh264 = fopen(h264file, "wb");
    FILE *faac = fopen(aacfile, "wb");
    FILE *faacrtp = fopen(aacrtp, "wb");

    auto extra = std::make_shared<AACExtra>();
    extra->aacConfig_ = "1210";
    auto aacPack = RtpFactory::CreateRtpPack(4568712, 1400, 44100, 97, RtpPayloadStream::MPEG4_GENERIC);

    aacPack->SetOnRtpPack([=](const RtpPacket::Ptr &rtp) {
        printf("rtp packed seq: %d, timestamp: %d, size: %d\n", rtp->GetSeq(), rtp->GetStamp(), rtp->Size());
        fwrite(rtp->Data(), rtp->Size(), 1, faacrtp);
    });

    auto aacunPack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{97, 44100, RtpPayloadStream::MPEG4_GENERIC, extra});
    aacunPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
        printf("SetOnRtpUnpack\n");
        if (frame->GetTrackType() == TRACK_AUDIO) {
            printf("aac data: len: %d dts: %d", frame->Size(), frame->Dts());
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
            aacPack->InputFrame(frame);
        }
    });

    StartRtpServer(1234, [aacunPack](const char *buf, size_t len) { aacunPack->ParseRtp(buf, len); });

    auto h264unPack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{96, 90000, RtpPayloadStream::H264});
    h264unPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
        if (frame->GetTrackType() == TRACK_VIDEO) {
            printf("h264 data: len: %d dts: %d", frame->Size(), frame->Dts());
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
        }
    });

    StartRtpServer(1236, [h264unPack](const char *buf, size_t len) { h264unPack->ParseRtp(buf, len); });

    return 0;
}