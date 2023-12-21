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

#include "media_channel.h"
#include <chrono>
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "common/reflect_registration.h"
#include "configuration/include/config.h"
#include "magic_enum.hpp"
#include "mediachannel/channel_manager.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace Sharing {

MediaChannel::MediaChannel()
{
    SHARING_LOGD("mediachannelId: %{public}u.", GetId());
    SharingValue::Ptr values = nullptr;
    int32_t maxBufferCapacity = MAX_BUFFER_CAPACITY;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "bufferDispatcher", "maxBufferCapacity", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(maxBufferCapacity);
    }

    int32_t bufferCapacityIncrement = BUFFER_CAPACITY_INCREMENT;
    ret = Config::GetInstance().GetConfig("mediachannel", "bufferDispatcher", "bufferCapacityIncrement", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(bufferCapacityIncrement);
    }

    dispatcher_ = std::make_shared<BufferDispatcher>(maxBufferCapacity, bufferCapacityIncrement);
    playController_ = std::make_shared<MediaController>(GetId());
}

MediaChannel::~MediaChannel()
{
    SHARING_LOGD("mediachannelId: %{public}u.", GetId());
    if (consumer_) {
        consumer_.reset();
    }

    if (dispatcher_ != nullptr) {
        dispatcher_->StopDispatch();
        dispatcher_->FlushBuffer();
        dispatcher_->ReleaseAllReceiver();
        dispatcher_.reset();
    }

    SHARING_LOGD("leave, mediachannelId: %{public}u.", GetId());
}

void MediaChannel::OnWriteTimeout()
{
    SHARING_LOGD("trace.");
    auto msg = std::make_shared<EventMsg>();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = msg;
    statusMsg->errorCode = ERR_INTAKE_TIMEOUT;
    statusMsg->agentId = GetSinkAgentId();
    statusMsg->prosumerId = consumer_ ? consumer_->GetId() : INVALID_ID;

    SendAgentEvent(statusMsg, EVENT_AGENT_STATE_WRITE_WARNING);
}

uint32_t MediaChannel::GetSinkAgentId()
{
    SHARING_LOGD("trace.");
    if (consumer_)
        return consumer_->GetSinkAgentId();
    else
        return INVALID_ID;
}

void MediaChannel::OnProducerNotify(ProsumerStatusMsg::Ptr &msg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    RETURN_IF_NULL(msg->eventMsg);
    SHARING_LOGI("mediachannelId: %{public}u, producerId: %{public}u, status: %{public}s.", GetId(), msg->prosumerId,
                 std::string(magic_enum::enum_name(static_cast<ProsumerNotifyStatus>(msg->status))).c_str());
    switch (msg->status) {
        case ProsumerNotifyStatus::PROSUMER_NOTIFY_INIT_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_CREATE);
            break;
        case PROSUMER_NOTIFY_START_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_START);
            break;
        case PROSUMER_NOTIFY_STOP_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_STOP);
            break;
        case PROSUMER_NOTIFY_PAUSE_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_PAUSE);
            break;
        case PROSUMER_NOTIFY_RESUME_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_RESUME);
            break;
        case PROSUMER_NOTIFY_DESTROY_SUCCESS: {
            SHARING_LOGD("destroy producer, mediachannelId: %{public}u.", GetId());
            std::unique_lock<std::mutex> lock(mutex_);
            auto iter = producers_.find(msg->prosumerId);
            if (iter != producers_.end()) {
                producers_.erase(iter);
            }
            SHARING_LOGD("erase producer, mediachannelId: %{public}u.", GetId());
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_DESTROY);
            break;
        }
        case PROSUMER_NOTIFY_ERROR:
            SendAgentEvent(msg, EVENT_AGENT_PROSUMER_ERROR);
            break;
        case PROSUMER_NOTIFY_PRIVATE_EVENT: {
            SharingEvent agentEvent;
            agentEvent.eventMsg = msg->eventMsg;
            SendEvent(agentEvent);
            return;
        }
        default:
            break;
    }
}

