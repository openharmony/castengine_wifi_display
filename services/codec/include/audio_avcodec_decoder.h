/*
 * Copyright (c) 2025-2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#ifndef OHOS_SHARING_AUDIO_AVCODEC_DECODER_H
#define OHOS_SHARING_AUDIO_AVCODEC_DECODER_H

#include <queue>
#include "audio_decoder.h"
#include "avcodec_audio_decoder.h"

namespace OHOS {
namespace Sharing {
constexpr static uint32_t AUDIO_DECODE_WAIT_MILLISECONDS = 5000;
constexpr static uint32_t AUDIO_DECODE_DEFAULT_SAMPLERRATE = 48000;
constexpr static uint32_t AUDIO_DECODE_DEFAULT_CHANNEL_COUNT = 2;
constexpr static uint32_t ERROR_DECODER_INIT = -1;

class AudioAvCodecDecoder : public AudioDecoder,
                            public MediaAVCodec::AVCodecCallback {
public:
    explicit AudioAvCodecDecoder();
    virtual ~AudioAvCodecDecoder();

    bool Start() override;
    void Stop() override;
    void Release() override;
    int32_t Init(const AudioTrack &audioTrack) override;
    void OnFrame(const Frame::Ptr &frame) override;
    bool SetDecoderFormat(const AudioTrack &audiotrack);
    void OnOutputFormatChanged(const MediaAVCodec::Format &format) override;
    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode) override;
    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;
    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
                                 MediaAVCodec::AVCodecBufferFlag flag,
                                 std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;
    std::shared_ptr<MediaAVCodec::AVCodecAudioDecoder> GetDecoder();

private:
    bool InitDecoder();
    bool StopDecoder();
    bool StartDecoder();
    bool SetAudioCallback();

public:
    std::queue<std::pair<int32_t, std::shared_ptr<MediaAVCodec::AVSharedMemory>>> inBufferQueue_;
    std::mutex inputBufferMutex_;
    std::mutex decoderMutex_;
    std::condition_variable inCond_;
    std::atomic_bool isRunning_ = false;
    std::shared_ptr<MediaAVCodec::AVCodecAudioDecoder> audioDecoder_ = nullptr;
    CodecId audioCodecId_ = CODEC_NONE;
};
} // namespace Sharing
} // namespace OHOS
#endif