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

#include "interaction_manager.h"
#include <future>
#include <unistd.h>
#include "ability_manager_client.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"
#include "event/event_manager.h"
#include "interaction/device_kit/dm_kit.h"
#include "interaction/domain/domain_manager.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "magic_enum.hpp"
#include "utils.h"

namespace OHOS {
namespace Sharing {

InteractionManager::InteractionManager()
{
    SHARING_LOGD("trace.");
    EventManager::GetInstance().AddListener(std::make_shared<InteractionEventListener>());
}

InteractionManager::~InteractionManager()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    interactions_.clear();
    sharedFromThis_.reset();
}

void InteractionManager::Init()
{
    SHARING_LOGD("trace.");
    if (sharedFromThis_ == nullptr) {
        sharedFromThis_ = Ptr(this, [](InteractionManager *) { SHARING_LOGD("trace."); });
    }
    DomainManager::GetInstance()->SetListener(sharedFromThis_);
}

int32_t InteractionManager::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("fromMgr: %{public}u, srcId: %{public}u, toMgr: %{public}u, dstId: %{public}u, event: %{public}s.",
                 event.eventMsg->fromMgr, event.eventMsg->srcId, event.eventMsg->toMgr, event.eventMsg->dstId,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    switch (event.eventMsg->type) {
        case EVENT_CONFIGURE_INTERACT:
            break;
        case EVENT_INTERACTIONMGR_DESTROY_INTERACTION: {
            auto eventMsg = std::static_pointer_cast<InteractionEventMsg>(event.eventMsg);
            if (eventMsg) {
                DestroyInteraction(eventMsg->dstId);
            } else {
                SHARING_LOGE("event msg null.");
            }
            break;
        }
        case EVENT_INTERACTIONMGR_REMOVE_INTERACTION: {
            auto eventMsg = std::static_pointer_cast<InteractionEventMsg>(event.eventMsg);
            if (eventMsg) {
                RemoveInteraction(eventMsg->dstId);
            } else {
                SHARING_LOGE("event msg null.");
            }
            break;
        }
        default: {
            auto eventMsg = std::static_pointer_cast<InteractionEventMsg>(event.eventMsg);
            if (eventMsg) {
                Interaction::Ptr interaction = GetInteraction(eventMsg->dstId);
                if (interaction) {
                    interaction->HandleEvent(event);
                } else {
                    SHARING_LOGE("interaction null interactionId: %{public}d.", eventMsg->dstId);
                }
            } else {
                SHARING_LOGE("event msg null.");
            }
            break;
        }
    }

    return 0;
}

Interaction::Ptr InteractionManager::CreateInteraction(const std::string &className)
{
    SHARING_LOGD("trace.");
    Interaction::Ptr interaction = std::make_shared<Interaction>();
    if (interaction->CreateScene(className)) {
        std::lock_guard<std::mutex> lock(mutex_);
        interactions_.emplace(interaction->GetId(), interaction);
        SHARING_LOGI("id: %{public}d size: %{public}zu.", interaction->GetId(), interactions_.size());
        return interaction;
    } else {
        SHARING_LOGE("create scene failed.");
        return nullptr;
    }
}

void InteractionManager::DestroyInteraction(uint32_t interactionId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = interactions_.find(interactionId);
    if (itr != interactions_.end() && itr->second != nullptr) {
        itr->second->Destroy();
    }
}

void InteractionManager::RemoveInteraction(uint32_t interactionId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = interactions_.find(interactionId);
    if (itr != interactions_.end()) {
        SHARING_LOGI("remove interaction key: %{public}s, id: %{public}u.", itr->second->GetRpcKey().c_str(),
                     interactionId);
        interactionKeys_.erase(itr->second->GetRpcKey());
        interactions_.erase(itr);
    }
}

Interaction::Ptr InteractionManager::GetInteraction(uint32_t interactionId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = interactions_.find(interactionId);
    if (itr != interactions_.end()) {
        return itr->second;
    }

    return nullptr;
}

int32_t InteractionManager::OnDomainMsg(std::shared_ptr<BaseDomainMsg> msg)
{
    SHARING_LOGD("Finding target key: %{public}s, msgId: %{public}d, pullup app: %{public}d.", msg->toRpcKey.c_str(),
                 msg->GetMsgId(), msg->pullUpApp);
    if (interactionKeys_.find(msg->toRpcKey) != interactionKeys_.end()) {
        uint32_t interactionId = GetInteractionId(msg->toRpcKey);
        Interaction::Ptr interaction = GetInteraction(interactionId);
        if (interaction) {
            interaction->OnDomainMsg(msg);
        } else {
            SHARING_LOGE("interaction null interactionId: %{public}d.", interactionId);
        }
    } else {
        SHARING_LOGW("interactionKeys_ not find fromDevId: %{public}s.", msg->fromDevId.c_str());
        if (!msg->pullUpApp) {
            SHARING_LOGW("msgId: %{public}d, don't need to pull up the app.", msg->GetMsgId());
            return -1;
        }

        RpcKeyParser parser;
        parser.ParseKey(msg->toRpcKey);
        std::string bundleName = parser.GetBundleName();
        std::string abilityName = parser.GetAbilityName();
        AAFwk::Want want;
        AppExecFwk::ElementName element("", bundleName.c_str(), abilityName.c_str());
        want.SetElement(element);
        auto ret = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
        if (ret) {
            SHARING_LOGE("start app failed, bundleName: %{public}s, abilityName: %{public}s.", bundleName.c_str(),
                         abilityName.c_str());
            return -1;
        }

        uint32_t interactionId = 0;
        uint32_t tryCount = 10;
        while (tryCount--) {
            interactionId = GetInteractionId(msg->toRpcKey);
            if (interactionId != 0) {
                break;
            }
            usleep(300000); // 300000:us
        }

        Interaction::Ptr interaction = GetInteraction(interactionId);
        if (interaction) {
            interaction->OnDomainMsg(msg);
        } else {
            SHARING_LOGE("find interaction failed afer app started, key: %{public}s.", msg->toRpcKey.c_str());
            return -1;
        }
        return 0;
    }

    return 0;
}

int32_t InteractionManager::SendDomainMsg(std::shared_ptr<BaseDomainMsg> msg)
{
    SHARING_LOGD("trace.");

    std::async(std::launch::async, [this, msg] { DomainManager::GetInstance()->SendDomainRequest(msg->toDevId, msg); });

    return 0;
}

int32_t InteractionEventListener::OnEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    return InteractionManager::GetInstance().HandleEvent(event);
}

void InteractionManager::AddInteractionKey(const std::string &key, uint32_t interactionId)
{
    SHARING_LOGD("trace.");
    DelInteractionKey(key);
    std::lock_guard<std::mutex> lock(mutex_);
    interactionKeys_.emplace(key, interactionId);
}

void InteractionManager::DelInteractionKey(const std::string &key)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = interactionKeys_.find(key);
    if (itr != interactionKeys_.end()) {
        interactionKeys_.erase(itr);
    }
}

int32_t InteractionManager::GetInteractionId(const std::string &key)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = interactionKeys_.find(key);
    if (itr != interactionKeys_.end()) {
        return itr->second;
    }

    return 0;
}

Interaction::Ptr InteractionManager::GetInteraction(const std::string &key)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (interactionKeys_.find(key) != interactionKeys_.end()) {
        auto itr = interactions_.find(interactionKeys_[key]);
        if (itr != interactions_.end()) {
            return itr->second;
        }
    }

    return nullptr;
}

} // namespace Sharing
} // namespace OHOS