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

#ifndef OHOS_INTER_IPC_MEDDAGE_DECODER_H
#define OHOS_INTER_IPC_MEDDAGE_DECODER_H

#include <singleton.h>
#include "impl/scene/wfd/wfd_msg.h"
#include "ipc_msg.h"
#include "ipc_msg_bind_macros.h"

namespace OHOS {
namespace Sharing {

class IpcMsgDecoder : public Singleton<IpcMsgDecoder> {
    friend class Singleton<IpcMsgDecoder>;

public:
    virtual ~IpcMsgDecoder() = default;

    int32_t MsgDecode(std::shared_ptr<BaseMsg> &sharingMsg, MessageParcel &ipcMsg);
    int32_t DomainMsgDecode(std::shared_ptr<BaseDomainMsg> &sharingMsg, MessageParcel &ipcMsg);

public:
    BEGIN_IPC_MESSAGE_DECODE_BIND

#include "impl/scene/wfd/wfd_msg_decoder.inc"

    END_IPC_MESSAGE_DECODE_BIND
};

} // namespace Sharing
} // namespace OHOS

#endif
