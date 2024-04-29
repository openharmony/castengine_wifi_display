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

#include "wfd_sink_impl.h"
#include <string>
#include "common/common_macro.h"
#include "interaction/interprocess/client_factory.h"
#include "surface_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {

std::shared_ptr<WfdSink> WfdSinkFactory::wfdSinkImpl_ = nullptr;

std::shared_ptr<WfdSink> OHOS::Sharing::WfdSinkFactory::CreateSink(int32_t type, const std::string key)
{
    SHARING_LOGD("trace.");

    if (!wfdSinkImpl_) {
        auto client = std::static_pointer_cast<InterIpcClient>(
            ClientFactory::GetInstance().CreateClient(key, "WfdSinkImpl", "WfdSinkScene"));
        if (client == nullptr) {
            SHARING_LOGE("failed to get client.");
            return nullptr;
        }

        auto adapter = client->GetMsgAdapter();
        if (adapter == nullptr) {
            SHARING_LOGE("failed to get adapter.");
        }

        std::shared_ptr<WfdSinkImpl> impl = std::make_shared<WfdSinkImpl>();
        if (impl == nullptr) {
            SHARING_LOGE("failed to new WfdSinkImpl.");
            return nullptr;
        }

        impl->SetIpcClient(client);
        impl->SetIpcAdapter(adapter);
        wfdSinkImpl_ = impl;
    }

    if (wfdSinkImpl_ == nullptr) {
        SHARING_LOGE("failed to Get wfdSinkImpl.");
        return nullptr;
    }

    return wfdSinkImpl_;
}

WfdSinkImpl::WfdSinkImpl()
{
    SHARING_LOGI("ctor.");
}

WfdSinkImpl::~WfdSinkImpl()
{
    SHARING_LOGI("dtor.");
}

int32_t WfdSinkImpl::GetSinkConfig(SinkConfig &sinkConfig)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<GetSinkConfigReq>();
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<GetSinkConfigRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<GetSinkConfigRsp>(reply);
    if (replyMsg->foregroundMaximum == INVALID_ID) {
        SHARING_LOGE("ipc do get sink config request failed.");
        return -1;
    }

    sinkConfig.accessDevMaximum = replyMsg->accessDevMaximum;
    sinkConfig.foregroundMaximum = replyMsg->foregroundMaximum;
    sinkConfig.surfaceMaximum = replyMsg->surfaceMaximum;
    SHARING_LOGD("access device size:%{public}u,  foreground size:%{public}u, surface size:%{public}u.",
                 sinkConfig.accessDevMaximum, sinkConfig.foregroundMaximum, sinkConfig.surfaceMaximum);

    return ret;
}

int32_t WfdSinkImpl::AppendSurface(std::string deviceId, uint64_t surfaceId)
{
    SHARING_LOGD("add device: %{public}s, surfaceId: %{public}" PRIx64 ".", GetAnonyString(deviceId).c_str(),
                 surfaceId);
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (nullptr == surface) {
        SHARING_LOGE("get surface failed.");
        return -1;
    }

    sptr<IBufferProducer> producer = surface->GetProducer();
    if (nullptr == producer) {
        SHARING_LOGE("producer object is null.");
        return -1;
    }

    sptr<IRemoteObject> object = producer->AsObject();
    if (nullptr == object) {
        SHARING_LOGE("surface object is null.");
        return -1;
    }

    auto msg = std::make_shared<WfdAppendSurfaceReq>();
    msg->surface = object;
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::RemoveSurface(std::string deviceId, uint64_t surfaceId)
{
    SHARING_LOGD("delete device: %{public}s, surfaceId:%{public}" PRIx64 ".", GetAnonyString(deviceId).c_str(),
                 surfaceId);
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdRemoveSurfaceReq>();
    msg->surfaceId = surfaceId;
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Start()
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Stop()
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSinkStopReq>();
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::SetMediaFormat(std::string deviceId, CodecAttr videoAttr, CodecAttr audioAttr)
{
    SHARING_LOGD("set device: %{public}s, VideoFormat: %{public}d, AudioFormat: %{public}d.", deviceId.c_str(),
                 videoAttr.formatId, audioAttr.formatId);
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<SetMediaFormatReq>();
    msg->deviceId = deviceId;
    msg->videoAttr = videoAttr;
    msg->audioAttr = audioAttr;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Play(std::string deviceId)
{
    SHARING_LOGD("device: %{public}s.", deviceId.c_str());
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdPlayReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Pause(std::string deviceId)
{
    SHARING_LOGD("device:%{public}s.", deviceId.c_str());
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdPauseReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Close(std::string deviceId)
{
    SHARING_LOGD("device: %{public}s.", deviceId.c_str());
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdCloseReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::SetSceneType(std::string deviceId, uint64_t surfaceId, SceneType sceneType)
{
    SHARING_LOGD("set device: %{public}s, surfaceId: %{public}" PRIx64 ", sceneType: %{public}d.",
                 GetAnonyString(deviceId).c_str(), surfaceId, sceneType);
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<SetSceneTypeReq>();
    msg->deviceId = deviceId;
    msg->surfaceId = surfaceId;
    msg->sceneType = sceneType;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::Mute(std::string deviceId)
{
    SHARING_LOGD("device: %{public}s.", GetAnonyString(deviceId).c_str());
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<MuteReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::UnMute(std::string deviceId)
{
    SHARING_LOGD("device: %{public}s.", GetAnonyString(deviceId).c_str());
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_INVALID_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<UnMuteReq>();
    msg->deviceId = deviceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    auto ret = ipcAdapter->SendRequest(msg, reply);
    if (ret != 0) {
        SHARING_LOGE("ipc sendRequest failed: %{public}d.", ret);
        return ret;
    }

    auto replyMsg = std::static_pointer_cast<WfdCommonRsp>(reply);
    return replyMsg->ret;
}

int32_t WfdSinkImpl::SetListener(const std::shared_ptr<IWfdEventListener> &callback)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(callback);

    listener_ = callback;
    return 0;
}

void WfdSinkImpl::OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);

    SHARING_LOGD("Recv msg: %{public}d.", msg->GetMsgId());
    auto listener = listener_.lock();
    if (listener) {
        listener->OnInfo(msg);
    } else {
        SHARING_LOGW("no listener provided by napi.");
    }
}

void WfdSinkImpl::OnRemoteDied()
{
    SHARING_LOGD("trace.");
    auto msg = std::make_shared<WfdErrorMsg>();
    msg->errorCode = ERR_REMOTE_DIED;
    msg->message = "OnRemoteDied.";

    auto listener = listener_.lock();
    if (listener) {
        std::shared_ptr<BaseMsg> baseMsg = std::static_pointer_cast<BaseMsg>(msg);
        listener->OnInfo(baseMsg);
    } else {
        SHARING_LOGW("no listener provided by napi.");
    }
}

} // namespace Sharing
} // namespace OHOS
