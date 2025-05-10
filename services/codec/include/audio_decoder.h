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

#ifndef OHOS_SHARING_AUDIO_DECODER_H
#define OHOS_SHARING_AUDIO_DECODER_H

#include "common/event_comm.h"
#include "common/sharing_log.h"
#include "media_frame_pipeline.h"

namespace OHOS {
namespace Sharing {
class AudioDecoder : public FrameSource,
                     public FrameDestination {
public:
    AudioDecoder() = default;
    virtual ~AudioDecoder() = default;

    virtual int32_t Init(const AudioTrack &audioTrack) = 0;

    virtual bool Start()
    {
        SHARING_LOGD("trace.");
        return true;
    }

    virtual void Stop()
    {
        SHARING_LOGD("trace.");
    }

    virtual void Release()
    {
        SHARING_LOGD("trace.");
    }

    virtual int64_t GetDecoderTimestamp()
    {
        return 0;
    }

    virtual void DropOneFrame()
    {
        SHARING_LOGD("trace.");
    }

public:
    bool inited_ = false;
};
} // namespace Sharing
} // namespace OHOS
#endif