void MediaChannel::OnConsumerNotify(ProsumerStatusMsg::Ptr &msg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    RETURN_IF_NULL(msg->eventMsg);
    SHARING_LOGI("mediachannelId: %{public}u, consumerId: %{public}u, status: %{public}s.", GetId(), msg->prosumerId,
                 std::string(magic_enum::enum_name(static_cast<ProsumerNotifyStatus>(msg->status))).c_str());

    switch (msg->status) {
        case ProsumerNotifyStatus::PROSUMER_NOTIFY_INIT_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_CREATE);
            break;
        case PROSUMER_NOTIFY_START_SUCCESS: {
            msg->errorCode = InitPlayController();
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_START);
            break;
        }
        case PROSUMER_NOTIFY_STOP_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_STOP);
            break;
        case PROSUMER_NOTIFY_PAUSE_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_PAUSE);
            break;
        case PROSUMER_NOTIFY_RESUME_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_RESUME);
            break;
        case PROSUMER_NOTIFY_DESTROY_SUCCESS:
            SendAgentEvent(msg, EVENT_AGENT_STATE_PROSUMER_DESTROY);
            break;
        case PROSUMER_NOTIFY_ERROR:
            SendAgentEvent(msg, EVENT_AGENT_PROSUMER_ERROR);
            break;
        case PROSUMER_NOTIFY_PRIVATE_EVENT: {
            SharingEvent agentEvent;
            agentEvent.eventMsg = msg->eventMsg;
            SendEvent(agentEvent);
            return;
        }
        default:
            break;
    }
}

void MediaChannel::OnMediaControllerNotify(ProsumerStatusMsg::Ptr &msg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    RETURN_IF_NULL(msg->eventMsg);
    SHARING_LOGI("mediachannelId: %{public}u, prosumerId: %{public}u, status: %{public}s.", GetId(), msg->prosumerId,
                 std::string(magic_enum::enum_name(static_cast<PlayConntrollerNotifyStatus>(msg->status))).c_str());

    switch (msg->status) {
        case CONNTROLLER_NOTIFY_ACCELERATION:
            SendAgentEvent(msg, EVENT_AGEINT_ACCELERATION_DONE);
            break;
        case CONNTROLLER_NOTIFY_DECODER_DIED:
            SendAgentEvent(msg, EVENT_AGENT_DECODER_DIED);
            break;
        case CONNTROLLER_NOTIFY_KEYMOD_START:
            SendAgentEvent(msg, EVENT_AGENT_KEYMODE_START);
            break;
        case CONNTROLLER_NOTIFY_KEYMOD_STOP:
            SendAgentEvent(msg, EVENT_AGENT_KEYMODE_STOP);
            break;
        default:
            break;
    }
}

uint32_t MediaChannel::CreateProducer(std::string &className)
{
    SHARING_LOGD("trace.");
    BaseProducer::Ptr producer = ClassReflector<BaseProducer>::Class2Instance(className);
    if (producer) {
        std::unique_lock<std::mutex> lock(mutex_);
        producer->SetProducerListener(shared_from_this());
        producers_[producer->GetId()] = producer;
        SHARING_LOGD("create producer success, mediachannelId: %{public}u.", GetId());
        return producer->GetId();
    }

    return INVALID_ID;
}

