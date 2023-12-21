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

#ifndef OHOS_SHARING_AUDIO_PLAY_CONTROLLER_H
#define OHOS_SHARING_AUDIO_PLAY_CONTROLLER_H

#include <thread>
#include "buffer_dispatcher.h"
#include "common/event_comm.h"
#include "mediaplayer/include/audio_player.h"


namespace OHOS {
namespace Sharing {

class AudioPlayController {
public:
    explicit AudioPlayController(uint32_t mediaChannelId);
    ~AudioPlayController();

    void Release();
    void SetVolume(float volume);
    void Stop(BufferDispatcher::Ptr &dispatcher);
    bool Init(AudioTrack &audioTrack);
    bool Start(BufferDispatcher::Ptr &dispatcher);

protected:
    void StopAudioThread();
    void AudioPlayThread();
    void StartAudioThread();

private:
    uint32_t mediachannelId_ = 0;
    std::atomic_bool isAudioRunning_ = false;
    std::shared_ptr<AudioPlayer> audioPlayer_ = nullptr;
    std::shared_ptr<std::thread> audioPlayThread_ = nullptr;
    std::shared_ptr<BufferReceiver> bufferReceiver_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif