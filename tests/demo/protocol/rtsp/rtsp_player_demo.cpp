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
#include <condition_variable>
#include <functional>
#include <getopt.h>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "rtp_def.h"
#include "rtp_factory.h"
#include "rtsp_client.h"
#include "sharing_log.h"

using namespace OHOS::Sharing;

#define RTP_AUDIO_LOCAL_PORT 60000
#define RTP_VIDEO_LOCAL_PORT 60002
#define BUFF_SIZE            10240

FILE *h264File = nullptr;
FILE *aacFile = nullptr;

void HandleSignal(int sig)
{
    if (h264File) {
        fclose(h264File);
    }

    if (aacFile) {
        fclose(aacFile);
    }

    printf("Catch Ctrl+C, exit success.\n");
    exit(0);
}

int RecvUDP(int port, const std::function<void(const char *buf, int len)> &cb)
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
    servAddr.sin_port = htons(port);
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

void RecvSaveH264()
{
    if (!h264File) {
        time_t t = time(nullptr);
        std::string file = "RTP_" + std::to_string(t) + ".h264";
        h264File = fopen(file.c_str(), "wb");
        if (!h264File) {
            return;
        }
        printf("Save H264 (%s)\n", file.c_str());
    }

    auto h264unPack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{96, 90000, RtpPayloadStream::H264});
    h264unPack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
        if (frame->GetTrackType() == TRACK_VIDEO) {
            printf("h264 data: len: %d dts: %d\n", frame->Size(), frame->Dts());
            auto data = frame->Data();
            auto bytes = frame->Size();
            for (size_t i = 0; i < 16 && i < bytes; i++) {
                printf("%02x ", data[i] & 0xff);
            }
            printf("\nfwrite (len: %d)\n", frame->Size());
            fwrite(frame->Data(), frame->Size(), 1, h264File); // save .h264 file
        }
    });

    RecvUDP(RTP_VIDEO_LOCAL_PORT, [h264unPack](const char *buf, int len) {
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
}

int main(int argc, char *argv[])
{
    auto rtspClient = std::make_shared<RtspClient>();
    std::vector<OHOS::Sharing::SdpTrack::Ptr> medias;
    uint32_t trackIndex = 0;
    std::mutex mutex;
    std::condition_variable cvSignal;
    std::string url(argv[1]);

    // Catch Ctrl+C cvSignal
    signal(SIGINT, HandleSignal);

    rtspClient->SetUrl(url);
    rtspClient->SetOnRecvRtspPacket([&](const std::string &method, const RtspMessage &rsp) {
        printf("SetOnRecvRtspPacket method: %s\n", method.c_str());
        if (method == "OPTIONS") {
            printf("SetOnRecvRtspPacket OPTIONS");
            auto err = rtspClient->HandleOptionRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_UNAUTHED:
                    rtspClient->SendOption();
                    break;
                case RtspRspResult::RSP_OK:
                    rtspClient->SendDescribe();
                    break;
                default: {
                    printf("SetOnRecvRtspPacket ret err");
                    cvSignal.notify_one();
                } break;
            }

        } else if (method == "DESCRIBE") {
            printf("SetOnRecvRtspPacket DESCRIBE");
            auto err = rtspClient->HandleDescribeRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_UNAUTHED:
                    rtspClient->SendDescribe();
                    break;
                case RtspRspResult::RSP_OK: {
                    auto mediaTemp = rtspClient->GetAllMedia();
                    for (auto media : mediaTemp) {
                        medias.emplace_back(media);
                    }
                    // set RTP listening port for a/v, may just use one port
                    rtspClient->SetTransPort(RTP_AUDIO_LOCAL_PORT, TRACK_AUDIO);
                    rtspClient->SetTransPort(RTP_VIDEO_LOCAL_PORT, TRACK_VIDEO);
                    rtspClient->SendSetup(trackIndex++);

                    break;
                }
                default: {
                    printf("SetOnRecvRtspPacket ret err");
                    cvSignal.notify_one();
                    break;
                }
            }
        } else if (method == "SETUP") {
            printf("SetOnRecvRtspPacket SETUP");
            auto err = rtspClient->HandleSetupRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_OK:
                    if (trackIndex < medias.size()) {
                        rtspClient->SendSetup(trackIndex++);
                    } else {
                        rtspClient->SendPlay();
                        trackIndex = 0;
                    }
                    break;
                default: {
                    printf("SetOnRecvRtspPacket ret err");
                    cvSignal.notify_one();
                } break;
            }
        } else if (method == "PLAY") {
            printf("SetOnRecvRtspPacket PLAY");
            auto err = rtspClient->HandlePlayRsp(rsp);
            if (err != RtspRspResult::RSP_OK) {
                printf("SetOnRecvRtspPacket ret err");
                cvSignal.notify_one();
            }

            RecvSaveH264();

            cvSignal.notify_one();
        }
    });

    rtspClient->SetExceptionCb([&](const RtspExceptType except) {
        printf("OnRtspException err");
        printf("%s %d\n", __FUNCTION__, __LINE__);
        cvSignal.notify_one();
    });

    rtspClient->SetRtspReadyCb([&]() {
        printf("%s %d send Option\n", __FUNCTION__, __LINE__);
        rtspClient->SendOption();
    });

    if (!rtspClient->Open()) {
        printf("OpenRtspClient err");
        return 0;
    }

    std::unique_lock<std::mutex> lk(mutex);
    cvSignal.wait(lk);
    return 0;
};