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
#include <functional>
#include <getopt.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "rtp_def.h"
#include "rtp_factory.h"

using namespace OHOS::Sharing;

int gType = -1; // 0 for aac, 1 for h264, 2 for g711, 3 for mpegts
char *gFileName = nullptr;
char *gAudioFileName = nullptr;
int gPort = 1234;
FILE *gFd = nullptr;
FILE *gAudioFd = nullptr;

#define BUFF_SIZE 10240

void ShowUsage(char *exe)
{
    printf("Usage:\n%s -t <type> -p <listening port> -o <file to save data>\n", exe);
    printf("\t-t 0: AAC, 1: H264, 2: G711, 3: mpegts\n");
    printf("\t-p local listening port, default 1234\n");
    printf("\t-o file to save data OR video data in ts/ps demux stream, such as 123.h264, 234.aac\n");
    printf("\t-a file to save audio data in ts/ps demux stream, such as 234.aac\n");
}

int ParseParam(int argc, char *argv[])
{
    int ret;
    while ((ret = getopt(argc, argv, ":t:p:o:a:")) != -1) {
        switch (ret) {
            case ('t'):
                gType = atoi(optarg);
                break;
            case ('p'):
                gPort = atoi(optarg);
                break;
            case ('o'):
                gFileName = optarg;
                break;
            case ('a'):
                gAudioFileName = optarg;
                break;
            case ':':
                printf("option [-%c] requires an argument", static_cast<char>(optopt));
                break;
            case '?':
                printf("unknown option: %c", static_cast<char>(optopt));
                break;
            default:
                break;
        }
    }

    if ((gType < 0 || gType > 3) || gFileName == nullptr) {
        printf("param error\n");
        ShowUsage(argv[0]);
        return -1;
    }
    return 0;
}

