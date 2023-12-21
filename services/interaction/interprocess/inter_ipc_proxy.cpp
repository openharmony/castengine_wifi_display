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

#include "common/sharing_log.h"
#include "inter_ipc_proxy.h"
#include "ipc_msg_encoder.h"
#include "ipc_msg_decoder.h"
namespace OHOS {
namespace Sharing {

InterIpcProxy::InterIpcProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IInterIpc>(impl)
{
    SHARING_LOGD("trace.");
}

int32_t InterIpcProxy::SetListenerObject(std::string key, const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    (void)data.WriteString(key);
    (void)data.WriteRemoteObject(object);

    int error = Remote()->SendRequest(InterIpcMsg::SET_LISTENER_OBJ, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("set listener object failed: %{public}d.", error);
        return error;
    }

    return reply.ReadInt32();
}


sptr<IRemoteObject> InterIpcProxy::GetSubSystemAbility(std::string key, std::string className)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteString(key);
    data.WriteString(className);

    int error = Remote()->SendRequest(InterIpcMsg::GET_SUBSYSTEM, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("create sub proxy failed, error: %{public}d.", error);
        return nullptr;
    }

    return reply.ReadRemoteObject();
}


int32_t InterIpcProxy::DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    IpcMsgEncoder::GetInstance().MsgEncode(data, msg);

    int error = Remote()->SendRequest(InterIpcMsg::INTER_IPC_MSG, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("do ipc command failed %{public}d.", error);
        return error;
    }

    IpcMsgDecoder::GetInstance().MsgDecode(replyMsg, reply);
    return reply.ReadInt32();
}

}
}

