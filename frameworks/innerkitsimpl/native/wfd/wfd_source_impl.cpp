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

#include "wfd_source_impl.h"
#include "common/common_macro.h"
#include "interaction/interprocess/client_factory.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {

std::shared_ptr<WfdSource> WfdSourceFactory::wfdSourceImpl_ = nullptr;

std::shared_ptr<WfdSource> OHOS::Sharing::WfdSourceFactory::CreateSource(int32_t type, const std::string key)
{
    SHARING_LOGD("trace.");

    if (!wfdSourceImpl_) {
        auto client = std::static_pointer_cast<InterIpcClient>(
            ClientFactory::GetInstance().CreateClient(key, "WfdSourceImpl", "WfdSourceScene"));
        if (client == nullptr) {
            SHARING_LOGE("failed to get client.");
            return nullptr;
        }

        auto adapter = client->GetMsgAdapter();
        if (adapter == nullptr) {
            SHARING_LOGE("failed to get adapter.");
        }

        std::shared_ptr<WfdSourceImpl> impl = std::make_shared<WfdSourceImpl>();
        if (impl == nullptr) {
            SHARING_LOGE("failed to new WfdSourceImpl.");
            return nullptr;
        }

        impl->SetIpcClient(client);
        impl->SetIpcAdapter(adapter);
        wfdSourceImpl_ = impl;
    }

    if (wfdSourceImpl_ == nullptr) {
        SHARING_LOGE("failed to Get wfdSourceImpl.");
    }

    return wfdSourceImpl_;
}

WfdSourceImpl::WfdSourceImpl()
{
    SHARING_LOGD("trace.");
}

WfdSourceImpl::~WfdSourceImpl()
{
    SHARING_LOGD("trace.");
}

int32_t WfdSourceImpl::StartDiscover()
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);
    auto msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSourceImpl::StopDiscover()
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msgStopDiscoveryReq = std::make_shared<WfdSourceStopDiscoveryReq>();
    auto replyStopDiscoveryReq = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto retStopDiscoveryReq = ipcAdapter->SendRequest(msgStopDiscoveryReq, replyStopDiscoveryReq);
    if (retStopDiscoveryReq != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", retStopDiscoveryReq);
        return retStopDiscoveryReq;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(replyStopDiscoveryReq);
    return replyMsg->ret;
}

int32_t WfdSourceImpl::AddDevice(uint64_t screenId, WfdCastDeviceInfo &deviceInfo)
{
    SHARING_LOGD("Add deviceId: %{public}s, screenId: %{public}" PRIx64 ".",
                 GetAnonyString(deviceInfo.deviceId).c_str(), screenId);
    std::lock_guard<std::mutex> lock(mutex_);
    if (deviceAdded_) {
        return 0;
    }

    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSourceAddDeviceReq>();
    msg->deviceId = deviceInfo.deviceId;
    msg->screenId = screenId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
    } else {
        deviceAdded_ = true;
    }
    return ret;
}

int32_t WfdSourceImpl::RemoveDevice(std::string deviceId)
{
    SHARING_LOGD("device: %{public}s.", GetAnonyString(deviceId).c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!deviceAdded_) {
        return 0;
    }
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    } else {
        deviceAdded_ = false;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSourceImpl::SetListener(const std::shared_ptr<IWfdEventListener> &callback)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(callback);

    listener_ = callback;
    return 0;
}

void WfdSourceImpl::OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);

    SHARING_LOGD("Recv msg: %{public}d.", msg->GetMsgId());
    if (auto listener = listener_.lock()) {
        listener->OnInfo(msg);
    } else {
        SHARING_LOGW("no listener provided by napi.");
    }
}

void WfdSourceImpl::OnRemoteDied()
{
    SHARING_LOGD("trace.");
    auto msg = std::make_shared<WfdErrorMsg>();
    msg->errorCode = ERR_REMOTE_DIED;
    msg->message = "OnRemoteDied.";

    if (auto listener = listener_.lock()) {
        std::shared_ptr<BaseMsg> baseMsg = std::static_pointer_cast<BaseMsg>(msg);
        listener->OnInfo(baseMsg);
    } else {
        SHARING_LOGW("no listener provided by napi.");
    }
}

} // namespace Sharing
} // namespace OHOS