int RecvUDP(const std::function<void(const char *buf, int len)> &cb)
{
    int servSocket, nbytes;
    struct sockaddr_in servAddr, cliAddr;
    int addrLen = sizeof(cliAddr);
    char buffer[BUFF_SIZE];

    if ((servSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket err");
        return -1;
    }

    bzero(&servAddr, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(gPort);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(servSocket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind err");
        return -1;
    }

    while (1) {
        if ((nbytes = recvfrom(servSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliAddr,
                               (socklen_t *)&addrLen)) < 0) {
            perror("recvfrom err");
            return -1;
        }

        printf("\nRecv data(size:%d) From %s:%d\n", nbytes, inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        cb(buffer, nbytes);
        memset(buffer, 0, BUFF_SIZE);
    }
}

void HandleSignal(int sig)
{
    if (gFd != nullptr) {
        fclose(gFd);
    }

    if (gAudioFd != nullptr) {
        fclose(gAudioFd);
    }
    printf("Catch Ctrl+C,\nsave '%s' success\nexit success.\n", gFileName);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (ParseParam(argc, argv) != 0) {
        return -1;
    }
    printf("%s -t %d -p %d -o %s -a %s\n", argv[0], gType, gPort, gFileName, gAudioFileName);

    // Catch Ctrl+C signal
    signal(SIGINT, HandleSignal);
    if (gFileName != nullptr) {
        gFd = fopen(gFileName, "wb");
    }
    if (gAudioFileName) {
        gAudioFd = fopen(gAudioFileName, "wb");
    }
    if (gType == 1) {
        auto h264unPack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{96, 90000, RtpPayloadStream::H264});
        h264unPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
            if (frame->GetTrackType() == TRACK_VIDEO) {
                printf("h264 data: len: %d dts: %d\n", frame->Size(), frame->Dts());
                auto data = frame->Data();
                auto bytes = frame->Size();
                for (size_t i = 0; i < 16 && i < bytes; i++) {
                    printf("%02x ", data[i] & 0xff);
                }
                printf("\n");
                fwrite(frame->Data(), frame->Size(), 1, gFd); // save .h264 file
            }
        });

        RecvUDP([h264unPack](const char *buf, int len) {
            // print data (12B RTP header + 12B Payload)
            for (size_t i = 0; i < 24 && i < len; i++) {
                printf("%02x ", buf[i] & 0xff);
                if (i == 11) {
                    printf("\n");
                }
            }
            printf("\n");
            h264unPack->ParseRtp(buf, len);
        });

    } else if (gType == 0) {
        auto extra = std::make_shared<AACExtra>();
        extra->aacConfig_ = "1210";
        auto aacunPack =
            RtpFactory::CreateRtpUnpack(RtpPlaylodParam{97, 44100, RtpPayloadStream::MPEG4_GENERIC, extra});
        aacunPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
            printf("SetOnRtpUnpack\n");
            if (frame->GetTrackType() == TRACK_AUDIO) {
                printf("aac data: len: %d dts: %d\n", frame->Size(), frame->Dts());
                auto data = frame->Data();
                auto bytes = frame->Size();
                for (size_t i = 0; i < 16 && i < bytes; i++) {
                    printf("%02x ", data[i] & 0xff);
                }
                printf("\n");
                fwrite(frame->Data(), frame->Size(), 1, gFd); // save .aac file
            }
        });

        RecvUDP([aacunPack](const char *buf, int len) {
            // print data (12B RTP header + 12B Payload)
            for (size_t i = 0; i < 24 && i < len; i++) {
                printf("%02x ", buf[i] & 0xff);
                if (i == 11) {
                    printf("\n");
                }
            }
            printf("\n");
            aacunPack->ParseRtp(buf, len);
        });
    } else if (gType == 2) {
        auto g711Unpack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{97, 8000, RtpPayloadStream::PCMA});
        g711Unpack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
            printf("SetOnRtpUnpack\n");
            if (frame->GetTrackType() == TRACK_AUDIO) {
                printf("g711 data: len: %d dts: %d\n", frame->Size(), frame->Dts());
                auto data = frame->Data();
                auto bytes = frame->Size();
                for (size_t i = 0; i < 16 && i < bytes; i++) {
                    printf("%02x ", data[i] & 0xff);
                }
                printf("\n");
                fwrite(frame->Data(), frame->Size(), 1, gFd); // save .pcma file
            }
        });

        RecvUDP([g711Unpack](const char *buf, int len) {
            // print data (12B RTP header + 12B Payload)
            for (size_t i = 0; i < 24 && i < len; i++) {
                printf("%02x ", buf[i] & 0xff);
                if (i == 11) {
                    printf("\n");
                }
            }
            printf("\n");
            g711Unpack->ParseRtp(buf, len);
        });
    } else if (gType == 3) {
        auto tsUnpack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{33, 90000, RtpPayloadStream::MPEG2_TS});
        tsUnpack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
            printf("mpegts OnRtpUnpack in TrackType = %d size=%d dts=%d\n", frame->GetTrackType(), frame->Size(),
                   frame->Dts());
            printf("ts data: len: %d dts: %d\n", frame->Size(), frame->Dts());
            auto data = frame->Data();
            auto bytes = frame->Size();
            for (size_t i = 0; i < 16 && i < bytes; i++) {
                printf("%02x ", data[i] & 0xff);
            }
            printf("\n");
            if (frame->GetTrackType() == TRACK_VIDEO) {
                fwrite(frame->Data(), frame->Size(), 1, gFd); // save .h264 file
            } else if (frame->GetTrackType() == TRACK_AUDIO) {
                fwrite(frame->Data(), frame->Size(), 1, gAudioFd);
            }
        });

        RecvUDP([tsUnpack](const char *buf, int len) {
            // print data (12B RTP header + 12B Payload)
            for (size_t i = 0; i < 24 && i < len; i++) {
                printf("%02x ", buf[i] & 0xff);
                if (i == 11) {
                    printf("\n");
                }
            }
            printf("\n");
            tsUnpack->ParseRtp(buf, len);
        });
    }

    return 0;
}