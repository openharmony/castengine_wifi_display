/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "sink_codec_factory.h"
#ifdef WIFI_DISPLAY_SINK
#include "audio_aac_decoder.h"
#include "audio_avcodec_decoder.h"
#include "audio_g711_decoder.h"
#endif
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
std::shared_ptr<AudioDecoder> SinkCodecFactory::CreateAudioDecoder(CodecId format)
{
    SHARING_LOGD("trace.");
#ifdef WIFI_DISPLAY_SINK
    std::shared_ptr<AudioDecoder> decoder;
    switch (format) {
        case CODEC_G711A:
            decoder.reset(new AudioG711Decoder(G711_TYPE::G711_ALAW));
            break;
        case CODEC_G711U:
            decoder.reset(new AudioG711Decoder(G711_TYPE::G711_ULAW));
            break;
        case CODEC_AAC:
            decoder.reset(new AudioAvCodecDecoder());
            break;
        default:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
    }

    return decoder;
#else
    SHARING_LOGE("sink is not supported.");
    return nullptr;
#endif
}

std::shared_ptr<VideoDecoder> SinkCodecFactory::CreateVideoDecoder(CodecId format)
{
    SHARING_LOGD("trace.");
#ifdef WIFI_DISPLAY_SINK
    std::shared_ptr<VideoDecoder> decoder;
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

    return decoder;
#else
    SHARING_LOGE("sink is not supported.");
    return nullptr;
#endif
}
} // namespace Sharing
} // namespace OHOS