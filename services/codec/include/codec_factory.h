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

#ifndef OHOS_SHARING_CODEC_FACTORY_H
#define OHOS_SHARING_CODEC_FACTORY_H

#include <memory>
#include "audio_decoder.h"
#include "audio_encoder.h"
#include "media_frame_pipeline.h"
#include "video_decoder.h"
#include "video_encoder.h"

namespace OHOS {
namespace Sharing {
class CodecFactory {
public:
    static std::shared_ptr<AudioEncoder> CreateAudioEncoder(CodecId format);
    static std::shared_ptr<AudioDecoder> CreateAudioDecoder(CodecId format);
    static std::shared_ptr<VideoEncoder> CreateVideoEncoder(CodecId format);
    static std::shared_ptr<VideoDecoder> CreateVideoDecoder(CodecId format);
};
} // namespace Sharing
} // namespace OHOS
#endif