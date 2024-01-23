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

#include "inter_ipc_service.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "common/sharing_log.h"
#include "configuration/include/config.h"
#include "context/context_manager.h"
#include "event/event_manager.h"
#include "hap_token_info.h"
#include "inter_ipc_service_death_listener.h"
#include "interaction/interaction.h"
#include "interaction/interaction_manager.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "interaction/scene/base_scene.h"
#include "ipc_skeleton.h"
#include "mediachannel/channel_manager.h"
#include "nativetoken_kit.h"
#include "scheduler/scheduler.h"
#include "token_setproc.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sharing {

REGISTER_SYSTEM_ABILITY_BY_ID(InterIpcService, SHARING_SERVICE_TEMP_SA_ID, true)

InterIpcService::InterIpcService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
    SHARING_LOGD("trace.");
}

InterIpcService::~InterIpcService()
{
    SHARING_LOGD("trace.");
}

void InterIpcService::OnDump()
{
    SHARING_LOGD("trace.");
}

void InterIpcService::OnStart()
{
    SHARING_LOGD("trace.");
    if (Publish(this)) {
        SHARING_LOGD("success.");
        EventManager::GetInstance().Init();
        EventManager::GetInstance().StartEventLoop();
        Scheduler::GetInstance();
        Config::GetInstance().Init();
        InteractionManager::GetInstance().Init();
        ContextManager::GetInstance().Init();
        ChannelManager::GetInstance().Init();
    } else {
        SHARING_LOGD("failed.");
    }
}

void InterIpcService::OnStop()
{
    SHARING_LOGD("trace.");
}

void InterIpcService::RecordProcessIdByKvStore()
{
    SHARING_LOGD("trace.");
    int32_t pid = getpid();
    SHARING_LOGD("open KvStore.");
    kvOperator_.OpenKvStore();
    
    nlohmann::json jsObj;
    jsObj[VALID_PID] = std::to_string(pid);
    jsObj[ENABLE_HARDWARE] = true;

    SharingValue::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("codec", "forceSWDecoder", "isEnable", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        bool value;
        values->GetValue<bool>(value);
        jsObj[ENABLE_HARDWARE] = static_cast<bool>(!value);
    }

    kvOperator_.PutValues(SHARING_INFOS, jsObj.dump());
    SHARING_LOGD("kv put value, pid: %{public}d.", pid);
    
    kvOperator_.CloseKvStore();
}

} // namespace Sharing
} // namespace OHOS