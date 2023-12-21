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

#ifndef OHOS_SHARING_WFD_MSG_H
#define OHOS_SHARING_WFD_MSG_H

#include "common/const_def.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "iremote_object.h"

namespace OHOS {
namespace Sharing {

enum WfdMsgId {
    WFD_COMMON_RSP = SharingMsgId::WFD_MSG_ID_START,
    WFD_SINK_START_REQ,
    WFD_SINK_STOP_REQ,
    WFD_SET_SURFACE_REQ,
    WFD_ADD_SURFACE_REQ,
    WFD_DEL_SURFACE_REQ,
    WFD_SET_MEDIA_FORMAT_REQ,
    WFD_PLAY_REQ,
    WFD_PAUSE_REQ,
    WFD_CLOSE_REQ,
    WFD_SET_SCENE_TYPE_REQ,
    WFD_MUTE_REQ,
    WFD_UNMUTE_REQ,
    WFD_GET_SINK_CONFIG_REQ,
    WFD_GET_SINK_CONFIG_RSP,
    WFD_CONNECTION_CHANGED_MSG,
    WFD_DECODER_ACCELERATION_DONE,
    WFD_ERROR_MSG,
    WFD_INFO_MSG,
    WFD_SURFACE_FAILURE,
    WFD_SOURCE_START_REQ,
    WFD_SOURCE_STOP_REQ,
    WFD_SOURCE_START_DISCOVERY_REQ,
    WFD_SOURCE_STOP_DISCOVERY_REQ,
    WFD_SOURCE_DEVICE_FOUND_MSG,
    WFD_SOURCE_ADD_DEVICE_REQ,
    WFD_SOURCE_REMOVE_DEVICE_REQ,
    WFD_SOURCE_CONNECT_DEVICE_REQ,
    WFD_SOURCE_CREATE_SCREEN_CAPTURE_REQ,
    WFD_SOURCE_CREATE_SCREEN_CAPTURE_RSP,
    WFD_SOURCE_DESTROY_SCREEN_CAPTUREREQ_REQ,
    WFD_SOURCE_APPEND_CAST_REQ,
    WFD_SOURCE_APPEND_CAST_RSP,
    WFD_SOURCE_REMOVE_CAST_REQ,
    // domain msg
};

struct WfdCommonRsp : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_COMMON_RSP };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdCommonRsp &operator=(const WfdCommonRsp &rReq)
    {
        if (this != &rReq) {
            this->ret = rReq.ret;
        }

        return *this;
    }

    IPC_BIND_ATTR(ret)

    int32_t ret = 0;
};

struct WfdSinkStartReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SINK_START_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdSinkStopReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SINK_STOP_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdAppendSurfaceReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SET_SURFACE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdAppendSurfaceReq &operator=(const WfdAppendSurfaceReq &rReq)
    {
        if (this != &rReq) {
            this->sceneType = rReq.sceneType;
            this->deviceId = rReq.deviceId;
            this->surface = rReq.surface;
        }

        return *this;
    }

    IPC_BIND_ATTR(sceneType, deviceId, surface)

    uint32_t sceneType = 1;

    std::string deviceId;
    sptr<IRemoteObject> surface;
};

struct WfdRemoveSurfaceReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_DEL_SURFACE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdRemoveSurfaceReq &operator=(const WfdRemoveSurfaceReq &rReq)
    {
        if (this != &rReq) {
            this->surfaceId = rReq.surfaceId;
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(surfaceId, deviceId)

    uint64_t surfaceId;
    std::string deviceId;
};

struct CodecAttr {
    IPC_BIND_ATTR(formatId, codecType)

    int32_t formatId = -1;
    int32_t codecType = -1;
};

struct SetMediaFormatReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SET_MEDIA_FORMAT_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    SetMediaFormatReq &operator=(const SetMediaFormatReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
            this->videoAttr = rReq.videoAttr;
            this->audioAttr = rReq.audioAttr;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId, videoAttr, audioAttr)

    std::string deviceId;
    CodecAttr videoAttr;
    CodecAttr audioAttr;
};

