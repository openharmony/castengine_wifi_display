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

#ifndef OHOS_SHARING_H264_FRAME_H
#define OHOS_SHARING_H264_FRAME_H

#include <cstdlib>
#include <memory>
#include <string>
#include "frame.h"

namespace OHOS {
namespace Sharing {

#define H264_TYPE(v) ((uint8_t)(v)&0x1F)

size_t PrefixSize(const char *ptr, size_t len);
void SplitH264(const char *ptr, size_t len, size_t prefix, const std::function<void(const char *, size_t, size_t)> &cb);
// template <typename Parent>
class H264Frame : public FrameImpl {
public:
    using Ptr = std::shared_ptr<H264Frame>;

    enum {
        NAL_IDR = 5,
        NAL_SEI = 6,
        NAL_SPS = 7,
        NAL_PPS = 8,
        NAL_AUD = 9,
        NAL_B_P = 1,
    };

    H264Frame()
    {
        this->codecId_ = CODEC_H264;
    }

    explicit H264Frame(DataBuffer &&dataBuffer) : FrameImpl(std::move(dataBuffer))
    {
        this->codecId_ = CODEC_H264;
    }

    H264Frame(uint8_t *ptr, size_t size, uint32_t dts, uint32_t pts = 0, size_t prefix_size = 0)
    {
        this->Assign((char *)ptr, (int32_t)size);
        dts_ = dts;
        pts_ = pts;
        prefixSize_ = prefix_size;
        this->codecId_ = CODEC_H264;
    }

    ~H264Frame() override {};

    bool KeyFrame() override
    {
        auto nalPtr = (uint8_t *)this->Data() + this->PrefixSize();
        return H264_TYPE(*nalPtr) == NAL_IDR && DecodeAble();
    }

    bool ConfigFrame() override
    {
        auto nalPtr = (uint8_t *)this->Data() + this->PrefixSize();
        switch (H264_TYPE(*nalPtr)) {
            case NAL_SPS: // fall-through
            case NAL_PPS:
                return true;
            default:
                return false;
        }
    }

    bool DropAble() override
    {
        auto nalPtr = (uint8_t *)this->Data() + this->PrefixSize();
        switch (H264_TYPE(*nalPtr)) {
            case NAL_SEI: // fall-through
            case NAL_AUD:
                return true;
            default:
                return false;
        }
    }

    bool DecodeAble() override
    {
        auto nalPtr = (uint8_t *)this->Data() + this->PrefixSize();
        auto type = H264_TYPE(*nalPtr);

        return type >= NAL_B_P && type <= NAL_IDR && (nalPtr[1] & 0x80);
    }
};
} // namespace Sharing
} // namespace OHOS
#endif