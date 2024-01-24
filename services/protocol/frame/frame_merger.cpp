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

#include "frame_merger.h"
#include <arpa/inet.h>

namespace OHOS {
namespace Sharing {
static size_t constexpr MAX_FRAME_CACHE_SIZE = 100;

void FrameMerger::Clear()
{
    frameCache_.clear();
    haveDecodeAbleFrame_ = false;
}

void FrameMerger::SetType(int32_t type)
{
    type_ = type;
}

bool FrameMerger::InputFrame(const Frame::Ptr &frame, DataBuffer::Ptr &buffer, const onOutput &cb)
{
    if (buffer == nullptr) {
        return false;
    }

    if (WillFlush(frame)) {
        if (frameCache_.empty()) {
            return false;
        }
        Frame::Ptr back = frameCache_.back();
        bool haveKeyFrame = back->KeyFrame();
        if (frameCache_.size() != 1 || type_ == MP4_NAL_SIZE || buffer) {
            DataBuffer::Ptr &merged = buffer;

            for (auto &&vframe : frameCache_) {
                DoMerge(merged, vframe);
                if (vframe->KeyFrame()) {
                    haveKeyFrame = true;
                }
            }
        }
        cb(back->Dts(), back->Pts(), buffer, haveKeyFrame);
        frameCache_.clear();
        haveDecodeAbleFrame_ = false;
    }

    if (!frame) {
        return false;
    }

    if (frame->DecodeAble()) {
        haveDecodeAbleFrame_ = true;
    }
    frameCache_.emplace_back(frame);
    return true;
}

bool FrameMerger::WillFlush(const Frame::Ptr &frame) const
{
    if (frameCache_.empty()) {
        return false;
    }
    if (!frame) {
        return true;
    }
    switch (type_) {
        case NONE: {
            bool newFrame = false;
            switch (frame->GetCodecId()) {
                case CODEC_H264:
                case CODEC_H265: {
                    newFrame = frame->PrefixSize();
                    break;
                }
                default:
                    break;
            }

            return newFrame || frameCache_.back()->Dts() != frame->Dts() || frameCache_.size() > MAX_FRAME_CACHE_SIZE;
        }

        case MP4_NAL_SIZE:
        case H264_PREFIX: {
            if (!haveDecodeAbleFrame_) {
                return frameCache_.size() > MAX_FRAME_CACHE_SIZE;
            }
            if (frameCache_.back()->Dts() != frame->Dts() || frame->DecodeAble() || frame->ConfigFrame()) {
                return true;
            }

            return frameCache_.size() > MAX_FRAME_CACHE_SIZE;
        }
        default:
            return false;
    }
}

void FrameMerger::DoMerge(DataBuffer::Ptr &merged, const Frame::Ptr &frame) const
{
    switch (type_) {
        case NONE: {
            merged->Append(frame->Data(), frame->Size());
            break;
        }
        case H264_PREFIX: {
            if (frame->PrefixSize()) {
                merged->Append(frame->Data(), frame->Size());
            } else {
                merged->Append("\x00\x00\x00\x01", 4); // 4:avc start code size
                merged->Append(frame->Data(), frame->Size());
            }
            break;
        }
        case MP4_NAL_SIZE: {
            uint32_t naluSize = (uint32_t)(frame->Size() - frame->PrefixSize());
            naluSize = htonl(naluSize);
            merged->Append((char *)&naluSize, 4); // 4:avc start code size
            merged->Append(frame->Data() + frame->PrefixSize(), frame->Size() - frame->PrefixSize());
            break;
        }
        default:
            break;
    }
}
} // namespace Sharing
} // namespace OHOS