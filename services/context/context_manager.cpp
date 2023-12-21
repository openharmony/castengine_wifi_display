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

#include "context_manager.h"
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/event_comm.h"
#include "common/sharing_log.h"
#include "configuration/include/config.h"
#include "magic_enum.hpp"
#include "network/network_session_manager.h"

namespace OHOS {
namespace Sharing {

ContextManager::~ContextManager()
{
    SHARING_LOGD("id: %{public}s.", std::string(magic_enum::enum_name(ModuleType::MODULE_CONTEXT)).c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    contexts_.clear();
}

void ContextManager::Init()
{
    SHARING_LOGD("trace.");
    SharingValue::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("context", "agentLimit", "maxSinkAgent", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t value;
        values->GetValue<int32_t>(value);
        maxSinkAgent_ = static_cast<uint32_t>(value);
    } else {
        maxSinkAgent_ = MAX_SINK_AGENT_NUM;
    }

    ret = Config::GetInstance().GetConfig("context", "agentLimit", "maxSrcAgent", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t value;
        values->GetValue<int32_t>(value);
        maxSrcAgent_ = static_cast<uint32_t>(value);
    } else {
        maxSrcAgent_ = MAX_SRC_AGENT_NUM;
    }

    ret = Config::GetInstance().GetConfig("context", "agentLimit", "maxContext", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t value;
        values->GetValue<int32_t>(value);
        maxContext_ = static_cast<uint32_t>(value);
    } else {
        maxContext_ = MAX_CONTEXT_NUM;
    }

    int32_t logOn;
    ret = Config::GetInstance().GetConfig("network", "networkLimit", "logOn", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(logOn);
    } else {
        logOn = 0;
    }

    NetworkSessionManager::GetInstance().SetLogFlag(static_cast<int8_t>(logOn));
}

int32_t ContextManager::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("fromMgr: %{public}u, srcId: %{public}u, toMgr: %{public}u, dstId: %{public}u, event: %{public}s.",
                 event.eventMsg->fromMgr, event.eventMsg->srcId, event.eventMsg->toMgr, event.eventMsg->dstId,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    switch (event.eventMsg->type) {
        case EventType::EVENT_CONFIGURE_CONTEXT:
            HandleConfiguration(event);
            break;
        case EventType::EVENT_CONTEXTMGR_CREATE:
            event.eventMsg->dstId = HandleContextCreate();
            break;
        case EventType::EVENT_CONTEXTMGR_AGENT_CREATE:
            HandleAgentCreate(event);
            break;
        case EventType::EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY:
            HandleMediachannelDestroy(event);
            break;
        default:
            HandleContextEvent(event);
            break;
    }

    return 0;
}

void ContextManager::HandleConfiguration(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto eventMsg = ConvertEventMsg<ConfigEventMsg>(event);
    if (eventMsg && eventMsg->data) {
        auto data = eventMsg->data;
        if (data->HasTag("agentLimit")) {
            auto valueTest = data->GetSharingValue("agentLimit", "testdescription");
            if (valueTest != nullptr && valueTest->IsString()) {
                std::string valueData;
                valueTest->GetValue<std::string>(valueData);
                SHARING_LOGD("sinkLimit: %{public}s.", valueData.c_str());
            }
        } else if (data->HasTag("networkLimit")) {
            auto logFlag = data->GetSharingValue("networkLimit", "logOn");
            if (logFlag != nullptr && logFlag->IsInt32()) {
                int32_t valueData = -1;
                if (logFlag->GetValue<int32_t>(valueData)) {
                    SHARING_LOGD("logFlag: %{public}d.", valueData);
                    NetworkSessionManager::GetInstance().SetLogFlag(valueData);
                }
            }
        }
    } else {
        SHARING_LOGE("msg null.");
    }
}

void ContextManager::HandleAgentCreate(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto contextEventMsg = ConvertEventMsg<ContextEventMsg>(event);
    RETURN_IF_NULL(contextEventMsg);
    if (!CheckAgentSize((AgentType)contextEventMsg->agentType)) {
        return;
    }

    if (event.eventMsg->dstId == INVALID_ID || GetContextById(event.eventMsg->dstId) == nullptr) {
        event.eventMsg->dstId = HandleContextCreate();
    }

    if (event.eventMsg->dstId == INVALID_ID) {
        SHARING_LOGE("create context error! invalid id.");
        return;
    }

    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    HandleContextEvent(event);
}

bool ContextManager::CheckAgentSize(AgentType agentType)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (agentType == SINK_AGENT) {
        int32_t sinkCount = 0;
        for (auto &item : contexts_) {
            sinkCount += (item.second->GetSinkAgentSize());
        }
        SHARING_LOGI("now sink agent num: %{public}d.", sinkCount);
        if (sinkCount >= GetAgentSinkLimit()) {
            SHARING_LOGE("check agent size error! limit sink agent size.");
            return false;
        }
    } else {
        int32_t srcCount = 0;
        for (auto &item : contexts_) {
            srcCount += (item.second->GetSrcAgentSize());
        }
        SHARING_LOGI("now src agent num: %{public}d.", srcCount);
        if (srcCount >= GetAgentSrcLimit()) {
            SHARING_LOGE("check agent size error! limit src agent size.");
            return false;
        }
    }

    return true;
}

uint32_t ContextManager::HandleContextCreate()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (contexts_.size() >= maxContext_) {
        SHARING_LOGE("create context error! limit context size.");
        return INVALID_ID;
    }

    auto context = std::make_shared<Context>();
    contexts_.emplace(context->GetId(), context);
    SHARING_LOGI("contextId: %{public}d contextSize: %{public}zu.", context->GetId(), contexts_.size());
    return context->GetId();
}

void ContextManager::HandleMediachannelDestroy(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);
    auto context = GetContextById(event.eventMsg->dstId);
    if (context) {
        event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
        context->HandleEvent(event);
        if (context->IsEmptyAgent()) {
            DestroyContext(event.eventMsg->dstId);
        } else {
            SHARING_LOGD("need wait other agent destroy contextId: %{public}u.", context->GetId());
        }
    } else {
        SHARING_LOGW("exception to enter! dstId: %{public}u.", event.eventMsg->dstId);
    }
}

void ContextManager::HandleContextEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);
    auto context = GetContextById(event.eventMsg->dstId);
    if (context) {
        context->HandleEvent(event);
    } else {
        SHARING_LOGE("context not exist, contextId: %{public}u eventType: %{public}s.", event.eventMsg->dstId,
                     std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    }
}

Context::Ptr ContextManager::GetContextById(uint32_t contextId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = contexts_.find(contextId);
    if (itr != contexts_.end()) {
        return itr->second;
    }

    return nullptr;
}

void ContextManager::DestroyContext(uint32_t contextId)
{
    SHARING_LOGD("trace.");
    bool paFlag = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto itr = contexts_.find(contextId);
        if (itr != contexts_.end()) {
            if (itr->second != nullptr) {
                itr->second->Release();
            }
            contexts_.erase(itr);
        }
        if (0 == contexts_.size()) {
            paFlag = true;
        }
    }

    if (paFlag) {
        system("pactl set-default-source Built_in_mic");
        system("pactl set-default-sink Speaker");
    }

    SHARING_LOGI("contextId: %{public}d contextSize: %{public}zu.", contextId, contexts_.size());
}

} // namespace Sharing
} // namespace OHOS