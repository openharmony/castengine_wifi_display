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

#include "wfd_demo.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include "extend/magic_enum/magic_enum.hpp"
#include "impl/scene/wfd/wfd_def.h"
#include "math.h"
#include "surface.h"
#include "surface_utils.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
#include "utils/utils.h"
#include "window.h"
#include "window_option.h"

using namespace OHOS;
using namespace OHOS::Sharing;

VideoFormat DEFAULT_VIDEO_FORMAT = VideoFormat::VIDEO_1920X1080_30;
AudioFormat DEFAULT_AUDIO_FORMAT = AudioFormat::AUDIO_48000_16_2;
int32_t DEFAULT_WINDOW_WIDTH = 1920;
int32_t DEFAULT_WINDOW_HEIGHT = 1080;
std::vector<std::pair<int32_t, int32_t>> position{{0, 0}, {960, 0}, {0, 540}, {960, 540}};

bool WfdDemo::Init(const WfdMode mode)
{
    std::cout << "to get wifi display " << std::string(magic_enum::enum_name(mode)).c_str() << '\n';
    client_ = MiracastFactory::GetInstance(mode);
    if (!client_) {
        std::cout << "create wfd client error\n";
        return false;
    }
    client_->SetListener(shared_from_this());

    // when mode is SINK, init a window to display
    if (mode == SINK) {
        InitWindow();
        WifiDisplayTable_.emplace("Play", std::bind(&WifiDisplay::Play, client_, std::placeholders::_1));
        WifiDisplayTable_.emplace("Pause", std::bind(&WifiDisplay::Pause, client_, std::placeholders::_1));
        WifiDisplayTable_.emplace("Close", std::bind(&WifiDisplay::Close, client_, std::placeholders::_1));
    }
    return true;
}

void WfdDemo::InitWindow()
{
    if (!surfaceIds_.empty())
        return;
    std::cout << "create window enter\n";

    for (int i = 0; i < windowNum_; i++) {
        sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
        option->SetWindowRect({position[i].first, position[i].second, DEFAULT_WINDOW_WIDTH / sqrt(windowNum_),
                               DEFAULT_WINDOW_HEIGHT / sqrt(windowNum_)});
        option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
        option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FULLSCREEN);
        sptr<Rosen::Window> window = Rosen::Window::Create("wifi display window:" + std::to_string(i), option);
        auto surfaceNode = window->GetSurfaceNode();
        surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE);
        Rosen::RSTransaction::FlushImplicitTransaction();
        sptr<Surface> surface = surfaceNode->GetSurface();
        window->SetRequestedOrientation(Rosen::Orientation::HORIZONTAL);
        window->Show();
        auto surfaceId = surface->GetUniqueId();
        surfaceIds_.push_back(surfaceId);
        int ret = SurfaceUtils::GetInstance()->Add(surfaceId, surface);
        if (ret != 0)
            std::cout << "add failed\n";
        devicesIsPlaying_.push_back("");
    }
}

void WfdDemo::SelectMediaFormat()
{
    videoAttr_.codecType = CodecType::CODEC_H264;
    videoAttr_.format = 7;
    audioAttr_.codecType = CodecType::CODEC_AAC;
    audioAttr_.format = 43;

    std::cout << "please input videoFormatId:(default 4)\n";
    std::cout << "0: VIDEO_640X480_60\n";
    std::cout << "1: VIDEO_1280X720_25\n";
    std::cout << "2: VIDEO_1280X720_30\n";
    std::cout << "4: VIDEO_1920X1080_25\n";
    std::cout << "5: VIDEO_1920X1080_30\n";

    std::string input;
    getline(std::cin, input);
    if (input != "") {
        videoAttr_.format = static_cast<VideoFormat>(atoi(input.c_str()) + 2);
    }

    std::cout << "please input audioFormatId:\n(default 13)";
    std::cout << "0: AUDIO_44100_8_1\n";
    std::cout << "1: AUDIO_44100_8_2\n";
    std::cout << "2: AUDIO_44100_16_1\n";
    std::cout << "3: AUDIO_44100_16_2\n";
    std::cout << "10: AUDIO_48000_8_1\n";
    std::cout << "11: AUDIO_48000_8_2\n";
    std::cout << "12: AUDIO_48000_16_1\n";
    std::cout << "13: AUDIO_48000_16_2\n";

    getline(std::cin, input);
    if (input != "") {
        audioAttr_.format = static_cast<AudioFormat>(atoi(input.c_str()) + 30);
    }
}

void WfdDemo::AddDevice() {}
void WfdDemo::RemoveDevice() {}

void WfdDemo::OnError(const CastErrorInfo &errorInfo)
{
    std::cout << "on error. deviceId : " << errorInfo.deviceId << ", errorCode : " << errorInfo.errorCode << '\n';
}

