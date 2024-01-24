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

#include "ipc_msg_encoder.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {

int32_t IpcMsgEncoder::MsgEncode(MessageParcel &ipcMsg, std::shared_ptr<BaseMsg> &sharingMsg)
{
    SHARING_LOGD("trace.");
    int32_t nMsgType = sharingMsg->GetMsgId();
    ipcMsg.WriteInt32(nMsgType);

    return this->OnIpcMessage(nMsgType, ipcMsg, sharingMsg);
}

int32_t IpcMsgEncoder::DomainMsgEncode(MessageParcel &ipcMsg, std::shared_ptr<BaseDomainMsg> &sharingMsg)
{
    SHARING_LOGD("trace.");
    auto msg = std::static_pointer_cast<BaseMsg>(sharingMsg);
    int32_t ret = MsgEncode(ipcMsg, msg);
    if (ret != 0) {
        SHARING_LOGE("MsgEncode domain msg failed.");
        return ret;
    }

    return 0;
}

} // namespace Sharing
} // namespace OHOS