SharingErrorCode MediaChannel::CreateConsumer(std::string &className)
{
    SHARING_LOGD("trace.");
    if (consumer_) {
        consumer_.reset();
        SHARING_LOGW("create consumer. The consumer is reseted.");
    }
    consumer_ = ClassReflector<BaseConsumer>::Class2Instance(className);
    if (consumer_) {
        consumer_->SetConsumerListener(shared_from_this());
        if (dispatcher_) {
            dispatcher_->SetBufferDispatcherListener(shared_from_this());
        }
        SHARING_LOGD("create consumer success name: %{public}s, mediachannelId: %{public}u.", className.c_str(),
                     GetId());
        return SharingErrorCode::ERR_OK;
    }

    SHARING_LOGD("create consumer error name: %{public}s.", className.c_str());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

int32_t MediaChannel::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI(
        "HandleEvent mediachannelId: %{public}u, fromMgr: %{public}u, srcId: %{public}u, handle event: %{public}s.",
        event.eventMsg->dstId, event.eventMsg->fromMgr, event.eventMsg->srcId,
        std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());

    switch (event.eventMsg->type) {
        case EventType::EVENT_MEDIA_CONSUMER_CREATE:
            HandleCreateConsumer(event);
            break;
        case EventType::EVENT_MEDIA_CONSUMER_DESTROY:
            HandleDestroyConsumer(event);
            break;
        case EventType::EVENT_MEDIA_CONSUMER_START:
            HandleStartConsumer(event);
            break;
        case EventType::EVENT_MEDIA_CONSUMER_STOP:
            HandleStopConsumer(event);
            break;
        case EventType::EVENT_MEDIA_CONSUMER_PAUSE:
            HandlePauseConsumer(event);
            break;
        case EventType::EVENT_MEDIA_CONSUMER_RESUME:
            HandleResumeConsumer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_CREATE:
            HandleCreateProducer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_DESTROY:
            HandleDestroyProducer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_START:
            HandleStartProducer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_STOP:
            HandleStopProducer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_PAUSE:
            HandlePauseProducer(event);
            break;
        case EventType::EVENT_MEDIA_PRODUCER_RESUME:
            HandleResumeProducer(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_APPENDSURFACE:
            HandleAppendSurface(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_REMOVESURFACE:
            HandleRemoveSurface(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_SETSCENETYPE:
            HandleSetSceneType(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_SETVOLUME:
            HandleSetVolume(event);
            break;
        case EventType::EVENT_MEDIA_PLAY_START:
            HandleStartPlay(event);
            break;
        case EventType::EVENT_MEDIA_PLAY_STOP:
            HandleStopPlay(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_KEY_REDIRECT:
            HandleSetKeyRedirect(event);
            break;
        default: {
            auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
            if (channelMsg) {
                if (consumer_ && (consumer_->GetId() == channelMsg->prosumerId)) {
                    SHARING_LOGD("send event to consumer.");
                    consumer_->HandleEvent(event);
                } else if (producers_.find(channelMsg->prosumerId) != producers_.end()) {
                    SHARING_LOGD("send event to producer.");
                    producers_[channelMsg->prosumerId]->HandleEvent(event);
                    return SharingErrorCode::ERR_OK;
                } else {
                    SHARING_LOGW("unknow prosumerId: %{public}u.", channelMsg->prosumerId);
                }
            } else {
                SHARING_LOGW("unknown event msg.");
            }

            break;
        }
    }

    return 0;
}

int32_t MediaChannel::Release()
{
    SHARING_LOGD("trace, mediachannelId: %{public}u.", GetId());
    if (nullptr != playController_) {
        SHARING_LOGD("release playController, mediachannelId: %{public}u.", GetId());
        playController_->Release();
        playController_.reset();
    } else {
        SHARING_LOGD("playerController has been released, mediachannelId: %{public}u.", GetId());
    }
    if (nullptr != consumer_) {
        SHARING_LOGD("release Consumer, mediachannelId: %{public}u.", GetId());
        consumer_.reset();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    producers_.clear();
    SHARING_LOGD("end mediachannelId: %{public}u.", GetId());
    return 0;
}

void MediaChannel::SetMediaChannelListener(std::weak_ptr<IMediaChannelListener> listener)
{
    SHARING_LOGD("trace.");
    listener_ = listener;
}

SharingErrorCode MediaChannel::HandleAppendSurface(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelAppendSurfaceEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGD("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    if ((nullptr != channelMsg->surface) && playController_) {
        if (playController_->AppendSurface(channelMsg->surface, channelMsg->sceneType)) {
            statusMsg->errorCode = ERR_OK;
            statusMsg->prosumerId = consumer_->GetId();
            statusMsg->agentId = consumer_->GetSinkAgentId();
            SendAgentEvent(statusMsg, EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE);
            return ERR_OK;
        } else {
            SHARING_LOGW("AppendSurface failed.");
            statusMsg->errorCode = ERR_INVALID_SURFACE_ID;
        }
    } else {
        SHARING_LOGW("playerController or surface is nullptr.");
        statusMsg->errorCode = ERR_PLAY_START;
    }

    statusMsg->prosumerId = channelMsg->prosumerId;
    statusMsg->agentId = channelMsg->agentId;
    SendAgentEvent(statusMsg, EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE);
    return ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandleRemoveSurface(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    if (consumer_ == nullptr) {
        SHARING_LOGE("consumer is not inited.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto channelMsg = ConvertEventMsg<ChannelRemoveSurfaceEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    if (playController_) {
        playController_->RemoveSurface(channelMsg->surfaceId);
    } else {
        SHARING_LOGW("remove surface error, mediachannelId: %{public}u.", GetId());
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->errorCode = ERR_OK;
    statusMsg->prosumerId = consumer_->GetId();
    statusMsg->agentId = consumer_->GetSinkAgentId();
    statusMsg->surfaceId = channelMsg->surfaceId;
    SendAgentEvent(statusMsg, EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE);

    return SharingErrorCode::ERR_OK;
}

SharingErrorCode MediaChannel::HandleSetSceneType(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelSetSceneTypeEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    if (playController_) {
        if (channelMsg->sceneType == OHOS::Sharing::SceneType::FOREGROUND) {
            playController_->SetKeyMode(channelMsg->surfaceId, false);
        } else if (channelMsg->sceneType == OHOS::Sharing::SceneType::BACKGROUND) {
            playController_->SetKeyMode(channelMsg->surfaceId, true);
        }
    }

    SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
    return ERR_OK;
}

SharingErrorCode MediaChannel::HandleSetKeyRedirect(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelSetKeyRedirectEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    if (playController_) {
        playController_->SetKeyRedirect(channelMsg->surfaceId, channelMsg->keyRedirect);
    } else {
        SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
    }

    return ERR_OK;
}

SharingErrorCode MediaChannel::HandleSetVolume(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelSetVolumeEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    float volume = channelMsg->volume;
    if (playController_) {
        playController_->SetVolume(volume);
    } else {
        SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
    }

    return ERR_OK;
}

SharingErrorCode MediaChannel::HandleStartPlay(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->prosumerId = consumer_ ? consumer_->GetId() : channelMsg->prosumerId;
    statusMsg->agentId = channelMsg->agentId;

    if (playController_) {
        playController_->Start();
        statusMsg->errorCode = ERR_OK;
    } else {
        SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_PLAY_START;
    }

    SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PLAY_START);
    return ERR_OK;
}

SharingErrorCode MediaChannel::HandleStopPlay(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->prosumerId = consumer_ ? consumer_->GetId() : channelMsg->prosumerId;
    statusMsg->agentId = channelMsg->agentId;

    if (playController_) {
        playController_->Stop();
        statusMsg->errorCode = ERR_OK;
    } else {
        SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_PLAY_STOP;
    }

    SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PLAY_STOP);
    return ERR_OK;
}

SharingErrorCode MediaChannel::HandleCreateConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    if (CreateConsumer(channelMsg->className) == ERR_OK) {
        if (consumer_) {
            SHARING_LOGD("create consumer success, consumerId: %{public}u, mediachannelId: %{public}u.",
                         consumer_->GetId(), GetId());
            statusMsg->status = PROSUMER_INIT;
            consumer_->SetSinkAgentId(channelMsg->agentId);
            consumer_->UpdateOperation(statusMsg);
        }
        return ERR_OK;
    } else {
        SHARING_LOGW("create consumer failed.");
        statusMsg->errorCode = ERR_PROSUMER_CREATE;
        statusMsg->prosumerId = INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
        return ERR_GENERAL_ERROR;
    }
}

SharingErrorCode MediaChannel::HandleDestroyConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    if (consumer_ == nullptr) {
        SHARING_LOGW("consumer is null, mediachannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_OK;
        statusMsg->prosumerId = INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PROSUMER_DESTROY);
        return SharingErrorCode::ERR_OK;
    }

    if (consumer_->IsRunning()) {
        SHARING_LOGD("consumer is running, mediachannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_PROSUMER_DESTROY;
        statusMsg->prosumerId = consumer_ ? consumer_->GetId() : INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
        return SharingErrorCode::ERR_GENERAL_ERROR;
    } else {
        statusMsg->status = PROSUMER_DESTROY;
        consumer_->UpdateOperation(statusMsg);
        SHARING_LOGD("ERR_OK.");
        return SharingErrorCode::ERR_OK;
    }
}

SharingErrorCode MediaChannel::HandleStartConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    if (consumer_) {
        statusMsg->status = PROSUMER_START;
        consumer_->UpdateOperation(statusMsg);
        SHARING_LOGD("update consumer start success, mediaChannelId: %{public}u.", GetId());
        return SharingErrorCode::ERR_OK;
    } else {
        SHARING_LOGE("consumer is null, mediachannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_PROSUMER_START;
        statusMsg->prosumerId = INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }
}

SharingErrorCode MediaChannel::HandleStopConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    if (nullptr != playController_) {
        playController_->Stop();
        SHARING_LOGD("play controller stop success, mediaChannelId: %{public}u.", GetId());
    } else {
        SHARING_LOGW("playerController is nullptr, mediaChannelId: %{public}u.", GetId());
    }

    if (consumer_) {
        statusMsg->status = PROSUMER_STOP;
        consumer_->UpdateOperation(statusMsg);
        SHARING_LOGD("update consumer stop success, mediaChannelId: %{public}u.", GetId());
    } else {
        SHARING_LOGW("consumer null mediaChannelId: %{public}u.", GetId());
        statusMsg->errorCode = ERR_OK;
        statusMsg->prosumerId = INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PROSUMER_STOP);
    }

    return SharingErrorCode::ERR_OK;
}

SharingErrorCode MediaChannel::HandlePauseConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->mediaType = channelMsg->mediaType;

    if (consumer_) {
        statusMsg->status = PROSUMER_PAUSE;
        consumer_->UpdateOperation(statusMsg);
        return SharingErrorCode::ERR_OK;
    }

    SHARING_LOGD("pasue consumer error mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandleResumeConsumer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->mediaType = channelMsg->mediaType;

    if (consumer_) {
        statusMsg->status = PROSUMER_RESUME;
        consumer_->UpdateOperation(statusMsg);
        return SharingErrorCode::ERR_OK;
    }

    SHARING_LOGD("resume consumer error mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandleCreateProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    uint32_t retId = CreateProducer(channelMsg->className);
    if (retId != INVALID_ID) {
        SHARING_LOGD("create prosumer success.");
        producers_[retId]->SetSrcAgentId(channelMsg->agentId);
        statusMsg->status = PROSUMER_INIT;
        producers_[retId]->UpdateOperation(statusMsg);
        return SharingErrorCode::ERR_OK;
    } else {
        SHARING_LOGW("create prosumer failed.");
        statusMsg->errorCode = ERR_PROSUMER_CREATE;
        statusMsg->prosumerId = INVALID_ID;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }
}

SharingErrorCode MediaChannel::HandleDestroyProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    uint32_t id = channelMsg->prosumerId;
    BaseProducer::Ptr pProducer = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = producers_.find(id);
        if (iter != producers_.end()) {
            pProducer = iter->second;
        } else {
            SHARING_LOGE("cann't find producerId: %{public}d, channelId: %{public}d.", id, GetId());
        }
    }

    if (nullptr == pProducer) {
        statusMsg->errorCode = ERR_OK;
        statusMsg->prosumerId = id;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PROSUMER_DESTROY);
        return SharingErrorCode::ERR_OK;
    }

    if (pProducer->IsRunning()) {
        SHARING_LOGD("producer is running, producerId: %{public}d, mediachannelId: %{public}u.", id, GetId());
        statusMsg->errorCode = ERR_PROSUMER_DESTROY;
        statusMsg->prosumerId = id;
        statusMsg->agentId = channelMsg->agentId;
        SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
    } else {
        statusMsg->status = PROSUMER_DESTROY;
        pProducer->UpdateOperation(statusMsg);
    }

    SHARING_LOGD("producer destroy exit, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_OK;
}

SharingErrorCode MediaChannel::HandleStartProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = producers_.find(channelMsg->prosumerId);
        if (iter != producers_.end() && (iter->second)) {
            auto producer = iter->second;
            dispatcher_->AttachReceiver(producer);
            statusMsg->status = PROSUMER_START;
            producer->UpdateOperation(statusMsg);
            producer->StartDispatchThread();
            SHARING_LOGD("media start producer success, mediachannelId: %{public}u.", GetId());
            return SharingErrorCode::ERR_OK;
        } else {
            SHARING_LOGW("cann't find producerId: %{public}d, mediachannelId: %{public}d, agentId: %{public}d.",
                         channelMsg->prosumerId, GetId(), channelMsg->agentId);
            statusMsg->errorCode = ERR_PROSUMER_START;
            statusMsg->prosumerId = channelMsg->prosumerId;
            statusMsg->agentId = channelMsg->agentId;
            SendAgentEvent(statusMsg, EVENT_AGENT_PROSUMER_ERROR);
        }
    }

    SHARING_LOGD("media start producer end, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandleStopProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = producers_.find(channelMsg->prosumerId);
        if (iter != producers_.end() && (iter->second)) {
            dispatcher_->DetachReceiver(iter->second);
            statusMsg->status = PROSUMER_STOP;
            iter->second->UpdateOperation(statusMsg);
            SHARING_LOGD("media stop producer success, mediachannelId: %{public}u.", GetId());
            return SharingErrorCode::ERR_OK;
        } else {
            SHARING_LOGW("cann't find producerId: %{public}d, mediachannelId: %{public}d, agentId: %{public}d.",
                         channelMsg->prosumerId, GetId(), channelMsg->agentId);
            statusMsg->errorCode = ERR_OK;
            statusMsg->prosumerId = channelMsg->prosumerId;
            statusMsg->agentId = channelMsg->agentId;
            SendAgentEvent(statusMsg, EVENT_AGENT_STATE_PROSUMER_STOP);
        }
    }

    SHARING_LOGD("media stop producer end, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandlePauseProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->mediaType = channelMsg->mediaType;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = producers_.find(channelMsg->prosumerId);
        if (iter != producers_.end() && (iter->second)) {
            statusMsg->status = PROSUMER_PAUSE;
            iter->second->UpdateOperation(statusMsg);
            return SharingErrorCode::ERR_OK;
        } else {
            SHARING_LOGW("cann't find producerId: %{public}d, mediachannelId: %{public}d, agentId: %{public}d.",
                         channelMsg->prosumerId, GetId(), channelMsg->agentId);
        }
    }

    SHARING_LOGD("media pause producer end, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

SharingErrorCode MediaChannel::HandleResumeProducer(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto channelMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (channelMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = channelMsg;
    statusMsg->mediaType = channelMsg->mediaType;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = producers_.find(channelMsg->prosumerId);
        if (iter != producers_.end() && (iter->second)) {
            statusMsg->status = PROSUMER_RESUME;
            iter->second->UpdateOperation(statusMsg);
            return SharingErrorCode::ERR_OK;
        } else {
            SHARING_LOGW("cann't find producerId: %{public}d, mediachannelId: %{public}d, agentId: %{public}d.",
                         channelMsg->prosumerId, GetId(), channelMsg->agentId);
        }
    }

    SHARING_LOGD("media resume producer end, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_GENERAL_ERROR;
}

void MediaChannel::SendAgentEvent(ProsumerStatusMsg::Ptr &msg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    RETURN_IF_NULL(msg->eventMsg);
    SHARING_LOGD("send event: %{public}s ,mediachannelId: %{public}u, contextId: %{public}u, agentId: %{public}u.",
                 std::string(magic_enum::enum_name(eventType)).c_str(), GetId(), srcContextId_, msg->agentId);

    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->fromMgr = ModuleType::MODULE_MEDIACHANNEL;
    agentMsg->srcId = GetId();
    agentMsg->dstId = srcContextId_;
    agentMsg->type = eventType;
    agentMsg->requestId = msg->eventMsg->requestId;
    agentMsg->agentId = msg->agentId;
    agentMsg->prosumerId = msg->prosumerId;
    agentMsg->errorCode = msg->errorCode;
    agentMsg->surfaceId = msg->surfaceId;

    SharingEvent agentEvent;
    agentEvent.eventMsg = std::move(agentMsg);
    SendEvent(agentEvent);
}

SharingErrorCode MediaChannel::InitPlayController()
{
    SHARING_LOGD("trace.");
    if (playController_ != nullptr) {
        playController_->SetMediaChannel(shared_from_this());
        AudioTrack audioTrack;
        if (!(consumer_->IsCapture())) {
            audioTrack = consumer_->GetAudioTrack();
        }
        VideoTrack videoTrack = consumer_->GetVideoTrack();
        if (!playController_->Init(audioTrack, videoTrack)) {
            SHARING_LOGE("player controller Init error.");
            playController_ = nullptr;
            return SharingErrorCode::ERR_DECODE_FORMAT;
        }
    }

    SHARING_LOGD("create playcontroller end, mediachannelId: %{public}u.", GetId());
    return SharingErrorCode::ERR_OK;
}

} // namespace Sharing
} // namespace OHOS