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
#include <chrono>
#include <fstream>
#include <functional>
#include <getopt.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include "codec_factory.h"
#include "common/sharing_log.h"
#include "frame.h"
#include "media_frame_pipeline.h"
#include "media_player.h"
#include "rtp_def.h"
#include "rtp_factory.h"

using namespace OHOS::Sharing;

int gType = -1; // 0 for G.711
char *gInFile = nullptr;
int gPort = 0;
#define BUFF_SIZE 10240

void ShowUsage(char *exe)
{
    SHARING_LOGD("usage:\n%{public}s -i <in file> [-p <local port>]", exe);
    SHARING_LOGD("\t-t 0: play g711 alaw file, 1: play g711 RTP stream");
    SHARING_LOGD("\t-i in file");
    SHARING_LOGD("\t-p local rtp port");
}

int ParseParam(int argc, char *argv[])
{
    int ret;

    while ((ret = getopt(argc, argv, ":t:i:p:")) != -1) {
        switch (ret) {
            case ('t'):
                gType = atoi(optarg);
                break;
            case ('i'):
                gInFile = optarg;
                break;
            case ('p'):
                gPort = atoi(optarg);
                break;
            case ':':
                SHARING_LOGD("option [-%c] requires an argument.", static_cast<char>(optopt));
                break;
            case '?':
                SHARING_LOGD("unknown option: %c.", static_cast<char>(optopt));
                break;
            default:
                break;
        }
    }

    if (!((gType == 0 && gInFile != nullptr) || (gType == 1 && gPort > 0))) {
        SHARING_LOGD("param error");
        ShowUsage(argv[0]);
        return -1;
    }

    return 0;
}

class RawDataReceiver : public FrameDestination {
public:
    RawDataReceiver(const std::shared_ptr<MediaPlayer> &player) : player_(player) {}

    ~RawDataReceiver() = default;

    void OnFrame(const Frame::Ptr &frame) override
    {
        player_->OnData((uint8_t *)frame->Data(), (size_t)frame->Size(), true);
        SHARING_LOGD("recv decoded data(%p) len(%{public}d)", frame->Data(), frame->Size());
        for (int i = 0; i < 12 && i < frame->Size(); i++) {
            printf("%02x ", *(frame->Data() + i));
        }
        printf("\n");
    }

private:
    std::shared_ptr<MediaPlayer> player_;
};

std::shared_ptr<MediaPlayer> InitMediaPlayer(int channels, int sampleRate)
{
    std::shared_ptr<MediaPlayer> player = std::make_shared<MediaPlayer>();
    player->SetAudioParameter(channels, sampleRate);
    player->Prepare();
    player->Play();
    return player;
}

void DecodeG711ByTime(char *data, int length)
{
    std::shared_ptr<AudioDecoder> decoder = CodecFactory::CreateAudioDecoder(CODEC_G711A);
    if (!decoder) {
        return;
    }

    decoder->Init();
    // set audio paramters
    int sampleRate = 44100;
    int channel = 1;

    std::shared_ptr<MediaPlayer> player = InitMediaPlayer(channel, sampleRate);

    auto rawReceiver = std::make_shared<RawDataReceiver>(player);

    decoder->AddAudioDestination(rawReceiver);
    auto g711Frame = FrameImpl::Create();
    g711Frame->codecId_ = CODEC_G711A;
    char *p = data;

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::chrono::microseconds interval = std::chrono::milliseconds(20); // 20 ms, 50 packets/second
    int packSize = (sampleRate / 50) * channel;
    SHARING_LOGD("sampleRate: %{public}d, channel: %{public}d, packSize per 20ms: %{public}d", sampleRate, channel,
                 packSize);
    for (int i = 0; i < length / packSize; i++) {
        SHARING_LOGD("for i(%{public}d)", i);
        g711Frame->Clear();
        g711Frame->Assign(p, packSize);
        decoder->OnFrame(g711Frame);
        p += packSize;
        std::this_thread::sleep_until(start + interval * i);
    }

    SHARING_LOGD("decodeG711ByTime line(%{public}d).", __LINE__);
}

void DecodeG711Immediately(char *data, int length) {}

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

        SHARING_LOGD("\nRecv data(size:%{public}d) From %{public}s:%{public}d", nbytes, inet_ntoa(cliAddr.sin_addr),
                     ntohs(cliAddr.sin_port));

        cb(buffer, nbytes);
        memset(buffer, 0, BUFF_SIZE);
    }
}

int main(int argc, char *argv[])
{
    SHARING_LOGD("trace");
    if (ParseParam(argc, argv) != 0) {
        return -1;
    }

    // play local g711 file
    if (gType == 0) {
        std::fstream infile(gInFile, std::ios::in | std::ios_base::binary);
        if (!infile.is_open()) {
            SHARING_LOGD("failed to open file");
            return -1;
        }

        infile.seekg(0, std::ios::end);
        int size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        char *content = new char[size];
        infile.read(content, size);
        SHARING_LOGD("size %{public}d.", size);
        infile.close();

        DecodeG711ByTime(content, size);
        delete[] content;
    } else if (gType == 1) {
        // recv rtp g711 stream,  decode rtp, play

        std::shared_ptr<AudioDecoder> decoder = CodecFactory::CreateAudioDecoder(CODEC_G711A);
        if (!decoder) {
            return -1;
        }

        decoder->Init();
        std::shared_ptr<MediaPlayer> player = InitMediaPlayer(2, 8000);
        auto rawReceiver = std::make_shared<RawDataReceiver>(player);
        decoder->AddAudioDestination(rawReceiver);

        auto g711Unpack = RtpFactory::CreateRtpUnpack(RtpPlaylodParam{97, 8000, RtpPayloadStream::PCMA});
        g711Unpack->SetOnRtpUnpack([=](uint32_t ssrc, const Frame::Ptr &frame) {
            SHARING_LOGD("setOnRtpUnpack");
            if (frame->GetTrackType() == TRACK_AUDIO) {
                SHARING_LOGD("unpack G711 rtp: len: %{public}d dts: %{public}d.", frame->Size(), frame->Dts());
                for (int i = 0; i < 0x12 && i < static_cast<int>(frame->Size()); i++) {
                    printf("%02x ", *(frame->Data() + i));
                }
                printf("\n");

                decoder->OnFrame(frame);
            }
        });

        RecvUDP([g711Unpack](const char *buf, int len) {
            // print data (12B RTP header + 12B Payload)
            for (int i = 0; i < 24 && i < len; i++) {
                printf("%02x ", buf[i] & 0xff);
                if (i == 11) {
                    printf("\n");
                }
            }
            printf("\n");
            g711Unpack->ParseRtp(buf, len);
        });
    }
}