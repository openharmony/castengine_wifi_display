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
#include <iostream>
#include <thread>
#include "common/sharing_log.h"
#include "video_sink_decoder.h"

using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
constexpr int32_t START_CODE_OFFSET_ONE = -1;
constexpr int32_t START_CODE_OFFSET_SEC = -2;
constexpr int32_t START_CODE_OFFSET_THIRD = -3;
constexpr int32_t START_CODE_SIZE_FRAME = 4;
constexpr int32_t START_CODE_SIZE_SLICE = 3;
constexpr char START_CODE = 0x1;

class VideoSinkDecoderDemo : public VideoSinkDecoder,
                             public std::enable_shared_from_this<VideoSinkDecoderDemo> {
public:
    VideoSinkDecoderDemo() : VideoSinkDecoder(1) {}
    DataBuffer::Ptr ReadOnePacket()
    {
        if (file_ == nullptr) {
            file_ = std::make_unique<std::ifstream>();
            file_->open("/data/public_file/640x480.h264", std::ios::in | std::ios::binary);
        }

        constexpr uint32_t bufferSize = 1000000;
        auto fileBuffer = new char[bufferSize + 1];
        memset(fileBuffer, 0, bufferSize + 1);
        file_->read(fileBuffer, START_CODE_SIZE_FRAME);

        if (file_->eof() || file_->fail()) {
            SHARING_LOGD("read eof fail");
            return nullptr;
        }

        char *temp = fileBuffer;
        temp += START_CODE_SIZE_FRAME;
        bool ret = true;
        while (!file_->eof()) {
            file_->read(temp, 1);
            if (*temp == START_CODE) {
                if ((temp[START_CODE_OFFSET_ONE] == 0) && (temp[START_CODE_OFFSET_SEC] == 0) &&
                    (temp[START_CODE_OFFSET_THIRD] == 0)) {
                    file_->seekg(-START_CODE_SIZE_FRAME, std::ios_base::cur);
                    temp -= (START_CODE_SIZE_FRAME - 1);
                    ret = false;
                    break;
                } else if ((temp[START_CODE_OFFSET_ONE] == 0) && (temp[START_CODE_OFFSET_SEC] == 0)) {
                    file_->seekg(-START_CODE_SIZE_SLICE, std::ios_base::cur);
                    temp -= (START_CODE_SIZE_SLICE - 1);
                    ret = false;
                    break;
                }
            }
            temp++;
        }
        auto size = (temp - fileBuffer);
        if (size > 0) {
            auto data = std::make_shared<DataBuffer>();
            data->PushData(fileBuffer, size);
            delete[] fileBuffer;
            fileBuffer = nullptr;
            return data;
        }

        delete[] fileBuffer;
        fileBuffer = nullptr;
        return nullptr;
    }

    void PlayThread()
    {
        while (!stop_) {
            auto packetData = ReadOnePacket();
            if (packetData) {
                ProcessVideoData(packetData);
            } else {
                SHARING_LOGD("read null");
                break;
            }
        }
    }

    void ProcessVideoData(DataBuffer::Ptr data)
    {
        int32_t bufferIndex = -1;
        {
            SHARING_LOGD("process video data");
            std::unique_lock<std::mutex> lock(inMutex_);
            SHARING_LOGD("process video data in");
            if (inQueue_.empty()) {
                while (!stop_) {
                    inCond_.wait_for(lock, std::chrono::milliseconds(DECODE_WAIT_MILLISECONDS),
                                     [this]() { return (!inQueue_.empty()); });
                    if (inQueue_.empty()) {
                        SHARING_LOGD("index queue empty");
                        continue;
                    }
                    break;
                }
            }
            SHARING_LOGD("process video data get buffer index");
            bufferIndex = inQueue_.front();
            inQueue_.pop();
        }
        SHARING_LOGD("process video data bufferIndex: %{public}d.", bufferIndex);
        if (stop_) {
            SHARING_LOGD("stop return");
            return;
        }

        if (bufferIndex == -1) {
            SHARING_LOGD("stop no process video data");
        } else {
            DecodeVideoData(data->Peek(), data->Size(), bufferIndex);
        }
    }

    void StartPlayThread()
    {
        stop_ = false;
        std::lock_guard<std::mutex> lock(playMutex_);
        if (playThread_ != nullptr) {
            SHARING_LOGD("play start thread already exist");
            return;
        }

        playThread_ = std::make_shared<std::thread>(&VideoSinkDecoderDemo::PlayThread, this);
        if (playThread_ == nullptr) {
            SHARING_LOGE("play start create thread error");
        }
    }

    void StopPlayThread()
    {
        SHARING_LOGD("trace");
        stop_ = true;
        std::lock_guard<std::mutex> lock(playMutex_);
        SHARING_LOGD("wait play");
        if (playThread_) {
            if (playThread_->joinable()) {
                playThread_->join();
            }
            playThread_ = nullptr;
        }
        SHARING_LOGD("stop play exit");
    }

    void StartVideoDecoder()
    {
        SHARING_LOGD("trace");
    }

    void StopVideoDecoder()
    {
        SHARING_LOGD("trace");
    }

private:
    std::atomic<bool> stop_ = true;
    std::shared_ptr<std::thread> playThread_ = nullptr;
    std::mutex playMutex_;
    std::unique_ptr<std::ifstream> file_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS

int main()
{
    auto demo = std::make_shared<VideoSinkDecoderDemo>();
    demo->StartVideoDecoder();
    demo->StartPlayThread();
    std::string inputCmd;
    SHARING_LOGD("press [Enter] for stop");
    getline(std::cin, inputCmd);
    demo->StopVideoDecoder();
    demo->StopPlayThread();

    return 0;
}