struct WfdPlayReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_PLAY_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdPlayReq &operator=(const WfdPlayReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct WfdPauseReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_PAUSE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdPauseReq &operator=(const WfdPauseReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct WfdCloseReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_CLOSE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdCloseReq &operator=(const WfdCloseReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct SetSceneTypeReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SET_SCENE_TYPE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    SetSceneTypeReq &operator=(const SetSceneTypeReq &rReq)
    {
        if (this != &rReq) {
            this->sceneType = rReq.sceneType;
            this->surfaceId = rReq.surfaceId;
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(sceneType, surfaceId, deviceId)

    uint32_t sceneType;
    uint64_t surfaceId;

    std::string deviceId;
};

struct MuteReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_MUTE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    MuteReq &operator=(const MuteReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct UnMuteReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_UNMUTE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    UnMuteReq &operator=(const UnMuteReq &rReq)
    {
        if (this != &rReq) {
            this->deviceId = rReq.deviceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct GetSinkConfigReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_GET_SINK_CONFIG_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct GetSinkConfigRsp : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_GET_SINK_CONFIG_RSP };

    int32_t GetMsgId() final { return MSG_ID; }

    GetSinkConfigRsp &operator=(const GetSinkConfigRsp &rRsp)
    {
        if (this != &rRsp) {
            this->surfaceMaximum = rRsp.surfaceMaximum;
            this->accessDevMaximum = rRsp.accessDevMaximum;
            this->foregroundMaximum = rRsp.foregroundMaximum;
        }

        return *this;
    }

    IPC_BIND_ATTR(surfaceMaximum, accessDevMaximum, foregroundMaximum)

    uint32_t surfaceMaximum;
    uint32_t accessDevMaximum;
    uint32_t foregroundMaximum;
};

struct WfdSourceStartReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_START_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdSourceStopReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_STOP_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdSourceStartDiscoveryReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_START_DISCOVERY_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdSourceStopDiscoveryReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_STOP_DISCOVERY_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR0
};

struct WfdCastDeviceInfo {
    IPC_BIND_ATTR(deviceId, ipAddr, deviceName, primaryDeviceType, secondaryDeviceType)

    std::string deviceId;
    std::string ipAddr;
    std::string deviceName;
    std::string primaryDeviceType;
    std::string secondaryDeviceType;
};

struct WfdSourceDeviceFoundMsg : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_DEVICE_FOUND_MSG };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceInfos)

    std::vector<WfdCastDeviceInfo> deviceInfos;
};

struct WfdSourceAddDeviceReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_ADD_DEVICE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(screenId, deviceId)

    uint64_t screenId;
    std::string deviceId;
};

struct WfdSourceRemoveDeviceReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_REMOVE_DEVICE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct ConnectDeviceReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_CONNECT_DEVICE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct CreateScreenCaptureReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_CREATE_SCREEN_CAPTURE_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId, mediaType)

    std::string deviceId;
    uint32_t mediaType;
};

struct CreateScreenCaptureRsp : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_CREATE_SCREEN_CAPTURE_RSP };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(screenId, agentId, contextId)

    uint64_t screenId;
    uint32_t agentId;
    uint32_t contextId;
};

struct DestroyScreenCaptureReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_DESTROY_SCREEN_CAPTUREREQ_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct AppendCastRsp : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_APPEND_CAST_RSP };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct RemoveCastReq : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SOURCE_REMOVE_CAST_REQ };

    int32_t GetMsgId() final { return MSG_ID; }

    IPC_BIND_ATTR(deviceId)

    std::string deviceId;
};

struct WfdErrorMsg : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_ERROR_MSG };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdErrorMsg &operator=(const WfdErrorMsg &rReq)
    {
        if (this != &rReq) {
            this->errorCode = rReq.errorCode;
            this->agentId = rReq.agentId;
            this->contextId = rReq.contextId;
            this->message = rReq.message;
            this->mac = rReq.mac;
        }

        return *this;
    }

    IPC_BIND_ATTR(errorCode, agentId, contextId, message, mac)

    int32_t errorCode;

    uint32_t agentId;
    uint32_t contextId;


    std::string message;
    std::string mac;
};

struct WfdConnectionChangedMsg : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_CONNECTION_CHANGED_MSG };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdConnectionChangedMsg &operator=(const WfdConnectionChangedMsg &rReq)
    {
        if (this != &rReq) {
            this->state = rReq.state;
            this->surfaceId = rReq.surfaceId;
            this->ip = rReq.ip;
            this->mac = rReq.mac;
            this->deviceName = rReq.deviceName;
            this->primaryDeviceType = rReq.primaryDeviceType;
            this->secondaryDeviceType = rReq.secondaryDeviceType;
        }

        return *this;
    }

    IPC_BIND_ATTR(state, surfaceId, ip, mac, deviceName, primaryDeviceType, secondaryDeviceType)

    int32_t state;
    uint64_t surfaceId;

    std::string ip;
    std::string mac;
    std::string deviceName;
    std::string primaryDeviceType;
    std::string secondaryDeviceType;
};

struct WfdDecoderAccelerationDoneMsg : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_DECODER_ACCELERATION_DONE };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdDecoderAccelerationDoneMsg &operator=(const WfdDecoderAccelerationDoneMsg &rReq)
    {
        if (this != &rReq) {
            this->surfaceId = rReq.surfaceId;
        }

        return *this;
    }

    IPC_BIND_ATTR(surfaceId)

    uint64_t surfaceId;
};

struct WfdSurfaceFailureMsg : public BaseMsg {
    enum { MSG_ID = WfdMsgId::WFD_SURFACE_FAILURE };

    int32_t GetMsgId() final { return MSG_ID; }

    WfdSurfaceFailureMsg &operator=(const WfdSurfaceFailureMsg &rReq)
    {
        if (this != &rReq) {
            this->surfaceId = rReq.surfaceId;
            this->mac = rReq.mac;
        }

        return *this;
    }

    IPC_BIND_ATTR(surfaceId, mac)

    uint64_t surfaceId;
    std::string mac;
};

} // namespace Sharing
} // namespace OHOS
#endif
