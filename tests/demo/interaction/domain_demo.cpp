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

#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "extend/magic_enum/magic_enum.hpp"
#include "common/sharing_log.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "interaction/domain/domain_def.h"
#include "interaction/domain/rpc/domain_rpc_client.h"
#include "interaction/device_kit/dm_kit.h"
#include "interaction/ipc_codec/ipc_msg_encoder.h"
#include "interaction/ipc_codec/ipc_msg_decoder.h"

#include "access_token.h"
#include "hap_token_info.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"


using namespace OHOS::Sharing;


class PeerListener : public OHOS::Sharing::IDomainPeerListener {
public:
    PeerListener() = default;
    ~PeerListener() = default;
    void OnPeerConnected(std::string remoteId) override
    {
        SHARING_LOGI("OnPeerConnected: %{public}s", remoteId.c_str());
    }
    void OnPeerDisconnected(std::string remoteId) override
    {
        SHARING_LOGI("OnPeerDisconnected: %{public}s", remoteId.c_str());
    }
    void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> msg) override
    {
        SHARING_LOGI("OnDomainRequest: From %{public}s -> To %{public}s", msg->fromDevId.c_str(), msg->toDevId.c_str());
    }
};

void InitNativeToken()
{
    uint64_t tokenId;
    const char *perms[1];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "sharing_domain_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}




int main()
{



    InitNativeToken();
    SHARING_LOGI("DomainDemo start");
    SHARING_LOGI("Get DeviceInfos");
    OHOS::Sharing::DmKit::InitDeviceManager();

    auto localDeviceInfo = OHOS::Sharing::DmKit::GetLocalDevicesInfo();
    SHARING_LOGI(" localDeviceInfo: %{public}s", localDeviceInfo.deviceId);
    auto trustedDeviceInfos = OHOS::Sharing::DmKit::GetTrustedDevicesInfo();
    for(auto& deviceInfo : trustedDeviceInfos) {
        SHARING_LOGI("trustedDeviceInfos: %{public}s", deviceInfo.deviceId);
    }
    std::shared_ptr<PeerListener> peerListener = std::make_shared<PeerListener>();

    SHARING_LOGI("Create DomainRpcClient and Initialize");

    SHARING_LOGI("Enter any key to Initialize DomainRpcClient");
    getchar();

    std::shared_ptr<DomainRpcClient> domainClinet = std::make_shared<DomainRpcClient>();
    domainClinet->SetPeerListener(peerListener);

    domainClinet->GetDomainProxy(trustedDeviceInfos[0].deviceId);
    domainClinet->Initialize();


    SHARING_LOGI("Create DomainRpcClient end");


    SHARING_LOGI("Enter any key to send domain request");

    getchar();
    std::shared_ptr<ChatCallReq> msg = std::make_shared<ChatCallReq>();

    msg->fromDevId = localDeviceInfo.deviceId;
    msg->toDevId = trustedDeviceInfos[0].deviceId;
    msg->toRpcKey = "domain test url";

    OHOS::MessageParcel parcel;

    printf("inMsg: %d -- %s, %s\n",msg->GetMsgId(), msg->fromDevId.c_str(), msg->toRpcKey.c_str());
    // msg -> parcel
    auto inMsgBase = std::static_pointer_cast<BaseMsg>(msg);
    IpcMsgEncoder::GetInstance().MsgEncode(parcel, inMsgBase);

    // auto id = parcel.ReadInt32();
    // auto fromId =  parcel.ReadString();
    // auto toId =  parcel.ReadString();
    // auto fromIp =  parcel.ReadString();
    // auto toIp =  parcel.ReadString();
    // auto callerKey = parcel.ReadString();
    // auto mediaType = parcel.ReadUint32();
    // auto url = parcel.ReadString();

    // printf("parcel: id  %d -- %s, %s-- %d\n",id, fromId.c_str(), url.c_str(), mediaType);


    //parcel -> msg
    std::shared_ptr<BaseDomainMsg> outMsg = nullptr;

    IpcMsgDecoder::GetInstance().DomainMsgDecode(outMsg,  parcel);

    auto outMsgUpper = std::static_pointer_cast<ChatCallReq>(outMsg);
    printf("OutMsg: %d -- %s, %s\n",outMsgUpper->GetMsgId(), outMsgUpper->fromDevId.c_str(), outMsgUpper->toRpcKey.c_str());


    // domainClinet->SendDomainRequest(trustedDeviceInfos[0].deviceId, msg);

    usleep(2*1000*1000);

}