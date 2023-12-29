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

#include <fstream>
#include <functional>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "codec_factory.h"
#include "common/sharing_log.h"
#include "frame.h"
#include "media_frame_pipeline.h"

using namespace OHOS::Sharing;

char *gInFile = nullptr;
char *gOutFile = nullptr;
int gType = -1; // 0 for decode, 1 for encode

void ShowUsage(char *exe)
{
    SHARING_LOGD("usage:\n%{public}s -t <type> -i <in file> -o <out file>", exe);
    SHARING_LOGD("\t-t 0: decode G.711, 1: encode G.711, 2:decode AAC-ADTS");
    SHARING_LOGD("\t-i in file");
    SHARING_LOGD("\t-o out file");
}

int ParseParam(int argc, char *argv[])
{
    int ret;

    while ((ret = getopt(argc, argv, ":t:i:o:")) != -1) {
        switch (ret) {
            case ('t'):
                gType = atoi(optarg);
                break;
            case ('i'):
                gInFile = optarg;
                break;
            case ('o'):
                gOutFile = optarg;
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

    if ((gType > 2 || gType < 0) || gInFile == nullptr || gOutFile == nullptr) {
        SHARING_LOGD("param error");
        ShowUsage(argv[0]);
        return -1;
    }

    return 0;
}

class RawDataReceiver : public FrameDestination {
public:
    RawDataReceiver(std::fstream &fd) : fd_(fd) {}
    ~RawDataReceiver() = default;
    void OnFrame(const Frame::Ptr &frame) override
    {
        fd_.write((char *)frame->Data(), frame->Size());
        SHARING_LOGD("write data(%{public}p) len(%{public}d)", frame->Data(), frame->Size());
    }

private:
    std::fstream &fd_;
};

void DecodeG711(char *data, int length, std::fstream &fd)
{
    std::shared_ptr<AudioDecoder> decoder = CodecFactory::CreateAudioDecoder(CODEC_G711A);
    if (!decoder) {
        return;
    }
    decoder->Init();

    auto rawReceiver = std::make_shared<RawDataReceiver>(fd);

    decoder->AddAudioDestination(rawReceiver);
    auto g711Frame = FrameImpl::Create();
    g711Frame->codecId_ = CODEC_G711A;
    char *p = data;
    for (int i = 0; i < length / 160; i++) {
        SHARING_LOGD("for i(%{public}d)", i);
        g711Frame->Clear();
        g711Frame->Assign(p, 160);
        decoder->OnFrame(g711Frame);
        p += 160;
    }
    SHARING_LOGD("decodeG711 line(%{public}d).", __LINE__);
}

void DecodeAAC(char *data, int length, std::fstream &fd)
{
    std::shared_ptr<AudioDecoder> decoder = CodecFactory::CreateAudioDecoder(OHOS::Sharing::CODEC_AAC);
    if (!decoder) {
        return;
    }
    decoder->Init();

    auto rawReceiver = std::make_shared<RawDataReceiver>(fd);

    decoder->AddAudioDestination(rawReceiver);
    auto aacFrame = FrameImpl::Create();
    aacFrame->codecId_ = CODEC_AAC;
    char *p = data;
    for (int i = 0; i < length / 2048; i++) {
        SHARING_LOGD("for i(%{public}d)", i);
        aacFrame->Clear();
        aacFrame->Assign(p, 2048);
        decoder->OnFrame(aacFrame);
        p += 2048;
    }
    SHARING_LOGD("decodeAAC line(%{public}d).", __LINE__);
}

void EncodeG711(char *data, int length, std::fstream &fd)
{
    std::shared_ptr<AudioEncoder> encoder = CodecFactory::CreateAudioEncoder(CODEC_G711A);
    if (!encoder) {
        return;
    }
    encoder->Init();

    auto rawReceiver = std::make_shared<RawDataReceiver>(fd);

    encoder->AddAudioDestination(rawReceiver);

    auto pcmFrame = FrameImpl::Create();
    pcmFrame->codecId_ = CODEC_PCM;
    char *p = data;
    for (int i = 0; i < length / 320; i++) {
        SHARING_LOGD("for i(%{public}d)", i);
        pcmFrame->Clear();
        pcmFrame->Assign(p, 320);
        encoder->OnFrame(pcmFrame);
        p += 320;
    }
    SHARING_LOGD("decodeG711 line(%{public}d).", __LINE__);
}

int main(int argc, char *argv[])
{
    SHARING_LOGD("hello codec_demo.");
    if (ParseParam(argc, argv) != 0) {
        return -1;
    }

    std::fstream infile(gInFile, std::ios::in | std::ios_base::binary);
    if (!infile.is_open()) {
        SHARING_LOGD("failed to open file");
        return -1;
    }

    std::fstream outfile(gOutFile, std::ios::out | std::ios_base::binary);
    if (!outfile.is_open()) {
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
    if (gType == 0) {
        DecodeG711(content, size, outfile);
    } else if (gType == 1) {
        EncodeG711(content, size, outfile);
    } else if (gType == 2) {
        DecodeAAC(content, size, outfile);
    }

    outfile.close();
    delete[] content;
}
