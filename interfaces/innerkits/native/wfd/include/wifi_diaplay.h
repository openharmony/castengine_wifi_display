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

#ifndef OHOS_SHARING_WIFIDISPLAY_H
#define OHOS_SHARING_WIFIDISPLAY_H

#include <cstdint>
#include <memory>
#include <string>
#include "impl/scene/wfd/wfd_def.h"
#include "wfd_sink_impl.h"

namespace OHOS {
namespace Sharing {

enum WfdMode : int32_t {
    SINK,
    SOURCE,
};

enum class CodecType : int32_t {
    CODEC_NONE = -1,
    CODEC_H264 = 1,
    CODEC_H265,
    CODEC_AAC,
};

enum class CastDeviceState : int32_t {
    CONNECTED,
    DISCONNECTED,
    INIT,
    READY,
    PLAYING,
    PAUSED,
    STOPED,
};

struct CastCodecAttr {
    int32_t format;
    CodecType codecType;
};

struct CastDeviceInfo {
    std::string deviceId;
    std::string ipAddr;
    std::string deviceName;
    std::string primaryDeviceType;
    std::string secondaryDeviceType;
    CastDeviceState state;
};

struct CastErrorInfo {
    std::string deviceId;
    int32_t errorCode;
};

class IWfdListener {
public:
    virtual ~IWfdListener() = default;

    virtual void OnError(const CastErrorInfo &errorInfo) = 0;
    virtual void OnDeviceState(const CastDeviceInfo &deviceInfo) = 0;
    virtual void OnDeviceFound(const std::vector<CastDeviceInfo> &deviceInfos) = 0;
};

class WifiDisplay : public IWfdEventListener,
                    public std::enable_shared_from_this<WifiDisplay> {
public:
    ~WifiDisplay();
    WifiDisplay(WfdMode mode);

    void Init();
    int32_t Stop();
    int32_t Start();
    int32_t Play(std::string devId);
    int32_t Pause(std::string devId);
    int32_t Close(std::string devId);
    int32_t AppendSurface(std::string devId, uint64_t surfaceId);
    int32_t SetMediaFormat(std::string deviceId, CastCodecAttr videoAttr, CastCodecAttr audioAttr);

    int32_t StopDiscover();
    int32_t StartDiscover();
    int32_t RemoveDevice(std::string deviceId);
    int32_t AddDevice(const CastDeviceInfo &deviceInfo);

    int32_t SetListener(const std::shared_ptr<IWfdListener> &listener);

    void OnInfo(std::shared_ptr<BaseMsg> &info) override;

private:
    bool IsVideoCodecAttr(CastCodecAttr videoAttr);
    bool IsAudioCodecAttr(CastCodecAttr audioAttr);

private:
    WfdMode mode_ = WfdMode::SINK;
    std::weak_ptr<IWfdListener> listener_;
    std::shared_ptr<WfdSink> wfdSinkImpl_ = nullptr;
};

class __attribute__((visibility("default"))) MiracastFactory {
public:
    static std::shared_ptr<WifiDisplay> GetInstance(WfdMode mode);

private:
    MiracastFactory() = default;
    ~MiracastFactory() = default;
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_WFD_H