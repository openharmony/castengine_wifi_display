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

#ifndef OHOS_SHARING_DOMAIN_RPC_SERVICE_H
#define OHOS_SHARING_DOMAIN_RPC_SERVICE_H

#include <unordered_map>
#include "inter_ipc_service_stub.h"
#include "system_ability.h"
#include "common/kv_operator.h"

namespace OHOS {
namespace Sharing {
    
class InterIpcService final : public SystemAbility,
                               public InterIpcServiceStub,
                               public std::enable_shared_from_this<InterIpcService> {
    DECLARE_SYSTEM_ABILITY(InterIpcService);

public:
    explicit InterIpcService(int32_t systemAbilityId, bool runOnCreate = true);
    ~InterIpcService() override;

protected:
    void OnDump() final;
    void OnStop() final;
    void OnStart() final;

private:
    void RecordProcessIdByKvStore();

private:
    KvOperator kvOperator_;
};

} // namespace Sharing
} // namespace OHOS
#endif