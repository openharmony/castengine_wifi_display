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
 
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/sharing_log.h"
#include "wfd_sink_impl.h"
#include "wifi_diaplay.h"

namespace OHOS {
namespace Sharing {

std::shared_ptr<WifiDisplay> MiracastFactory::GetInstance(WfdMode mode)
{
    SHARING_LOGD("trace.");
    auto wifiDisplay = std::make_shared<WifiDisplay>(mode);
    wifiDisplay->Init();
    return wifiDisplay;
}

WifiDisplay::WifiDisplay(WfdMode mode)
{
    SHARING_LOGD("trace.");
    if (mode == SINK) {
        SHARING_LOGD("trace.");
        wfdSinkImpl_ = WfdSinkFactory::CreateSink(0, "WFD_SINK");
    } else {
        SHARING_LOGD("create wfd source.");
    }
}

WifiDisplay::~WifiDisplay()
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_) {
        wfdSinkImpl_->Stop();
        wfdSinkImpl_ = nullptr;
    }
}

void WifiDisplay::Init()
{
    if (wfdSinkImpl_)
        wfdSinkImpl_->SetListener(shared_from_this());
}

int32_t WifiDisplay::Stop()
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->Stop();
        if (ret != 0) {
            SHARING_LOGE("stop wifi display failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::Start()
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->Start();
        if (ret != 0) {
            SHARING_LOGE("start wifi display failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::AppendSurface(std::string devId, uint64_t surfaceId)
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->AppendSurface(devId, surfaceId);
        if (ret != 0) {
            SHARING_LOGE("failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::SetMediaFormat(std::string deviceId, CastCodecAttr videoAttr, CastCodecAttr audioAttr)
{
    SHARING_LOGD("trace.");
    if (IsVideoCodecAttr(videoAttr) && IsAudioCodecAttr(audioAttr) && mode_ == SINK) {
        CodecAttr videoCodecAttr;
        videoCodecAttr.formatId = videoAttr.format;
        videoCodecAttr.codecType = (int32_t)videoAttr.codecType;

        CodecAttr audioCodecAttr;
        audioCodecAttr.formatId = audioAttr.format;
        audioCodecAttr.codecType = (int32_t)audioAttr.codecType;

        if (wfdSinkImpl_) {
            return wfdSinkImpl_->SetMediaFormat(deviceId, videoCodecAttr, audioCodecAttr);
        }
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::Play(std::string devId)
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->Play(devId);
        if (ret != 0) {
            SHARING_LOGE("start protocol interaction failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::Pause(std::string devId)
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->Pause(devId);
        if (ret != 0) {
            SHARING_LOGE("pause the playback failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::Close(std::string devId)
{
    SHARING_LOGD("trace.");
    if (wfdSinkImpl_ && mode_ == SINK) {
        auto ret = wfdSinkImpl_->Close(devId);
        if (ret != 0) {
            SHARING_LOGE("close the source failed");
        }

        return ret;
    }

    return ERR_GENERAL_ERROR;
}

int32_t WifiDisplay::StartDiscover()
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t WifiDisplay::StopDiscover()
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t WifiDisplay::AddDevice(const CastDeviceInfo &deviceInfo)
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t WifiDisplay::RemoveDevice(std::string deviceId)
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t WifiDisplay::SetListener(const std::shared_ptr<IWfdListener> &listener)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(listener);

    listener_ = listener;
    return ERR_OK;
}

void WifiDisplay::OnInfo(std::shared_ptr<BaseMsg> &info)
{
    SHARING_LOGD("trace.");
    CHECK_AND_RETURN_LOG(info != nullptr, "info is nullptr.");

    SHARING_LOGI("msg: %{public}d.", info->GetMsgId());
    switch (info->GetMsgId()) {
        case WfdErrorMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdErrorMsg>(info);
            CastErrorInfo errInfo;
            errInfo.deviceId = data->mac;
            errInfo.errorCode = data->errorCode;

            auto listener = listener_.lock();
            if (listener) {
                listener->OnError(errInfo);
            }

            break;
        }
        case WfdConnectionChangedMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdConnectionChangedMsg>(info);
            CastDeviceInfo deviceInfo;
            deviceInfo.ipAddr = data->ip;
            deviceInfo.deviceId = data->mac;
            deviceInfo.state = static_cast<CastDeviceState>(data->state);
            deviceInfo.deviceName = data->deviceName;
            deviceInfo.primaryDeviceType = data->primaryDeviceType;
            deviceInfo.secondaryDeviceType = data->secondaryDeviceType;

            auto callback = listener_.lock();
            if (callback) {
                callback->OnDeviceState(deviceInfo);
            }

            break;
        }
        case WfdSurfaceFailureMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSurfaceFailureMsg>(info);
            CastErrorInfo errInfo;
            errInfo.errorCode = ERR_SURFACE_FAILURE;

            auto listener = listener_.lock();
            if (listener) {
                listener->OnError(errInfo);
            }

            break;
        }
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

bool WifiDisplay::IsVideoCodecAttr(CastCodecAttr videoAttr)
{
    SHARING_LOGD("trace.");
    switch (videoAttr.codecType) {
        case CodecType::CODEC_H264:
            break;
        case CodecType::CODEC_H265:
            break;
        default:
            SHARING_LOGE("none process case.");
            return false;
    }

    switch (videoAttr.format) {
        case VideoFormat::VIDEO_640X480_60:
            break;
        case VideoFormat::VIDEO_1280X720_25:
            break;
        case VideoFormat::VIDEO_1280X720_30:
            break;
        case VideoFormat::VIDEO_1920X1080_25:
            break;
        case VideoFormat::VIDEO_1920X1080_30:
            break;
        default:
            SHARING_LOGE("none process case.");
            return false;
    }

    return true;
}

bool WifiDisplay::IsAudioCodecAttr(CastCodecAttr audioAttr)
{
    SHARING_LOGD("trace.");
    switch (audioAttr.codecType) {
        case CodecType::CODEC_AAC:
            break;
        default:
            SHARING_LOGI("none process case.");
            return false;
    }

    switch (audioAttr.format) {
        case AudioFormat::AUDIO_44100_8_1:
            break;
        case AudioFormat::AUDIO_44100_8_2:
            break;
        case AudioFormat::AUDIO_44100_16_1:
            break;
        case AudioFormat::AUDIO_44100_16_2:
            break;
        case AudioFormat::AUDIO_48000_8_1:
            break;
        case AudioFormat::AUDIO_48000_8_2:
            break;
        case AudioFormat::AUDIO_48000_16_1:
            break;
        case AudioFormat::AUDIO_48000_16_2:
            break;
        default:
            SHARING_LOGI("none process case.");
            return false;
    }

    return true;
}

} // namespace Sharing
} // namespace OHOS