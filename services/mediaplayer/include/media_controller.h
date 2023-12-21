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

#ifndef OHOS_SHARING_MEDIA_CONTROLLER_H
#define OHOS_SHARING_MEDIA_CONTROLLER_H

#include <thread>
#include "audio_play_controller.h"
#include "common/event_channel.h"
#include "mediachannel/buffer_dispatcher.h"
#include "video_play_controller.h"


namespace OHOS {
namespace Sharing {

class MediaChannel;

class MediaController : public std::enable_shared_from_this<MediaController> {
public:
    using Ptr = std::shared_ptr<MediaController>;

    explicit MediaController(uint32_t mediaChannelId);
    virtual ~MediaController();

    void SetMediaChannel(std::shared_ptr<MediaChannel> mediaChannel)
    {
        SHARING_LOGD("trace.");
        mediaChannel_ = mediaChannel;
    }

public:
    void Stop();
    void Start();
    void Release();
    bool Init(AudioTrack audioTrack, VideoTrack videoTrack);

    void RemoveSurface(uint64_t surfaceId);
    bool AppendSurface(sptr<Surface> surface, SceneType sceneType);

    void SetVolume(float volume);
    void SetKeyMode(uint64_t surfaceId, bool mode);
    void SetKeyRedirect(uint64_t surfaceId, bool keyRedirect);

    void OnPlayControllerNotify(ProsumerStatusMsg::Ptr &statusMsg);

private:
    uint32_t mediachannelId_ = 0;
    std::mutex playAudioMutex_;
    std::mutex playVideoMutex_;
    std::atomic_bool isPlaying_ = false;
    std::weak_ptr<MediaChannel> mediaChannel_;
    std::shared_ptr<AudioPlayController> audioPlayController_ = nullptr;
    std::map<uint64_t, std::shared_ptr<VideoPlayController>> videoPlayerMap_;

    AudioTrack audioTrack_;
    VideoTrack videoTrack_;
};

} // namespace Sharing
} // namespace OHOS
#endif