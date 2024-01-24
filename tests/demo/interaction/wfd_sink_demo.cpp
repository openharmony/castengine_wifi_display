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

#include "wfd_sink_demo.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "common/sharing_log.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "impl/scene/wfd/wfd_def.h"
#include "interaction/interprocess/client_factory.h"
#include "surface_utils.h"
#include "utils/utils.h"
#include "windowmgr/include/window_manager.h"

using namespace OHOS::Sharing;

VideoFormat DEFAULT_VIDEO_FORMAT = VideoFormat::VIDEO_1920X1080_30;
AudioFormat DEFAULT_AUDIO_FORMAT = AudioFormat::AUDIO_48000_16_2;
std::vector<std::pair<int32_t, int32_t>> position{{0, 0}, {960, 0}, {0, 540}, {960, 540}};

constexpr uint32_t DEFAULT_WIDTH = 1280;
constexpr uint32_t DEFAULT_HEIGHT = 720;

WfdSinkDemo::WfdSinkDemo()
{
    listener_ = std::make_shared<WfdSinkDemoListener>();
    if (!listener_)
        printf("create WfdSinkDemoListener failed");
}

std::shared_ptr<WfdSink> WfdSinkDemo::GetClient()
{
    return client_;
}

bool WfdSinkDemo::CreateClient()
{
    client_ = WfdSinkFactory::CreateSink(0, "wfd_sink_demo");
    if (!client_) {
        printf("create wfdsink client error");
        return false;
    }
    if (listener_) {
        listener_->SetListener(shared_from_this());
        client_->SetListener(listener_);
        return true;
    } else {
        printf("Listener is nullptr");
        return false;
    }
}

bool WfdSinkDemo::SetDiscoverable(bool enable)
{
    if (!client_) {
        int ret = 0;
        if (enable) {
            ret = client_->Start();
        } else {
            ret = client_->Stop();
        }
        if (ret == 0) {
            printf("start or stop p2p success");
            return true;
        }
    }
    printf("start or stop failed");
    return false;
}

bool WfdSinkDemo::GetConfig()
{
    return true;
}

bool WfdSinkDemo::Start()
{
    printf("enter start");
    if (!client_) {
        printf("client is nullptr");
        return false;
    }
    if (client_->Start() == -1) {
        printf("sink start error");
        return false;
    }
    return true;
}

bool WfdSinkDemo::Stop()
{
    if (client_->Stop() == -1) {
        printf("sink stop error");
        return false;
    }
    return true;
}

bool WfdSinkDemo::AppendSurface(std::string deviceId)
{
    uint64_t surfaceId = 0;
    for (auto item : surfaceUsing_) {
        if (!item.second) {
            surfaceId = item.first;
            surfaceUsing_[surfaceId] = true;
        }
    }
    if (surfaceId == 0 && wdnum_ < 4) {
        // window1
        printf("create window enter");
        WindowProperty::Ptr windowPropertyPtr = std::make_shared<WindowProperty>();
        windowPropertyPtr->height = DEFAULT_HEIGHT;
        windowPropertyPtr->width = DEFAULT_WIDTH;
        windowPropertyPtr->startX = position[wdnum_].first;
        windowPropertyPtr->startY = position[wdnum_].second;
        windowPropertyPtr->isFull = false;
        windowPropertyPtr->isShow = true;
        ++wdnum_;
        int32_t windowId = WindowManager::GetInstance().CreateWindow(windowPropertyPtr);
        sptr<Surface> surface = WindowManager::GetInstance().GetSurface(windowId);
        surfaceId = surface->GetUniqueId();
        int ret = SurfaceUtils::GetInstance()->Add(surfaceId, surface);
        if (ret != 0)
            printf("add failed");
        surfaceUsing_.emplace(surfaceId, true);
        surfaces_.push_back(surface);
    }
    printf("surfaceId: %llu", surfaceId);
    if (client_->AppendSurface(deviceId, surfaceId) == -1) {
        printf("SetSurface error");
        return false;
    }
    surfaceDevMap_[surfaceId] = deviceId;
    return true;
}

bool WfdSinkDemo::RemoveSurface(std::string deviceId, std::string surfaceId)
{
    uint64_t surfaceUintId = std::stoull(surfaceId);
    if (client_->RemoveSurface(deviceId, surfaceUintId) != 0) {
        printf("delete surface:%llu failed", surfaceUintId);
        return false;
    }
    return true;
}

