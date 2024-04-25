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

#ifndef OHOS_SHARING_INTERACTION_MANAGER_H
#define OHOS_SHARING_INTERACTION_MANAGER_H

#include <unordered_map>
#include "domain/domain_manager.h"
#include "event/event_base.h"
#include "event/handle_event_base.h"
#include "interaction.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "singleton.h"

namespace OHOS {
namespace Sharing {

class InteractionEventListener : public EventListener {
public:
    InteractionEventListener()
    {
        SHARING_LOGD("trace.");
        classType_ = CLASS_TYPE_INTERACTION;
    }

    virtual ~InteractionEventListener() = default;

    int32_t OnEvent(SharingEvent &event) final;
};

class InteractionManager final : public Singleton<InteractionManager>,
                                 public HandleEventBase,
                                 public DomainManagerListener,
                                 public EventEmitter {
    friend class Singleton<InteractionManager>;

public:
    using Ptr = std::shared_ptr<InteractionManager>;
    InteractionManager();
    ~InteractionManager() override;

    void Init();
    void RemoveInteraction(uint32_t interactionId);
    void DestroyInteraction(uint32_t interactionId);
    int32_t HandleEvent(SharingEvent &event) final;
    Interaction::Ptr CreateInteraction(const std::string &className);

    void DelInteractionKey(const std::string &key);
    void AddInteractionKey(const std::string &key, uint32_t interactionId);
    int32_t GetInteractionId(const std::string &key);

    int32_t SendDomainMsg(std::shared_ptr<BaseDomainMsg> msg);
    int32_t OnDomainMsg(std::shared_ptr<BaseDomainMsg> msg) override;

    Interaction::Ptr GetInteraction(const std::string &key);
    Interaction::Ptr GetInteraction(uint32_t interactionId);

private:
    std::mutex mutex_;
    Ptr sharedFromThis_ = nullptr;
    std::unordered_map<std::string, uint32_t> interactionKeys_;
    std::unordered_map<uint32_t, Interaction::Ptr> interactions_;
};

} // namespace Sharing
} // namespace OHOS
#endif