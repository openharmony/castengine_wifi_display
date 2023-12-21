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
#include "wfd.h"
namespace OHOS {
namespace Sharing {
class WfdSinkDemoListener;
class WfdSinkDemo : public std::enable_shared_from_this<WfdSinkDemo> {
public:
    WfdSinkDemo();
    ~WfdSinkDemo() = default;

    std::shared_ptr<WfdSink> GetClient();
    bool CreateClient();
    bool SetListener();
    bool SetDiscoverable(bool enable);
    bool GetConfig();
    bool Start();
    bool Stop();
    bool AppendSurface(std::string devId);
    bool RemoveSurface(std::string devId, std::string surfaceId);
    bool SetMediaFormat(std::string devId, VideoFormat videoFormatId, AudioFormat audioFormatId);
    bool SetSceneType(std::string devId, std::string surfaceId, SceneType sceneType);
    bool Mute(std::string devId);
    int32_t UnMute(std::string devId);
    void AddDevice(const std::string devId);
    void RemoveDevice(const std::string devId);
    void ListDevices();
    bool Play(std::string devId);
    bool Pause(std::string devId);
    bool Close(std::string devId);
    void DoCmd(std::string cmd);

private:
    std::shared_ptr<WfdSink> client_ = nullptr;
    std::shared_ptr<WfdSinkDemoListener> listener_ = nullptr;
    uint32_t wdnum_ = 0;
    std::unordered_map<uint64_t, bool> surfaceUsing_{};
    std::vector<sptr<Surface>> surfaces_{};
    std::unordered_map<uint64_t, std::string> surfaceDevMap_{};
    std::vector<std::string> devices_{};
};

class WfdSinkDemoListener : public IWfdSinkListener {
public:
    void OnError(uint32_t regionId, uint32_t agentId, SharingErrorCode errorCode);
    void OnInfo(std::shared_ptr<BaseMsg> &msg) override;
    void OnConnectionChanged(const ConnectionInfo &info);
    void SetListener(std::shared_ptr<WfdSinkDemo> listener)
    {
        listener_ = listener;
    }

private:
    std::weak_ptr<WfdSinkDemo> listener_;
};
} // namespace Sharing
} // namespace OHOS
