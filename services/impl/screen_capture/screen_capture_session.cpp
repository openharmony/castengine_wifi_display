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

#include "screen_capture_session.h"
#include "common/common.h"
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "screen_capture_def.h"

namespace OHOS {
namespace Sharing {
ScreenCaptureSession::ScreenCaptureSession()
{
    SHARING_LOGD("id: %{public}u.", GetId());
}

ScreenCaptureSession::~ScreenCaptureSession()
{
    SHARING_LOGD("id: %{public}u.", GetId());
}

int32_t ScreenCaptureSession::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);

    SHARING_LOGI("eventType: %{public}s, capture sessionId: %{public}u.",
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
    switch (event.eventMsg->type) {
        case EventType::EVENT_SESSION_INIT:
            HandleSessionInit(event);
            break;
        case EventType::EVENT_AGENT_STATE_PROSUMER_INIT:
            HandleProsumerInitState(event);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return 0;
}

void ScreenCaptureSession::HandleSessionInit(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto inputMsg = ConvertEventMsg<ScreenCaptureSessionEventMsg>(event);
    if (inputMsg) {
        captureType_ = inputMsg->mediaType;
        videoFormat_ = inputMsg->videoFormat;
        audioFormat_ = inputMsg->audioFormat;
        screenId_ = inputMsg->screenId;
        SHARING_LOGI("videoFormat_: %{public}d, type: %{public}d, screenId: %{public}" SCNu64 ".", videoFormat_,
                     captureType_, screenId_);
    } else {
        SHARING_LOGE("unknow event msg.");
    }
}

void ScreenCaptureSession::HandleProsumerInitState(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto inputMsg = ConvertEventMsg<ScreenCaptureSessionEventMsg>(event);
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->status = STATE_SESSION_ERROR;
    statusMsg->msg->errorCode = ERR_SESSION_START;

    if (inputMsg) {
        statusMsg->msg->requestId = inputMsg->requestId;

        if (inputMsg->errorCode == ERR_OK) {
            statusMsg->status = STATE_SESSION_STARTED;
            statusMsg->msg->errorCode = ERR_OK;
        } else {
            SHARING_LOGE("consumer inited failed, id: %{public}u.", inputMsg->prosumerId);
        }
    } else {
        SHARING_LOGE("consumer inited failed: unknow msg.");
        return;
    }

    NotifyAgentSessionStatus(statusMsg);
}

void ScreenCaptureSession::UpdateOperation(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    status_ = static_cast<SessionRunningStatus>(statusMsg->status);
    SHARING_LOGI("status: %{public}s.", std::string(magic_enum::enum_name(status_)).c_str());
    switch (status_) {
        case SESSION_START:
            statusMsg->status = NOTIFY_PROSUMER_CREATE;
            break;
        case SESSION_STOP:
            statusMsg->status = STATE_SESSION_STOPED;
            break;
        case SESSION_PAUSE:
            statusMsg->status = STATE_SESSION_PAUSED;
            break;
        case SESSION_RESUME:
            statusMsg->status = STATE_SESSION_RESUMED;
            break;
        case SESSION_DESTROY:
            statusMsg->status = STATE_SESSION_DESTROYED;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    NotifyAgentSessionStatus(statusMsg);
}

void ScreenCaptureSession::NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    auto eventMsg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    eventMsg->type = EventType::EVENT_SCREEN_CAPTURE_INIT;
    eventMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    eventMsg->screenId = screenId_;
    switch (captureType_) {
        case MEDIA_TYPE_AV:
            Common::SetVideoTrack(eventMsg->videoTrack, videoFormat_);
            Common::SetAudioTrack(eventMsg->audioTrack, audioFormat_);
            break;
        case MEDIA_TYPE_VIDEO:
            Common::SetVideoTrack(eventMsg->videoTrack, videoFormat_);
            break;
        case MEDIA_TYPE_AUDIO:
            Common::SetAudioTrack(eventMsg->audioTrack, audioFormat_);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
    SHARING_LOGI("afer SetVideoTrack, vtype:%{public}d, vFormat:%{public}d, vcodecId:%{public}d.", captureType_,
                 videoFormat_, eventMsg->videoTrack.codecId);
    statusMsg->msg = std::move(eventMsg);
    statusMsg->status = NOTIFY_SESSION_PRIVATE_EVENT;

    NotifyAgentSessionStatus(statusMsg);
}

void ScreenCaptureSession::UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    SHARING_LOGI("update media notify status: %{public}s.",
                 std::string(magic_enum::enum_name(static_cast<MediaNotifyStatus>(statusMsg->status))).c_str());
    switch (statusMsg->status) {
        case STATE_PROSUMER_CREATE_SUCCESS:
            NotifyProsumerInit(statusMsg);
            break;
        case STATE_PROSUMER_START_SUCCESS:
            break;
        case STATE_PROSUMER_STOP_SUCCESS:
            break;
        case STATE_PROSUMER_DESTROY_SUCCESS:
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void ScreenCaptureSession::NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);

    if (statusMsg->status == NOTIFY_PROSUMER_CREATE) {
        statusMsg->className = "ScreenCaptureConsumer";
    }

    Notify(statusMsg);
}

REGISTER_CLASS_REFLECTOR(ScreenCaptureSession);
} // namespace Sharing
} // namespace OHOS
