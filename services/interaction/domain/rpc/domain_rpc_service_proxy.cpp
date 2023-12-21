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
#include "domain_rpc_service_proxy.h"
#include "ipc_msg_encoder.h"
#include "ipc_msg_decoder.h"
namespace OHOS {
namespace Sharing {
    
DomainRpcServiceProxy::DomainRpcServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IDomainRpcService>(impl)
{
    SHARING_LOGD("trace.");
}

int32_t DomainRpcServiceProxy::SetListenerObject(const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    (void)data.WriteRemoteObject(object);

    int error = Remote()->SendRequest(DomainServiceMsg::SET_LISTENER_OBJ, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("set listener object failed: %{public}d.", error);
        return error;
    }

    return reply.ReadInt32();
}

sptr<IRemoteObject> DomainRpcServiceProxy::GetSubSystemAbility(int32_t type)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInt32(type);

    int error = Remote()->SendRequest(DomainServiceMsg::GET_SUBSYSTEM, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("create sub proxy failed, error: %{public}d.", error);
        return nullptr;
    }

    return reply.ReadRemoteObject();
}


int32_t DomainRpcServiceProxy::DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg)
{
    SHARING_LOGD("trace.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC, MessageOption::TF_WAIT_TIME};
    IpcMsgEncoder::GetInstance().DomainMsgEncode(data, msg);

    int error = Remote()->SendRequest(DomainServiceMsg::DOMAIN_MSG, data, reply, option);
    if (error != ERR_NONE) {
        SHARING_LOGE("do ipc command failed %{public}d.", error);
        return error;
    }

    return reply.ReadInt32();
}

}
}