void WfdDemo::OnDeviceState(const CastDeviceInfo &deviceInfo)
{
    switch (deviceInfo.state) {
        case CastDeviceState::CONNECTED: {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                for (int item = 0; item < windowNum_; item++) {
                    if (devicesIsPlaying_[item] == "") {
                        client_->AppendSurface(deviceInfo.deviceId, surfaceIds_[item]);
                        client_->SetMediaFormat(deviceInfo.deviceId, videoAttr_, audioAttr_);
                        client_->Play(deviceInfo.deviceId);
                        devicesIsPlaying_[item] = deviceInfo.deviceId;
                        break;
                    }
                }
                break;
            }
        }
        case CastDeviceState::DISCONNECTED: {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                for (int item = 0; item < windowNum_; item++) {
                    if (devicesIsPlaying_[item] == deviceInfo.deviceId) {
                        devicesIsPlaying_[item] = "";
                    }
                }
                break;
            }
        }
        default:
            break;
    }
    std::cout << "on OnConnectionChanged : \n";
    std::cout << "ip : " << deviceInfo.ipAddr.c_str() << '\n';
    std::cout << "mac : " << deviceInfo.deviceId.c_str() << '\n';
    std::cout << "state: " << std::string(magic_enum::enum_name(deviceInfo.state)).c_str() << '\n';
}

void WfdDemo::OnDeviceFound(const std::vector<CastDeviceInfo> &deviceInfos)
{
    for (int deviceNum = 0; deviceNum < deviceInfos.size(); deviceNum++) {
        std::cout << deviceNum << ". device id : " << deviceInfos[deviceNum].deviceId
                  << " device name : " << deviceInfos[deviceNum].deviceName << "\n";
    }
}

void WfdDemo::RunWfdSink()
{
    if (!Init(SINK))
        return;

    if (client_->Start() == 0) {
        std::cout << "wifi display sink service start!\n";
    }
    SelectMediaFormat();

    std::map<std::string, std::string> cmdMap = {{"1", "Start"},      {"2", "Stop"},  {"3", "SelectMediaFormat"},
                                                 {"4", "Play"},       {"5", "Pause"}, {"6", "Close"},
                                                 {"12", "ListDevice"}};

    std::string helpNotice = "select steps:        0-quit;\n"
                             "1-Start;             2-Stop;\n"
                             "3-SelectMediaFormat; 4-Play;\n"
                             "5-Pause;             6-Close;\n"
                             "12-ListDevice\n";

    std::string inputCmd;
    while (1) {
        std::cout << helpNotice;
        getline(std::cin, inputCmd);
        if (inputCmd == "") {
            continue;
        } else if (inputCmd == "0") {
            break;
        } else {
            if (cmdMap.count(inputCmd) == 0) {
                std::cout << "no cmd: " << inputCmd << ", input agin\n";
                continue;
            } else {
                DoCmd(cmdMap[inputCmd]);
            }
        }
    }
}

void WfdDemo::RunWfdSource() {}

void WfdDemo::DoCmd(std::string cmd)
{
    if (!client_)
        return;
    if (cmd.find("Start") != std::string::npos) {
        client_->Start();
    } else if (cmd.find("Stop") != std::string::npos) {
        client_->Stop();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            for (int item = 0; item < devicesIsPlaying_.size(); item++)
                devicesIsPlaying_[item] = "";
        }
    } else if (WifiDisplayTable_.find(cmd) != WifiDisplayTable_.end()) {
        std::cout << "enter the window number to operate: \n";
        int32_t winNum = 0;
        std::string input;
        getline(std::cin, input);
        if (input != "") {
            winNum = static_cast<int32_t>(atoi(input.c_str()));
            if (winNum >= windowNum_) {
                std::cout << "the window not exits\n";
                return;
            }
        }
        auto iter = WifiDisplayTable_.find(cmd);
        auto func = iter->second;
        func(devicesIsPlaying_[winNum]);
    } else if (cmd.find("SelectMediaFormat") != std::string::npos) {
        SelectMediaFormat();
    } else if (cmd.find("StartDiscover") != std::string::npos) {
        client_->StartDiscover();
    } else if (cmd.find("StopDiscover") != std::string::npos) {
        client_->StopDiscover();
    } else if (cmd.find("AddDevice") != std::string::npos) {
        CastDeviceInfo info;
        client_->AddDevice(info);
    } else if (cmd.find("RemoveDevice") != std::string::npos) {
        client_->RemoveDevice("0.0.0.0");
    } else {
        std::cout << "operation is invalid\n";
    }

    return;
}

int main()
{
    std::cout << "Please select a demo scenario number(default sink): \n";
    std::cout << "0:wfd sink\n";
    std::cout << "1:wfd source\n";

    auto wifiDisplay = std::make_shared<WfdDemo>();

    std::string mode;
    (void)getline(std::cin, mode);
    if (mode == "" || mode == "0") {
        wifiDisplay->RunWfdSink();
    } else if (mode == "1") {
        wifiDisplay->RunWfdSource();
    } else {
        std::cout << "no that selection\n";
        return 0;
    }

    std::cout << "wfd test end!\n";
    return 0;
}