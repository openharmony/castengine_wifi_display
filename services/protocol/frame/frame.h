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

#ifndef OHOS_SHARING_FRAME_H
#define OHOS_SHARING_FRAME_H

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "common/const_def.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {
class CodecInfo {
public:
    using Ptr = std::shared_ptr<CodecInfo>;

    CodecInfo() = default;
    virtual ~CodecInfo() = default;

    TrackType GetTrackType();
    virtual CodecId GetCodecId() = 0;
};

class Frame : public DataBuffer,
              public CodecInfo {
public:
    using Ptr = std::shared_ptr<Frame>;

    Frame() = default;
    explicit Frame(DataBuffer &&dataBuffer) : DataBuffer(std::move(dataBuffer)) {}

    ~Frame() override {};

    virtual uint32_t Dts() = 0;

    virtual uint32_t Pts()
    {
        return Dts();
    }

    virtual bool KeyFrame() = 0;
    virtual bool ConfigFrame() = 0;
    virtual size_t PrefixSize() = 0;

    virtual bool CacheAble()
    {
        return true;
    }

    virtual bool DropAble()
    {
        return false;
    }

    virtual bool DecodeAble()
    {
        if (GetTrackType() != TRACK_VIDEO) {
            return true;
        }

        return !ConfigFrame();
    }
};

class FrameImpl : public Frame {
public:
    using Ptr = std::shared_ptr<FrameImpl>;

    static std::shared_ptr<FrameImpl> Create()
    {
        return std::make_shared<FrameImpl>();
    }

    explicit FrameImpl(DataBuffer &&dataBuffer) : Frame(std::move(dataBuffer)) {}

    static std::shared_ptr<FrameImpl> CreateFrom(DataBuffer &&dataBuffer)
    {
        return std::make_shared<FrameImpl>(std::move(dataBuffer));
    }

    uint32_t Dts() override
    {
        return dts_;
    }

    uint32_t Pts() override
    {
        return pts_ ? pts_ : dts_;
    }

    size_t PrefixSize() override
    {
        return prefixSize_;
    }

    CodecId GetCodecId() override
    {
        return codecId_;
    }

    bool KeyFrame() override
    {
        return false;
    }

    bool ConfigFrame() override
    {
        return false;
    }

    FrameImpl() = default;

public:
    uint32_t dts_ = 0;
    uint32_t pts_ = 0;

    size_t prefixSize_ = 0;
    CodecId codecId_ = CODEC_NONE;
};
} // namespace Sharing
} // namespace OHOS
#endif