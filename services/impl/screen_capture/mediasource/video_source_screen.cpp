/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "video_source_screen.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "common/const_def.h"
#include "display_manager.h"
#include "display_type.h"
#include "media_log.h"
#include "screen.h"
#include "screen_manager.h"
#include "sharing_log.h"
#include "sync_fence.h"

namespace OHOS {
namespace Sharing {
using namespace std;
static constexpr uint32_t MIN_QUEUE_SIZE = 3;
static constexpr uint32_t IM_STATUS_SUCCESS = 0;
static constexpr uint32_t MILLI_PER_SECOND = 1000;

static BufferRequestConfig g_requestConfig = {.width = DEFAULT_VIDEO_WIDTH,
                                              .height = DEFAULT_VIDEO_HEIGHT,
                                              .strideAlignment = 8,
                                              .format = PIXEL_FMT_RGBA_8888,
                                              .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA,
                                              .timeout = 0};

static BufferFlushConfig g_flushConfig = {
    .damage =
        {
            .x = 0,
            .y = 0,
            .w = DEFAULT_VIDEO_WIDTH,
            .h = DEFAULT_VIDEO_HEIGHT,
        },
    .timestamp = 0,
};

static uint32_t CopySurfaceBuffer(sptr<OHOS::SurfaceBuffer> srcBuffer, sptr<OHOS::SurfaceBuffer> dstBuffer)
{
    SHARING_LOGD("trace.");
    return IM_STATUS_SUCCESS;
}

void VideoSourceScreen::ScreenGroupListener::OnChange(const std::vector<uint64_t> &screenIds,
                                                      Rosen::ScreenGroupChangeEvent event)
{
    SHARING_LOGD("trace.");
    switch (event) {
        case Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP:
            SHARING_LOGD("ADD_TO_GROUP done!");
            break;
        case Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP:
            SHARING_LOGD("REMOVE_FROM_GROUP done!");
            break;
        case Rosen::ScreenGroupChangeEvent::CHANGE_GROUP:
            SHARING_LOGD("CHANGE_GROUP done!");
            break;
        default:
            break;
    }
}

void VideoSourceScreen::ScreenBufferConsumerListener::OnBufferAvailable()
{
    SHARING_LOGD("trace.");
    if (auto parent = parent_.lock()) {
        parent->OnScreenBufferAvailable();
    }
}

VideoSourceScreen::~VideoSourceScreen()
{
    SHARING_LOGD("trace.");

    std::lock_guard<std::mutex> lk(frameRateCtrlMutex_);
    if (lastBuffer_) {
        consumerSurface_->ReleaseBuffer(lastBuffer_, SyncFence::INVALID_FENCE);
    }
    lastBuffer_ = nullptr;
    encoderSurface_ = nullptr;
    producerSurface_ = nullptr;
    consumerSurface_ = nullptr;
}

void VideoSourceScreen::FrameRateControlTimerWorker()
{
    int32_t fence = -1;
    sptr<OHOS::SurfaceBuffer> encSurfBuffer = nullptr;

    std::unique_lock<std::mutex> lock(frameRateCtrlMutex_);
    if (lastBuffer_ == nullptr && lastEncFrameBufferSeq_ == INVALID_SEQ) {
        return;
    }
    lock.unlock();

    GSError ret = encoderSurface_->RequestBuffer(encSurfBuffer, fence, g_requestConfig);
    if (ret != SURFACE_ERROR_OK) {
        SHARING_LOGE("RequestBuffer failed, ret:%{public}d!", ret);
        return;
    }

    OHOS::sptr<SyncFence> encFence = new SyncFence(fence);
    encFence->Wait(40); // 40: timeout
    if (encSurfBuffer == nullptr) {
        SHARING_LOGE("RequestBuffer failed, encSurfBuffer is null!");
        return;
    }

    lock.lock();
    if (lastBuffer_ != nullptr) {
        uint32_t result = CopySurfaceBuffer(lastBuffer_, encSurfBuffer);
        if (result != IM_STATUS_SUCCESS) {
            SHARING_LOGE("CopySurfaceBuffer failed, error code: %{public}d!", ret);
        }

        if (consumerSurface_->ReleaseBuffer(lastBuffer_, SyncFence::INVALID_FENCE) != SURFACE_ERROR_OK) {
            SHARING_LOGE("ReleaseBuffer failed! seq:%{public}u!", lastBuffer_->GetSeqNum());
        }
        lastBuffer_ = nullptr;
        lock.unlock();

        if (encoderSurface_->FlushBuffer(encSurfBuffer, fence, g_flushConfig) == SURFACE_ERROR_OK) {
            lastEncFrameBufferSeq_ = encSurfBuffer->GetSeqNum();
            lastEncFrameBuffer_ = encSurfBuffer;
        } else {
            SHARING_LOGE("Flush new Surface Buffer failed!");
        }
        return;
    } else {
        lock.unlock();
        if (encSurfBuffer->GetSeqNum() != lastEncFrameBufferSeq_) {
            CopySurfaceBuffer(lastEncFrameBuffer_, encSurfBuffer);
        }
        if (encoderSurface_->FlushBuffer(encSurfBuffer, fence, g_flushConfig) != SURFACE_ERROR_OK) {
            SHARING_LOGE("Flush last Surface Buffer failed!");
        }
        return;
    }
}

void VideoSourceScreen::OnScreenBufferAvailable()
{
    SHARING_LOGD("trace.");
    int64_t timestamp = 0;
    OHOS::Rect damage;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    OHOS::sptr<SyncFence> screenFence = nullptr;
    std::unique_lock<std::mutex> lock(frameRateCtrlMutex_);
    if (consumerSurface_ == nullptr) {
        SHARING_LOGE("consumer_ is nullptr!");
        return;
    }
    consumerSurface_->AcquireBuffer(buffer, screenFence, timestamp, damage);
    if (buffer == nullptr || screenFence == nullptr) {
        SHARING_LOGE("AcquireBuffer failed!");
        return;
    }

    screenFence->Wait(40); // 40: sync timeout
    SHARING_LOGD("AcquireBuffer success! seq:%{public}d, time:%{public}" PRId64 ", fence:%{public}d.",
                 buffer->GetSeqNum(), timestamp, screenFence->Get());

    if (lastBuffer_ != nullptr) {
        if (consumerSurface_->ReleaseBuffer(lastBuffer_, SyncFence::INVALID_FENCE) != SURFACE_ERROR_OK) {
            SHARING_LOGE("ReleaseBuffer failed! seq:%{public}u!", lastBuffer_->GetSeqNum());
        }
    }
    lastBuffer_ = buffer;
}

int32_t VideoSourceScreen::InitScreenSource(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("trace.");
    if (encoderSurface_ == nullptr) {
        SHARING_LOGE("encoderSurface_ is null!");
        return ERR_GENERAL_ERROR;
    }

    consumerSurface_ = OHOS::Surface::CreateSurfaceAsConsumer();
    if (consumerSurface_ == nullptr) {
        SHARING_LOGE("CreateSurfaceAsConsumer failed!");
        return ERR_GENERAL_ERROR;
    }

    queueSzie_ = encoderSurface_->GetQueueSize();
    if (queueSzie_ < MIN_QUEUE_SIZE) {
        queueSzie_ = MIN_QUEUE_SIZE;
        encoderSurface_->SetQueueSize(queueSzie_);
    }
    consumerSurface_->SetQueueSize(queueSzie_);

    SHARING_LOGD("consumerSurface_ qid:%{public}" PRIu64 ", encoderSurface_ qid:%{public}" PRIu64 ".",
                 consumerSurface_->GetUniqueId(), encoderSurface_->GetUniqueId());

    auto bufferProducer = consumerSurface_->GetProducer();
    producerSurface_ = OHOS::Surface::CreateSurfaceAsProducer(bufferProducer);

    screenBufferListener_ = new ScreenBufferConsumerListener(shared_from_this());
    sptr<IBufferConsumerListener> bufferConsumerListener = move(screenBufferListener_);
    consumerSurface_->RegisterConsumerListener(bufferConsumerListener);

    timer_->Register(std::bind(&VideoSourceScreen::FrameRateControlTimerWorker, this),
                     MILLI_PER_SECOND / configure.frameRate_);
    timer_->Setup();
    SHARING_LOGD("interval::%{public}d.", MILLI_PER_SECOND / configure.frameRate_);
    RegisterScreenGroupListener();
    CreateVirtualScreen(configure);
    srcScreenId_ = configure.srcScreenId_;
    return ERR_OK;
}

int32_t VideoSourceScreen::ReleaseScreenBuffer() const
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t VideoSourceScreen::RegisterScreenGroupListener()
{
    SHARING_LOGD("trace.");
    if (screenGroupListener_ == nullptr) {
        screenGroupListener_ = new ScreenGroupListener();
    }
    auto ret = Rosen::ScreenManager::GetInstance().RegisterScreenGroupListener(screenGroupListener_);
    if (ret != OHOS::Rosen::DMError::DM_OK) {
        SHARING_LOGE("RegisterScreenGroupListener Failed!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Register successed!");
    return ERR_OK;
}

int32_t VideoSourceScreen::UnregisterScreenGroupListener() const
{
    SHARING_LOGD("trace.");
    auto ret = Rosen::ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupListener_);
    if (ret != OHOS::Rosen::DMError::DM_OK) {
        SHARING_LOGE("UnregisterScreenGroupListener Failed!");
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

uint64_t VideoSourceScreen::CreateVirtualScreen(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("CreateVirtualScreen, width: %{public}u, height: %{public}u.", configure.screenWidth_,
                 configure.screenHeight_);
    std::string screenName = "MIRACAST_HOME_SCREEN";
    Rosen::VirtualScreenOption option = {screenName,
                                         configure.screenWidth_,
                                         configure.screenHeight_,
                                         DEFAULT_SCREEN_DENSITY,
                                         producerSurface_,
                                         DEFAULT_SCREEN_FLAGS,
                                         false};

    screenId_ = Rosen::ScreenManager::GetInstance().CreateVirtualScreen(option);
    SHARING_LOGD("virtualScreen id is: %{public}" PRIu64 ".", screenId_);

    return screenId_;
}

int32_t VideoSourceScreen::DestroyVirtualScreen() const
{
    SHARING_LOGD("trace.");
    if (screenId_ == SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Destroy virtual screen, screenId: %{public}" PRIu64 ".", screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().DestroyVirtualScreen(screenId_);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Destroy virtual screen failed, screenId:%{public}" PRIu64 "!", screenId_);
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

void VideoSourceScreen::StartScreenSourceCapture()
{
    SHARING_LOGD("trace.");
    std::vector<uint64_t> mirrorIds;
    mirrorIds.push_back(screenId_);
    Rosen::ScreenId groupId = 0;
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().MakeMirror(srcScreenId_, mirrorIds, groupId);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("MakeMirror failed, screenId:%{public}" PRIu64 "!", screenId_);
    }
}

void VideoSourceScreen::StopScreenSourceCapture()
{
    SHARING_LOGD("trace.");
    UnregisterScreenGroupListener();
    consumerSurface_->UnregisterConsumerListener();
    if (timer_ != nullptr) {
        timer_->Shutdown();
        timer_.reset();
    }

    int32_t ret = DestroyVirtualScreen();
    if (ret != ERR_OK) {
        SHARING_LOGE("Destroy virtual screen failed!");
    } else {
        SHARING_LOGD("Destroy virtual screen success!");
    }
}

int32_t VideoSourceScreen::SetEncoderSurface(sptr<OHOS::Surface> surface)
{
    SHARING_LOGD("trace.");
    if (screenId_ == SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
    }
    if (surface == nullptr) {
        SHARING_LOGE("Surface is nullptr!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Set surface for virtual screen, screenId: %{public}" PRIu64 ".", screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().SetVirtualScreenSurface(screenId_, surface);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Set surface for virtual screen failed, screenId:%{public}" PRIu64 "!", screenId_);
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

void VideoSourceScreen::RemoveScreenFromGroup() const
{
    SHARING_LOGD("trace.");
    if (screenId_ != SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
    }
    SHARING_LOGD("Remove screen from group, screenId: %{publid}" PRIu64 ".", screenId_);
    std::vector<uint64_t> screenIds;
    screenIds.push_back(screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(screenIds);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Remove screen from group failed, screenId:%{public}" PRIu64 "!", screenId_);
    }
}
} // namespace Sharing
} // namespace OHOS
