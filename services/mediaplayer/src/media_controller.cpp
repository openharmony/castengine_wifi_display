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

#include "media_controller.h"
#include <chrono>
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "configuration/include/config.h"
#include "magic_enum.hpp"
#include "media_channel.h"
#include "protocol/frame/h264_frame.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {

MediaController::MediaController(uint32_t mediaChannelId)
{
    SHARING_LOGD("trace.");
    mediachannelId_ = mediaChannelId;
}

MediaController::~MediaController()
{
    SHARING_LOGD("trace.");
    Release();
    SHARING_LOGD("delete media controller, mediachannelId: %{public}u.", mediachannelId_);
}

bool MediaController::Init(AudioTrack audioTrack, VideoTrack videoTrack)
{
    SHARING_LOGD("trace.");
    audioTrack_ = audioTrack;
    videoTrack_ = videoTrack;
    if (CODEC_NONE == audioTrack_.codecId && CODEC_NONE == videoTrack_.codecId) {
        SHARING_LOGW("no need to play.");
        return false;
    }

    if (CODEC_NONE != audioTrack_.codecId) {
        audioPlayController_ = std::make_shared<AudioPlayController>(mediachannelId_);
        if (!audioPlayController_->Init(audioTrack_)) {
            SHARING_LOGE("audio play init error.");
            return false;
        }
    }

    return true;
}

void MediaController::Start()
{
    SHARING_LOGD("trace.");
    auto mediaChannel = mediaChannel_.lock();
    RETURN_IF_NULL(mediaChannel);
    SHARING_LOGI("media play start, mediachannelId: %{public}u.", mediachannelId_);
    if (nullptr == audioPlayController_ && 0 == videoPlayerMap_.size()) {
        SHARING_LOGE("start must be after init.");
        return;
    }

    auto dispatcher = mediaChannel->GetDispatcher();
    RETURN_IF_NULL(dispatcher);
    {
        std::lock_guard<std::mutex> lock(playAudioMutex_);
        if (nullptr != audioPlayController_) {
            if (audioPlayController_->Start(dispatcher)) {
                isPlaying_ = true;
            }
        }
    }

    if (videoPlayerMap_.size() > 0) {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        for (auto &item : videoPlayerMap_) {
            if (item.second->Start(dispatcher)) {
                isPlaying_ = true;
            }
        }
    }

    SHARING_LOGI("media play start done, mediachannelId: %{public}u.", mediachannelId_);
}

void MediaController::Stop()
{
    SHARING_LOGD("trace.");
    auto mediaChannel = mediaChannel_.lock();
    RETURN_IF_NULL(mediaChannel);
    SHARING_LOGI("media play stop, mediachannelId: %{public}u.", mediachannelId_);
    auto dispatcher = mediaChannel->GetDispatcher();
    RETURN_IF_NULL(dispatcher);
    {
        std::lock_guard<std::mutex> lock(playAudioMutex_);
        if (isPlaying_ && (nullptr != audioPlayController_)) {
            audioPlayController_->Stop(dispatcher);
        }
    }

    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        if (isPlaying_) {
            for (auto &item : videoPlayerMap_) {
                item.second->Stop(dispatcher);
            }
        }
    }

    isPlaying_ = false;
    SHARING_LOGI("media play stop done, mediachannelId: %{public}u.", mediachannelId_);
}

void MediaController::Release()
{
    SHARING_LOGD("trace.");
    if (nullptr != audioPlayController_) {
        audioPlayController_->Release();
        audioPlayController_.reset();
    }

    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        for (auto &item : videoPlayerMap_) {
            item.second->Release();
        }
        videoPlayerMap_.clear();
    }

    SHARING_LOGI("media play release done, mediachannelId: %{public}u.", mediachannelId_);
}

bool MediaController::AppendSurface(sptr<Surface> surface, SceneType sceneType)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(surface);
    if (CODEC_NONE == videoTrack_.codecId) {
        SHARING_LOGW("no need to play video.");
        return false;
    }

    auto videoPlayController = std::make_shared<VideoPlayController>(mediachannelId_);
    uint64_t surfaceId = surface->GetUniqueId();
    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        if (videoPlayerMap_.count(surfaceId)) {
            SHARING_LOGE("surface is in use, %{public}" PRIx64 ".", surfaceId);
            return false;
        }

        bool keyFrame = sceneType == SceneType::BACKGROUND ? true : false;
        if (videoPlayController->Init(videoTrack_) && videoPlayController->SetSurface(surface, keyFrame)) {
            videoPlayerMap_.emplace(surfaceId, videoPlayController);
        } else {
            SHARING_LOGD("videoPlayController init failed, mediachannelId: %{public}u.", mediachannelId_);
            return false;
        }

        if (isPlaying_) {
            auto mediaChannel = mediaChannel_.lock();
            auto dispatcher = mediaChannel->GetDispatcher();
            videoPlayController->Start(dispatcher);
        }

        videoPlayController->SetMediaController(shared_from_this());
        SHARING_LOGI("media play append surface done, mediachannelId: %{public}u.", mediachannelId_);
    }

    return true;
}

void MediaController::RemoveSurface(uint64_t surfaceId)
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        auto videoPlayController = videoPlayerMap_.find(surfaceId);
        if (videoPlayController != videoPlayerMap_.end()) {
            auto mediaChannel = mediaChannel_.lock();
            auto dispatcher = mediaChannel->GetDispatcher();
            videoPlayController->second->Stop(dispatcher);
            videoPlayController->second->Release();
            videoPlayerMap_.erase(surfaceId);
        }
        SHARING_LOGI("media play remove surface done, mediachannelId: %{public}u.", mediachannelId_);
    }
}

void MediaController::SetVolume(float volume)
{
    SHARING_LOGD("Volume: %{public}f.", volume);
    if (audioPlayController_) {
        audioPlayController_->SetVolume(volume);
    }
}

void MediaController::SetKeyMode(uint64_t surfaceId, bool mode)
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        auto videoPlayController = videoPlayerMap_.find(surfaceId);
        if (videoPlayController != videoPlayerMap_.end()) {
            videoPlayController->second->SetKeyMode(mode);
        }
        SHARING_LOGI("media play set key mode done, mediachannelId: %{public}u.", mediachannelId_);
    }
}

void MediaController::SetKeyRedirect(uint64_t surfaceId, bool keyRedirect)
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(playVideoMutex_);
        auto videoPlayController = videoPlayerMap_.find(surfaceId);
        if (videoPlayController != videoPlayerMap_.end()) {
            videoPlayController->second->SetKeyRedirect(keyRedirect);
        }
        SHARING_LOGI("media play set key redirect done, mediachannelId: %{public}u.", mediachannelId_);
    }
}

void MediaController::OnPlayControllerNotify(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    auto mediaChannel = mediaChannel_.lock();
    RETURN_IF_NULL(mediaChannel);

    statusMsg->agentId = mediaChannel->GetSinkAgentId();
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

} // namespace Sharing
} // namespace OHOS