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

#ifndef OHOS_SHARING_WFD_SINK_H
#define OHOS_SHARING_WFD_SINK_H

#include "impl/scene/wfd/wfd_def.h"
#include "wfd.h"

namespace OHOS {
namespace Sharing {

class WfdSink {
public:
    virtual ~WfdSink() = default;

    virtual int32_t Stop() = 0;
    virtual int32_t Start() = 0;
    virtual int32_t Play(std::string deviceId) = 0;
    virtual int32_t Pause(std::string deviceId) = 0;
    virtual int32_t Close(std::string deviceId) = 0;

    virtual int32_t Mute(std::string deviceId) = 0;
    virtual int32_t UnMute(std::string deviceId) = 0;

    virtual int32_t GetSinkConfig(SinkConfig &sinkConfig) = 0;

    virtual int32_t AppendSurface(std::string deviceId, uint64_t surfaceId) = 0;
    virtual int32_t RemoveSurface(std::string deviceId, uint64_t surfaceId) = 0;

    virtual int32_t SetListener(const std::shared_ptr<IWfdEventListener> &listener) = 0;

    virtual int32_t SetSceneType(std::string deviceId, uint64_t surfaceId, SceneType sceneType) = 0;
    virtual int32_t SetMediaFormat(std::string deviceId, CodecAttr videoAttr, CodecAttr audioAttr) = 0;
};

class __attribute__((visibility("default"))) WfdSinkFactory {
public:
    static std::shared_ptr<WfdSink> CreateSink(int32_t type, const std::string key);

private:
    WfdSinkFactory() = default;
    ~WfdSinkFactory() = default;
    static std::shared_ptr<WfdSink> wfdSinkImpl_;
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_WFD_H
