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

#include "audio_g711_codec.h"
#include "common/common_macro.h"
#include "frame.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
AudioG711Encoder::AudioG711Encoder(G711_TYPE type) : type_(type) {}

AudioG711Encoder::~AudioG711Encoder() {}

int32_t AudioG711Encoder::Init()
{
    SHARING_LOGD("trace.");
    inited_ = true;
    return 0;
}

void AudioG711Encoder::OnFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (!inited_ && frame->GetCodecId() != CODEC_PCM) {
        return;
    }

    auto payload = frame->Data();
    int32_t outLength = frame->Size() / 2; // 2: double size
    if ((int32_t)outBuffer_.size() != outLength) {
        outBuffer_.resize(outLength);
    }

    Encode((int16_t *)payload, outLength, outBuffer_.data());

    auto g711Frame = FrameImpl::Create();
    g711Frame->codecId_ = type_ == G711_ALAW ? CODEC_G711A : CODEC_G711U;
    g711Frame->Assign((char *)outBuffer_.data(), outBuffer_.size());
    DeliverFrame(g711Frame);
}

int32_t AudioG711Encoder::Encode(int16_t *decoded, int32_t nSamples, uint8_t *encoded)
{
    RETURN_INVALID_IF_NULL(decoded);
    RETURN_INVALID_IF_NULL(encoded);
    if (nSamples < 0) {
        return -1;
    }

    return nSamples;
}

AudioG711Decoder::AudioG711Decoder(G711_TYPE type) : type_(type) {}

AudioG711Decoder::~AudioG711Decoder() {}

int32_t AudioG711Decoder::Init()
{
    SHARING_LOGD("trace.");
    inited_ = true;
    return 0;
}

void AudioG711Decoder::OnFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (!inited_) {
        SHARING_LOGE("donot init!");
        return;
    }

    if ((type_ == G711_ALAW && frame->GetCodecId() != CODEC_G711A) ||
        (type_ == G711_ULAW && frame->GetCodecId() != CODEC_G711U)) {
        SHARING_LOGE("codecId is invalid!");
        return;
    }

    auto payload = frame->Data();
    int32_t length = frame->Size();
    if ((int32_t)outBuffer_.size() != length * 2) { // 2: double size
        outBuffer_.resize(length * 2); // 2: double size
    }

    Decode((uint8_t *)payload, length, (int16_t *)outBuffer_.data());
    auto pcmFrame = FrameImpl::Create();
    pcmFrame->codecId_ = CODEC_PCM;
    pcmFrame->Assign((char *)outBuffer_.data(), outBuffer_.size());
    DeliverFrame(pcmFrame);
};

int32_t AudioG711Decoder::Decode(uint8_t *encoded, int32_t nSamples, int16_t *decoded)
{
    RETURN_INVALID_IF_NULL(decoded);
    RETURN_INVALID_IF_NULL(encoded);
    if (nSamples < 0) {
        return -1;
    }

    return nSamples;
}
} // namespace Sharing
} // namespace OHOS