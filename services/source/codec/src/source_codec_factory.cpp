/*
 * Copyright (c) 2023-2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "source_codec_factory.h"
#ifdef WIFI_DISPLAY_SOURCE
#include "audio_aac_encoder.h"
#include "audio_g711_encoder.h"
#include "audio_pcm_processor.h"
#endif
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
std::shared_ptr<AudioEncoder> SourceCodecFactory::CreateAudioEncoder(CodecId format)
{
    SHARING_LOGD("trace.");
#ifdef WIFI_DISPLAY_SOURCE
    std::shared_ptr<AudioEncoder> encoder;

    switch (format) {
        case CODEC_G711A:
            encoder.reset(new AudioG711Encoder(G711_TYPE::G711_ALAW));
            break;
        case CODEC_G711U:
            encoder.reset(new AudioG711Encoder(G711_TYPE::G711_ULAW));
            break;
        case CODEC_AAC:
            encoder.reset(new AudioAACEncoder());
            break;
        case CODEC_PCM:
            encoder.reset(new AudioPcmProcessor());
            break;
        default:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
    }

    return encoder;
#else
    SHARING_LOGE("source is not supported.");
    return nullptr;
#endif
}

std::shared_ptr<VideoEncoder> SourceCodecFactory::CreateVideoEncoder(CodecId format)
{
    SHARING_LOGD("trace.");
#ifdef WIFI_DISPLAY_SOURCE
    std::shared_ptr<VideoEncoder> encoder;
    switch (format) {
        case CODEC_AAC:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
        case CODEC_G711U:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
        default:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
    }

    return encoder;
#else
    SHARING_LOGE("source is not supported.");
    return nullptr;
#endif
}
} // namespace Sharing
} // namespace OHOS