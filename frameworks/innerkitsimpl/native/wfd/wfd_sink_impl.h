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

#ifndef OHOS_SHARING_WFD_SINK_IMPL_H
#define OHOS_SHARING_WFD_SINK_IMPL_H

#include <memory>
#include "impl/scene/wfd/wfd_msg.h"
#include "interaction/interprocess/inter_ipc_client.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "nocopyable.h"
#include "wfd_sink.h"

namespace OHOS {
namespace Sharing {

class IWfdSinkService;

class WfdSinkImpl : public WfdSink,
                    public MsgAdapterListener,
                    public std::enable_shared_from_this<WfdSinkImpl>,
                    public NoCopyable {
public:
    WfdSinkImpl();
    ~WfdSinkImpl();

    void SetIpcAdapter(const std::weak_ptr<IpcMsgAdapter> &adapter)
    {
        SHARING_LOGD("trace.");
        ipcAdapter_ = adapter;
        auto ipcAdapter = ipcAdapter_.lock();
        if (ipcAdapter) {
            ipcAdapter->SetListener(shared_from_this());
        }
    }

    void SetIpcClient(const std::shared_ptr<InterIpcClient> &client)
    {
        SHARING_LOGD("trace.");
        client_ = client;
    }

public:
    void OnRemoteDied() override;
    void OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) override;

    int32_t Stop() override;
    int32_t Start() override;
    int32_t Play(std::string deviceId) override;
    int32_t Pause(std::string deviceId) override;
    int32_t Close(std::string deviceId) override;

    int32_t Mute(std::string deviceId) override;
    int32_t UnMute(std::string deviceId) override;

    int32_t GetSinkConfig(SinkConfig &config) override;
    int32_t AppendSurface(std::string deviceId, uint64_t surfaceId) override;
    int32_t RemoveSurface(std::string deviceId, uint64_t surfaceId) override;

    int32_t SetListener(const std::shared_ptr<IWfdEventListener> &callback) override;

    int32_t SetSceneType(std::string deviceId, uint64_t surfaceId, SceneType sceneType) override;
    int32_t SetMediaFormat(std::string deviceId, CodecAttr videoAttr, CodecAttr audioAttr) override;

private:
    std::weak_ptr<IpcMsgAdapter> ipcAdapter_;
    std::weak_ptr<IWfdEventListener> listener_;
    std::shared_ptr<InterIpcClient> client_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
