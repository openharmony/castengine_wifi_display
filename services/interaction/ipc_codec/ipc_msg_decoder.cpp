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

#include "ipc_msg_decoder.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {

int32_t IpcMsgDecoder::MsgDecode(std::shared_ptr<BaseMsg> &sharingMsg, MessageParcel &ipcMsg)
{
    SHARING_LOGD("trace.");
    int32_t nMsgType = ipcMsg.ReadInt32();
    return this->OnIpcMessage(nMsgType, sharingMsg, ipcMsg);
}

int32_t IpcMsgDecoder::DomainMsgDecode(std::shared_ptr<BaseDomainMsg> &sharingMsg, MessageParcel &ipcMsg)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<BaseMsg> reply = nullptr;
    int32_t ret = MsgDecode(reply, ipcMsg);
    if (ret != 0) {
        SHARING_LOGE("decode domain msg failed.");
        return ret;
    }

    sharingMsg = std::static_pointer_cast<BaseDomainMsg>(reply);
    return 0;
}

} // namespace Sharing
} // namespace OHOS