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
#include <istream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include "frame/frame.h"
#include "frame/h264_frame.h"
#include "rtp_def.h"
#include "rtp_factory.h"

using namespace OHOS::Sharing;

int gType = -1; // 0 for aac, 1 for h264
char *gFileName = nullptr;
char *gIP = nullptr;
int gPort = 1234;

int sampling_frequency_table[13] = {96000, 88200, 64000, 48000, 44100, 32000, 24000,
                                    22050, 16000, 12000, 11025, 8000,  7350};

void ShowUsage(char *exe)
{
    printf("Usage:%s -t <type> -f <file> -o <destination host ip> -p <destination host port>", exe);
    printf("\t-t 0: AAC, 1: H264, 2: G.711");
    printf("\t-f local file name");
    printf("\t-o destination host ip, such as 192.168.1.100");
    printf("\t-p destination host port, default 1234");
}

int ParseParam(int argc, char *argv[])
{
    int ret;

    while ((ret = getopt(argc, argv, ":t:f:o:p:")) != -1) {
        switch (ret) {
            case ('t'):
                gType = atoi(optarg);
                break;
            case ('f'):
                gFileName = optarg;
                break;
            case ('o'):
                gIP = optarg;
                break;
            case ('p'):
                gPort = atoi(optarg);
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

    if ((gType > 2 || gType < 0) || gFileName == nullptr || gIP == nullptr) {
        printf("param error");
        ShowUsage(argv[0]);
        return -1;
    }
    return 0;
}

int SendRTP(const char *data, int size)
{
    static int socketFd = 0;
    static struct sockaddr_in destHostAddr;

    if (socketFd <= 0) {
        socketFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketFd < 0) {
            perror("socket ");
            return -1;
        }

        destHostAddr.sin_family = AF_INET;
        destHostAddr.sin_port = htons(gPort);
        inet_aton(gIP, &destHostAddr.sin_addr);
        printf("init UDP socket (%d) ok", socketFd);
    }

    ssize_t num = sendto(socketFd, data, size, 0, (struct sockaddr *)&destHostAddr, sizeof(destHostAddr));
    if (num != size) {
        perror("sendto error:");
        return -2;
    }
    printf("send data (size: %d) success.", size);
    return 0;
}

const uint8_t *FindNextNal(const uint8_t *start, const uint8_t *end)
{
    // there is two kind of nal head. four byte 0x00000001 or three byte 0x000001
    while (start <= end - 3) {
        if (start[0] == 0x00 && start[1] == 0x00 && start[2] == 0x01) {
            return start;
        }
        if (start[0] == 0x00 && start[1] == 0x00 && start[2] == 0x00 && start[3] == 0x01) {
            return start;
        }
        start++;
    }

    return end;
}

std::pair<const uint8_t *, int> FindH264Frame(const uint8_t *start, const uint8_t *end)
{
    const uint8_t *frame = FindNextNal(start, end);
    if (frame >= end) {
        return std::pair<const uint8_t *, int>{nullptr, 0};
    }
    const uint8_t *next = FindNextNal(frame + 3, end);
    return std::pair<const uint8_t *, int>{frame, next - frame};
}

int ReadH264File(const std::function<void(const char *buf, int len)> &cb)
{
    std::fstream infile(gFileName, std::ios::in | std::ios_base::binary);
    if (!infile.is_open()) {
        printf("failed to open file");
        return -1;
    }

    infile.seekg(0, std::ios::end);
    int size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    char *content = new char[size];
    infile.read(content, size);
    printf("size %d", size);
    infile.close();

    uint8_t *begin = (uint8_t *)content;
    int i = 0;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::chrono::microseconds interval = std::chrono::microseconds(1000000 / 24);

    while (begin < (uint8_t *)content + size) {
        std::pair<const uint8_t *, int> res = FindH264Frame(begin, (uint8_t *)content + size);

        printf("h264 frame [%d] %p length %d", i, res.first, res.second);
        if (res.second == 0)
            break;

        begin = (uint8_t *)res.first + res.second;

        // skip sei/sps/pps frame
        const unsigned char *p = res.first;
        p = *(p + 2) == 0x01 ? p + 3 : p + 4;
        if ((p[0] & 0x1f) != 0x06 && (p[0] & 0x1f) != 0x07 && (p[0] & 0x1f) != 0x08) {
            std::this_thread::sleep_until(start + interval * ++i);
        }

        cb((const char *)res.first, res.second);
    }
    return 0;
}

std::pair<const uint8_t *, int> FindAACFrame(uint8_t *begin, uint8_t *end)
{
    const uint8_t *p = begin;
    int length = 0;
    while (p <= (end - 7)) {
        if ((p[0] & 0xff) == 0xff && (p[1] & 0xf0) == 0xf0) {
            printf("is aac header");
            length = static_cast<int>(
                (((p[3] & 0x03) << 0x11) | ((p[4] & 0xff) << 0x3) | ((p[5] & 0xe0) >> 0x5))); // get AAC length
            if (p + length <= end) {
                break;
            }
        }
        p++;
    }

    return std::pair<const uint8_t *, int>{p, length};
}

int ReadAacFile(const std::function<void(const char *buf, int len)> &cb)
{
    std::fstream infile(gFileName, std::ios::in | std::ios_base::binary);
    if (!infile.is_open()) {
        printf("failed to open file");
        return -1;
    }

    infile.seekg(0, std::ios::end);
    int size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    char *content = new char[size];
    infile.read(content, size);
    printf("size %d", size);
    infile.close();

    uint8_t *begin = (uint8_t *)content;
    int i = 0;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    int sample_rate = 0;

    std::chrono::microseconds interval;

    while (begin < (uint8_t *)content + size) {
        std::pair<const uint8_t *, int> res = FindAACFrame(begin, (uint8_t *)content + size);
        i++;
        printf("aac frame [%d] %p length %d", i, res.first, res.second);
        if (res.second == 0)
            break;
        if (sample_rate == 0) {
            int index = int((((char *)res.first)[2] & 0x3c) >> 2);
            sample_rate = sampling_frequency_table[index];
            interval = std::chrono::microseconds(1024 * 1000000 / sample_rate);
        }

        begin = (uint8_t *)res.first + res.second;

        std::this_thread::sleep_until(start + interval * i);
        cb((const char *)res.first, res.second);
    }
    return 0;
}

int ReadG711File(int channels, const std::function<void(const char *buf, int len)> &cb)
{
    std::fstream infile(gFileName, std::ios::in | std::ios_base::binary);
    if (!infile.is_open()) {
        printf("failed to open file");
        return -1;
    }

    infile.seekg(0, std::ios::end);
    int size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    char *content = new char[size];
    infile.read(content, size);
    printf("size %d", size);
    infile.close();

    uint8_t *begin = (uint8_t *)content;
    int i = 0;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::chrono::microseconds interval = std::chrono::milliseconds(20); // 20 ms
    int packSize = 160 * channels;
    while (begin < (uint8_t *)content + size) {
        std::this_thread::sleep_until(start + interval * i++);
        cb((const char *)begin, packSize);
        begin = begin + packSize;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (ParseParam(argc, argv) != 0) {
        return -1;
    }
    printf("%s -t %d -f %s -o %s -p %d", argv[0], gType, gFileName, gIP, gPort);

    if (gType == 0) {
        auto aacPack = RtpFactory::CreateRtpPack(4568712, 1400, 48000, 97, RtpPayloadStream::MPEG4_GENERIC);

        aacPack->SetOnRtpPack([=](const RtpPacket::Ptr &rtp) {
            printf("rtp packed seq: %d, timestamp: %d, size: %d", rtp->GetSeq(), rtp->GetStamp(), rtp->Size());
            SendRTP((char *)rtp->Data(), rtp->Size());
        });

        ReadAacFile([=](const char *buf, int len) {
            printf("aac frame %p length %d", buf, len);
            static int index = 0;
            auto frame = FrameImpl::Create();
            frame->codecId_ = CODEC_AAC;
            frame->Assign((char *)buf, len);
            frame->prefixSize_ = 7;
            frame->dts_ = (index++ * 1024) / (48000 / 1000); // 48kHz
            aacPack->InputFrame(frame);
        });
    } else if (gType == 1) {
        auto avcPack = RtpFactory::CreateRtpPack(4568713, 1400, 90000, 96, RtpPayloadStream::H264);

        avcPack->SetOnRtpPack([=](const RtpPacket::Ptr &rtp) {
            printf("rtp packed seq: %d, timestamp: %d, size: %d", rtp->GetSeq(), rtp->GetStamp(), rtp->Size());
            SendRTP((char *)rtp->Data(), rtp->Size());
        });

        ReadH264File([=](const char *buf, int len) {
            printf("h264 frame %p length %d", buf, len);
            for (size_t i = 0; i < 10; i++) {
                printf("%02x ", buf[i]);
            }
            printf("");

            static int index = 0;
            uint32_t ts = index++ * 1000 / 24;
            Frame::Ptr frame = std::make_shared<H264Frame>((uint8_t *)buf, (size_t)len, ts, ts, PrefixSize(buf, len));
            avcPack->InputFrame(frame);
        });
    } else if (gType == 2) {
        auto g711Pack = RtpFactory::CreateRtpPack(4568713, 1400, 8000, 97, RtpPayloadStream::PCMA, 2);

        g711Pack->SetOnRtpPack([=](const RtpPacket::Ptr &rtp) {
            printf("rtp packed seq: %d, timestamp: %d, size: %d", rtp->GetSeq(), rtp->GetStamp(), rtp->Size());
            SendRTP((char *)rtp->Data(), rtp->Size());
        });

        ReadG711File(2, [=](const char *buf, int len) {
            printf("g711 frame %p length %d", buf, len);
            static int index = 0;
            auto frame = FrameImpl::Create();
            frame->codecId_ = CODEC_G711A;
            frame->Assign((char *)buf, len);
            frame->prefixSize_ = 0;
            frame->dts_ = (index++ * 160) / 8; // 8kHz: i * 160 * 1000 / sampleRate

            g711Pack->InputFrame(frame);
        });
    }

    return 0;
}