bool WfdSinkDemo::SetMediaFormat(std::string deviceId, VideoFormat videoFormatId, AudioFormat audioFormatId)
{
    CodecAttr videoAttr;
    videoAttr.codecType = CodecId::CODEC_H264;
    videoAttr.formatId = videoFormatId;
    CodecAttr audioAttr;
    audioAttr.codecType = CodecId::CODEC_AAC;
    audioAttr.formatId = audioFormatId;

    if (client_->SetMediaFormat(deviceId, videoAttr, audioAttr) == -1) {
        printf("SetVideoFormat error, surfaceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

bool WfdSinkDemo::SetSceneType(std::string deviceId, std::string surfaceId, SceneType sceneType)
{
    uint64_t stringUintId = std::stoull(surfaceId);
    if (client_->SetSceneType(deviceId, stringUintId, sceneType) == -1) {
        printf("SetSceneType error, surfaceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

bool WfdSinkDemo::Mute(std::string deviceId)
{
    if (client_->Mute(deviceId) == -1) {
        printf("Mute error, deviceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

int32_t WfdSinkDemo::UnMute(std::string deviceId)
{
    if (client_->UnMute(deviceId) == -1) {
        printf("UnMute error, deviceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

void WfdSinkDemo::AddDevice(const std::string deviceId)
{
    devices_.push_back(deviceId);
}

void WfdSinkDemo::RemoveDevice(const std::string deviceId)
{
    printf("remove device: %s", deviceId.c_str());
    for (uint32_t i = 0; i < devices_.size(); i++)
        if (devices_[i] == deviceId) {
            devices_.erase(devices_.begin() + i);
        }
    for (auto item : surfaceDevMap_) {
        if (item.second == deviceId) {
            printf("free surfacId: %llu", item.first);
            surfaceUsing_[item.first] = false;
        }
    }
}

void WfdSinkDemo::ListDevices()
{
    printf("The connected devices:");
    for (auto str : devices_) {
        printf("device : %s", str.c_str());
    }
}

bool WfdSinkDemo::Play(std::string deviceId)
{
    if (client_->Play(deviceId) == -1) {
        printf("play error, deviceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

bool WfdSinkDemo::Pause(std::string deviceId)
{
    if (client_->Pause(deviceId) == -1) {
        printf("Pause error, deviceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

bool WfdSinkDemo::Close(std::string deviceId)
{
    RemoveDevice(deviceId);
    if (client_->Close(deviceId) == -1) {
        printf("close error, deviceId: %s", deviceId.c_str());
        return false;
    }
    return true;
}

void WfdSinkDemo::DoCmd(std::string cmd)
{
    const std::map<std::string, int> cmd2index = {
        {"GetConfig", 1},   {"Start", 2},          {"Stop", 3},          {"SetMediaFormat", 4}, {"Play", 5},
        {"Pause", 6},       {"Close", 7},          {"Mute", 8},          {"UnMute", 9},         {"SetSceneType", 10},
        {"ListDevice", 11}, {"AppendSurface", 12}, {"RemoveSurface", 13}};

    if (cmd2index.count(cmd) == 0) {
        printf("command: %s not found", cmd.c_str());
        return;
    }
    printf("enter commond, the commond is %s, the id is %d", cmd.c_str(), cmd2index.at(cmd));
    std::string input;
    std::string deviceId;
    std::string surfaceId;
    VideoFormat videoFormatId = VIDEO_1920X1080_30;
    AudioFormat audioFormatId = AUDIO_48000_16_2;
    SceneType sceneType = FOREGROUND;
    // input params
    switch (cmd2index.at(cmd)) {
        case 5:  // play
        case 6:  // pause
        case 7:  // close
        case 8:  // mute
        case 9:  // unmute
        case 12: // AppendSurface
            printf("please input deviceId:");
            getline(std::cin, input);
            if (input != "") {
                deviceId = input;
            }
            break;
        case 10: // SetSceneType
            printf("please input deviceId:");
            getline(std::cin, input);
            if (input != "") {
                deviceId = input;
            }
            printf("please input surfaceId:");
            getline(std::cin, input);
            if (input != "") {
                surfaceId = input;
            }
            printf("please input scenetype: default for 0, (0: FOREGROUND, 1: BACKGROUND)");
            getline(std::cin, input);
            if (input != "") {
                sceneType = static_cast<SceneType>(atoi(input.c_str()));
            }
            break;
        case 4: // SetMediaFormat
            printf("please input deviceId:");
            getline(std::cin, input);
            if (input != "") {
                deviceId = input;
            }
            getline(std::cin, input);
            if (input != "") {
                videoFormatId = static_cast<VideoFormat>(atoi(input.c_str()));
            }
            getline(std::cin, input);
            if (input != "") {
                audioFormatId = static_cast<AudioFormat>(atoi(input.c_str()));
            }
            break;
        case 13: // delsurface
            printf("please input deviceId:");
            getline(std::cin, input);
            if (input != "") {
                deviceId = input;
            }
            printf("please input surfaceId:");
            getline(std::cin, input);
            if (input != "") {
                surfaceId = input;
            }
            break;
        default:
            break;
    }
    // execute command
    switch (cmd2index.at(cmd)) {
        case 1: // GetConfig
            if (GetConfig()) {
                printf("GetConfig success");
            }
            break;
        case 2: // start
            if (Start()) {
                printf("p2p start success");
            }
            break;
        case 3: // Stop
            if (Stop()) {
                printf("p2p stop success");
            }
            break;
        case 4: // SetMediaFormat
            if (SetMediaFormat(deviceId, videoFormatId, audioFormatId)) {
                printf("Start success");
            }
            break;
        case 5: // Play
            if (Play(deviceId)) {
                printf("Play success");
            }
            break;
        case 6: // Close
            if (Pause(deviceId)) {
                printf("Pause success");
            }
            break;
        case 7: // Close
            if (Close(deviceId)) {
                printf("Close success");
            }
            break;
        case 8: // Mute
            if (Mute(deviceId)) {
                printf("Mute success");
            }
            break;
        case 9: // UnMute
            if (UnMute(deviceId)) {
                printf("UnMute success");
            }
            break;
        case 10: // SetSceneType
            if (SetSceneType(deviceId, surfaceId, sceneType)) {
                printf("UnMute success");
            }
            break;
        case 11: // ListDevice
            ListDevices();
            break;
        case 12:
            AppendSurface(deviceId);
            break;
        case 13:
            RemoveSurface(deviceId, surfaceId);
            break;
        default:
            break;
    }
}

void WfdSinkDemoListener::OnError(uint32_t regionId, uint32_t agentId, SharingErrorCode errorCode)
{
    printf("on error. errorCode: %s, %d", std::string(magic_enum::enum_name(errorCode)).c_str(), errorCode);
}

void WfdSinkDemoListener::OnInfo(std::shared_ptr<BaseMsg> &msg)
{
    printf("on Info. msgId: %d", msg->GetMsgId());
    switch (msg->GetMsgId()) {
        case WfdConnectionChangedMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdConnectionChangedMsg>(msg);
            ConnectionInfo info;
            info.ip = data->ip;
            info.mac = data->mac;
            info.state = static_cast<ConnectionState>(data->state);
            info.surfaceId = data->surfaceId;
            info.deviceName = data->deviceName;
            info.primaryDeviceType = data->primaryDeviceType;
            info.secondaryDeviceType = data->secondaryDeviceType;
            OnConnectionChanged(info);
            break;
        }
        default:
            break;
    }
}

void WfdSinkDemoListener::OnConnectionChanged(const ConnectionInfo &info)
{
    auto listener = listener_.lock();
    if (!listener) {
        printf("no listener");
    }
    switch (info.state) {
        case ConnectionState::CONNECTED: {
            listener->AddDevice(info.mac);
            listener->AppendSurface(info.mac);
            listener->SetMediaFormat(info.mac, DEFAULT_VIDEO_FORMAT, DEFAULT_AUDIO_FORMAT);
            listener->Play(info.mac);
            break;
        }
        case ConnectionState::DISCONNECTED: {
            listener->RemoveDevice(info.mac);
            break;
        }
        default:
            break;
    }
    printf("on OnConnectionChanged. ip: %s, mac: %s, surfaceId: %llu", info.ip.c_str(), info.mac.c_str(),
           info.surfaceId);
}

int TestOneByOne()
{
    printf("ENTER TEST");
    std::map<std::string, std::string> cmdMap = {
        {"1", "GetConfig"}, {"2", "AppendSurface"}, {"3", "SetMediaFormat"}, {"4", "Start"},
        {"5", "Stop"},      {"6", "ListDevice"},    {"7", "Play"},           {"8", "Pause"},
        {"9", "Close"},     {"10", "SetSceneType"}, {"11", "Mute"},          {"12", "UnMute"}};

    std::string helpNotice = "select steps:     0-quit;"
                             "1-GetConfig;      2-AppendSurface;"
                             "3-SetMediaFormat; 4-Start;"
                             "5-Stop;           6-ListDevice;"
                             "7-Play;           8-Pause;"
                             "9-Close;          10-SetSceneType;"
                             "11-Mute;          12-UnMute";

    std::shared_ptr<WfdSinkDemo> demo = std::make_shared<WfdSinkDemo>();
    if (!demo->CreateClient()) {
        printf("create client failed");
        return -1;
    }
    std::string inputCmd;
    while (1) {
        printf("%s", helpNotice.c_str());
        getline(std::cin, inputCmd);
        if (inputCmd == "") {
            continue;
        } else if (inputCmd == "0") {
            break;
        } else {
            if (cmdMap.count(inputCmd) == 0) {
                printf("no cmd: %s, input again", inputCmd.c_str());
                continue;
            } else {
                demo->DoCmd(cmdMap[inputCmd]);
            }
        }
    }

    return 0;
}

int main()
{
    printf("wfd sink test start!");
    TestOneByOne();
    return 0;
}