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

#include <string>
#include "common/const_def.h"
#include "wfd_sink.h"
#include "wifi_diaplay.h"

namespace OHOS {
namespace Sharing {
class WfdSinkDemoListener;
class WfdSinkDemo : public std::enable_shared_from_this<WfdSinkDemo> {
public:
    WfdSinkDemo();
    ~WfdSinkDemo() = default;

    void ListDevices();
    void DoCmd(std::string cmd);
    void AddDevice(const std::string deviceId);
    void RemoveDevice(const std::string deviceId);

    bool Stop();
    bool Start();
    bool GetConfig();
    bool SetListener();
    bool CreateClient();
    bool Mute(std::string deviceId);
    bool Play(std::string deviceId);
    bool Pause(std::string deviceId);
    bool Close(std::string deviceId);
    bool SetDiscoverable(bool enable);
    bool AppendSurface(std::string deviceId);
    bool RemoveSurface(std::string deviceId, std::string surfaceId);
    bool SetSceneType(std::string deviceId, std::string surfaceId, SceneType sceneType);
    bool SetMediaFormat(std::string deviceId, VideoFormat videoFormatId, AudioFormat audioFormatId);

    int32_t UnMute(std::string deviceId);

    std::shared_ptr<WfdSink> GetClient();

private:
    uint32_t wdnum_ = 0;

    std::shared_ptr<WfdSink> client_ = nullptr;
    std::shared_ptr<WfdSinkDemoListener> listener_ = nullptr;

    std::vector<std::string> devices_{};
    std::vector<sptr<Surface>> surfaces_{};

    std::unordered_map<uint64_t, bool> surfaceUsing_{};
    std::unordered_map<uint64_t, std::string> surfaceDevMap_{};
};

class WfdSinkDemoListener : public IWfdEventListener {
public:
    void OnInfo(std::shared_ptr<BaseMsg> &msg) override;
    void OnConnectionChanged(const ConnectionInfo &info);
    void OnError(uint32_t regionId, uint32_t agentId, SharingErrorCode errorCode);

    void SetListener(std::shared_ptr<WfdSinkDemo> listener)
    {
        listener_ = listener;
    }

private:
    std::weak_ptr<WfdSinkDemo> listener_;
};
} // namespace Sharing
} // namespace OHOS
