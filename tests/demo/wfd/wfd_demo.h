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

#ifndef WFD_DEMO_H
#define WFD_DEMO_H

#include <cstdint>
#include <mutex>
#include <string>
#include "wifi_diaplay.h"

namespace OHOS {
namespace Sharing {

class WfdDemo : public IWfdListener,
                public std::enable_shared_from_this<WfdDemo> {
public:
    WfdDemo() = default;
    ~WfdDemo() = default;

    bool Init(const WfdMode mode);

    void AddDevice();
    void InitWindow();
    void ListDevices();
    void RemoveDevice();
    void SelectMediaFormat();

    void OnError(const CastErrorInfo &errorInfo) override;
    void OnDeviceState(const CastDeviceInfo &deviceInfo) override;
    void OnDeviceFound(const std::vector<CastDeviceInfo> &deviceInfos) override;

    void RunWfdSink();
    void RunWfdSource();
    void DoCmd(std::string cmd);

private:
    bool surfaceIsUsing_ = false;

    int32_t windowNum_ = 1;

    std::mutex mutex_;
    std::vector<uint64_t> surfaceIds_;
    std::vector<std::string> devicesIsPlaying_;
    std::shared_ptr<WifiDisplay> client_ = nullptr;
    std::map<std::string, std::function<int32_t(std::string)>> WifiDisplayTable_;

    CastCodecAttr videoAttr_;
    CastCodecAttr audioAttr_;
};

} // namespace Sharing
} // namespace OHOS

#endif