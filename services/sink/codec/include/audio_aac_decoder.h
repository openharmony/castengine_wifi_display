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

#ifndef OHOS_SHARING_AUDIO_AAC_DECODER_H
#define OHOS_SHARING_AUDIO_AAC_DECODER_H

#include <memory>
#include <stdint.h>
#include <string.h>
#include "audio_decoder.h"
#include "media_frame_pipeline.h"
#include "protocol/frame/h264_frame.h"
#include "utils/data_buffer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
};

namespace OHOS {
namespace Sharing {
class AudioAACDecoder : public AudioDecoder {
public:
    AudioAACDecoder();
    ~AudioAACDecoder();

    int32_t Init(const AudioTrack &audioTrack) override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    uint8_t *swrOutBuffer_ = nullptr;
    int32_t swrOutBufferSize_ = 0;

    std::unique_ptr<DataBuffer> buffer_;

    AVFrame *avFrame_ = nullptr;
    AVPacket *avPacket_ = nullptr;
    AVCodecContext *codecCtx_ = nullptr;
    SwrContext *swrContext_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif