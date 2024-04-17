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

#ifndef OHOS_SHARING_WFD_SOURCE_H
#define OHOS_SHARING_WFD_SOURCE_H

#include "impl/scene/wfd/wfd_def.h"
#include "wfd.h"

namespace OHOS {
namespace Sharing {

class WfdSource {
public:
    virtual ~WfdSource() = default;
    virtual int32_t StopDiscover() = 0;
    virtual int32_t StartDiscover() = 0;
    virtual int32_t RemoveDevice(std::string deviceId) = 0;
    virtual int32_t AddDevice(uint64_t screenId, WfdCastDeviceInfo &deviceInfo) = 0;
    virtual int32_t SetListener(const std::shared_ptr<IWfdEventListener> &listener) = 0;
};

class __attribute__((visibility("default"))) WfdSourceFactory {
public:
    static std::shared_ptr<WfdSource> CreateSource(int32_t type, const std::string key);

private:
    WfdSourceFactory() = default;
    ~WfdSourceFactory() = default;
    static std::shared_ptr<WfdSource> wfdSourceImpl_;
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_WFD_H
