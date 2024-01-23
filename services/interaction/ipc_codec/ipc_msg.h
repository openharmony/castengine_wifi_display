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

#ifndef OHOS__IPC_MEDDAGE_MSG_H
#define OHOS__IPC_MEDDAGE_MSG_H

#include "ipc_codec_bind_macros.h"
#include "ipc_msg_bind_macros.h"

namespace OHOS {
namespace Sharing {

struct BaseMsg {
    virtual int32_t GetMsgId()
    {
        return 0;
    }

    BaseMsg() = default;
    virtual ~BaseMsg() = default;
};

struct BaseDomainMsg : public BaseMsg {
    int32_t GetMsgId() override
    {
        return 1;
    }

    bool pullUpApp = false;
    std::string fromDevId;
    std::string toDevId;
    std::string fromRpcKey;
    std::string toRpcKey;
};

} // namespace Sharing
} // namespace OHOS

#endif