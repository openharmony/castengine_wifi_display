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

#ifndef OHOS_SHARING_AUDIO_SINK_H
#define OHOS_SHARING_AUDIO_SINK_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <cstdint>
#include <thread>
#include "audio_renderer.h"

namespace OHOS {
namespace Sharing {

enum PlayerMessageType {
    MEDIA_PLAYER_INFO_UNKNOWN,
};

enum PlayOnInfoType { INFO_TYPE_STATE_CHANGE, INFO_TYPE_VOLUME_CHANGE, INFO_TYPE_RESOLUTION_CHANGE };

enum PlayerState {
    PLAYER_STATE_ERROR,
    PLAYER_STATE_IDLE,
    PLAYER_STATE_INITIALIZED,
    PLAYER_STATE_PREPARING,
    PLAYER_STATE_PREPARED,
    PLAYER_STATE_STARTED,
    PLAYER_STATE_PAUSED,
    PLAYER_STATE_STOPPED,
    PLAYER_STATE_COMPLETE
};

enum PlayerErrorType {
    PLAYER_SUCCESS = 0,
    PLAYER_ERROR_UNKNOWN,
    PLAYER_ERROR_INVALID_STATE,
    PLAYER_ERROR_INVALID_PARAMS,
    PLAYER_ERROR_EMPTY_INSTANCE
};

class AudioSink {
public:
    AudioSink(uint32_t playerId);
    virtual ~AudioSink();

    int32_t Prepare();
    int32_t Prepare(int32_t channels, int32_t sampleRate);
    int32_t Stop();
    int32_t Start();
    int32_t Pause();
    int32_t Drain();
    int32_t Flush();
    int32_t Release();
    void RendererThread();

    int32_t SetVolume(float volume);
    int32_t SetParameters(int32_t bitsPerSample, int32_t channels, int32_t sampleRate);

    int32_t GetVolume(float &volume);
    int32_t GetBufferSize(int32_t &bufferSize);
    int32_t GetParameters(int32_t &bitsPerSample, int32_t &channels, int32_t &sampleRate);

    int32_t Write(uint8_t *buffer, size_t size);

private:
    bool running_ = false;
    uint32_t playerId_ = -1;

    std::mutex mutex_;
    std::unique_ptr<OHOS::AudioStandard::AudioRenderer> audioRenderer_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif