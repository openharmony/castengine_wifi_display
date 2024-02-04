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

#include "codec_factory.h"
#include "audio_aac_codec.h"
#include "audio_g711_codec.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
std::shared_ptr<AudioEncoder> CodecFactory::CreateAudioEncoder(CodecId format)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<AudioEncoder> encoder;

    switch (format) {
        case CODEC_G711A:
            break;
        case CODEC_G711U:
            break;
        default:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
    }

    return encoder;
}

std::shared_ptr<AudioDecoder> CodecFactory::CreateAudioDecoder(CodecId format)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<AudioDecoder> decoder;
    switch (format) {
        case CODEC_G711A:
            decoder.reset(new AudioG711Decoder(G711_TYPE::G711_ALAW));
            break;
        case CODEC_G711U:
            decoder.reset(new AudioG711Decoder(G711_TYPE::G711_ULAW));
            break;
        case CODEC_AAC:
            decoder.reset(new AudioAACDecoder());
            break;
        default:
            SHARING_LOGE("unsupported codec format %{public}d.", (int32_t)format);
            break;
    }

    return decoder;
}

std::shared_ptr<VideoEncoder> CodecFactory::CreateVideoEncoder(CodecId format)
{
    SHARING_LOGD("trace.");
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
}

std::shared_ptr<VideoDecoder> CodecFactory::CreateVideoDecoder(CodecId format)
{
    SHARING_LOGD("trace.");
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
}
} // namespace Sharing
} // namespace OHOS
