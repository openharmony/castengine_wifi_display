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

#ifndef OHOS_SHARING_AUDIO_SINK_DECODER_H
#define OHOS_SHARING_AUDIO_SINK_DECODER_H

#include "codec/include/codec_factory.h"
#include "common/const_def.h"
#include "common/identifier.h"
#include "common/media_log.h"
#include "mediaplayer/include/audio_sink.h"
#include "utils/data_buffer.h"


namespace OHOS {
namespace Sharing {

class AudioDecoderReceiver : public FrameDestination {
public:
    AudioDecoderReceiver(std::shared_ptr<AudioSink> &audioSink) : sink_(audioSink){};

    void OnFrame(const Frame::Ptr &frame) override
    {
        MEDIA_LOGD("trace.");
        auto sink = sink_.lock();
        if (sink != nullptr && frame) {
            sink->Write((uint8_t *)frame->Data(), frame->Size());
        }
    }

private:
    std::weak_ptr<AudioSink> sink_;
};

class AudioPlayer : public IdentifierInfo {
public:
    virtual ~AudioPlayer();

    void Stop();
    void Release();
    void SetVolume(float volume);
    void ProcessAudioData(DataBuffer::Ptr data);
    bool Start();
    bool Init(CodecId audioCodecId = CODEC_G711A);
    bool SetAudioFormat(int32_t channel = DEFAULT_CHANNEL, int32_t sampleRate = DEFAULT_SAMPLERATE);

private:
    uint32_t playerId_ = -1;
    int32_t channel_ = DEFAULT_CHANNEL;
    int32_t sampleRate_ = DEFAULT_SAMPLERATE;

    std::atomic_bool isRunning_ = false;
    std::shared_ptr<AudioSink> audioSink_ = nullptr;
    std::shared_ptr<AudioDecoder> audioDecoder_ = nullptr;
    std::shared_ptr<AudioDecoderReceiver> audioDecoderReceiver_ = nullptr;

    CodecId audioCodecId_ = CODEC_NONE;
};

} // namespace Sharing
} // namespace OHOS
#endif