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

#include "wfd_source_demo.h"
#include <iostream>
#include "magic_enum.hpp"

using namespace OHOS::Sharing;

WfdSourceDemo::WfdSourceDemo()
{
    listener_ = std::make_shared<WfdSourceDemoListener>();
    if (!listener_) {
        printf("create WfdSourceDemoListener failed.\n");
    }
}

std::shared_ptr<WfdSource> WfdSourceDemo::GetClient()
{
    return client_;
}

bool WfdSourceDemo::CreateClient()
{
    client_ = WfdSourceFactory::CreateSource(0, "wfd_Source_demo");
    if (!client_) {
        printf("create wfdSource client error.\n");
        return false;
    }
    if (listener_) {
        listener_->SetListener(shared_from_this());
        client_->SetListener(listener_);
        return true;
    } else {
        printf("Listener is nullptr.\n");
        return false;
    }
}

bool WfdSourceDemo::StartDiscover()
{
    if (!client_) {
        printf("client is null.\n");
        return false;
    }

    client_->StartDiscover();
    return true;
}

bool WfdSourceDemo::StopDiscover()
{
    if (!client_) {
        printf("client is null.\n");
        return false;
    }

    client_->StopDiscover();
    return true;
}

bool WfdSourceDemo::Start()
{
    printf("enter start.\n");
    if (!client_) {
        printf("client is nullptr.\n");
        return false;
    }
    if (client_->StartDiscover() == -1) {
        printf("Source start error.\n");
        return false;
    }
    return true;
}

bool WfdSourceDemo::Stop()
{
    if (client_->StopDiscover() == -1) {
        printf("Source stop error.\n");
        return false;
    }
    return true;
}

static std::string g_deviceId;
void WfdSourceDemo::AddCastDevice(const std::string deviceId)
{
    printf("add device: %s.\n", deviceId.c_str());
    WfdCastDeviceInfo deviceInfo;
    deviceInfo.deviceId = deviceId;
    if (client_->AddDevice(0, deviceInfo) == -1) {
        printf("AddDevice error.\n");
    }
    g_deviceId = deviceId;
}

void WfdSourceDemo::RemoveCastDevice(const std::string deviceId)
{
    printf("remove device: %s.\n", deviceId.c_str());
    if (client_->RemoveDevice(deviceId) == -1) {
        printf("RemoveDevice error.\n");
    }
}

void WfdSourceDemo::ExecuteCmd(int32_t cmd, std::string &deviceId)
{
    switch (cmd) {
        case 1: // 1: start
            if (Start()) {
                printf("p2p start success.\n");
            }
            break;
        case 2: // 2: Stop
            if (Stop()) {
                printf("p2p stop success.\n");
            }
            break;
        case 3: // 3: StartDiscover
            if (StartDiscover()) {
                printf("StartDiscover success.\n");
            }
            break;
        case 4: // 4: StopDiscover
            if (StopDiscover()) {
                printf("StartDiscover success.\n");
            }
            break;
        case 5: // 5: AddCastDevice
            AddCastDevice(deviceId);
            break;
        case 6: // 6: RemoveCastDevice
            RemoveCastDevice(deviceId);
            break;
        default:
            break;
    }
}

void WfdSourceDemo::DoCmd(std::string cmd)
{
    const std::map<std::string, int32_t> cmd2index = {{"Start", 1},         {"Stop", 2},
                                                      {"StartDiscover", 3}, {"StopDiscover", 4},
                                                      {"AddCastDevice", 5}, {"RemoveCastDevice", 6}};

    if (cmd2index.count(cmd) == 0) {
        printf("command: %s not found.", cmd.c_str());
        return;
    }
    printf("enter commond, the commond is %s, the id is %d.", cmd.c_str(), cmd2index.at(cmd));
    std::string input;
    std::string deviceId;
    // input params
    switch (cmd2index.at(cmd)) {
        case 5: // 5: AddCastDevice
        case 6: // 6: RemoveCastDevice
            printf("please input deviceId:\n");
            getline(std::cin, input);
            if (input != "") {
                deviceId = input;
            }
            break;
        default:
            break;
    }
    // execute command
    ExecuteCmd(cmd2index.at(cmd), deviceId);
}

void WfdSourceDemoListener::OnError(uint32_t regionId, uint32_t agentId, SharingErrorCode errorCode)
{
    (void)regionId;
    (void)agentId;
    printf("on error. errorCode: %s, %d.", std::string(magic_enum::enum_name(errorCode)).c_str(), errorCode);
}

void WfdSourceDemoListener::OnInfo(std::shared_ptr<BaseMsg> &msg)
{
    printf("on info. msgId: %d.\n", msg->GetMsgId());
    switch (msg->GetMsgId()) {
        case WfdSourceDeviceFoundMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSourceDeviceFoundMsg>(msg);
            auto infos = data->deviceInfos;
            OnDeviceFound(infos);
            break;
        }
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
        case WfdErrorMsg::MSG_ID: {
            auto data = std::static_pointer_cast<WfdErrorMsg>(msg);
            printf("OnError: %d.\n", data->errorCode);
            auto listener = listener_.lock();
            if (!listener) {
                printf("no listener.\n");
            }
            listener->RemoveCastDevice(g_deviceId);
            break;
        }
        default:
            break;
    }
}

void WfdSourceDemoListener::OnDeviceFound(const std::vector<WfdCastDeviceInfo> &deviceInfos)
{
    auto listener = listener_.lock();
    if (!listener) {
        printf("no listener.\n");
    }

    for (auto itDev : deviceInfos) {
        printf("device found: %s\n"
               "\tmac: %s"
               "\tprimaryDeviceType: %s"
               "\tsecondaryDeviceType: %s.\n",
               itDev.deviceName.c_str(), itDev.deviceId.c_str(), itDev.primaryDeviceType.c_str(),
               itDev.secondaryDeviceType.c_str());
    }
}

void WfdSourceDemoListener::OnConnectionChanged(const ConnectionInfo &info)
{
    auto listener = listener_.lock();
    if (!listener) {
        printf("no listener.\n");
    }

    printf("on OnConnectionChanged. state: %d, ip: %s, mac: %s.\n", info.state, info.ip.c_str(), info.mac.c_str());
}

static int TestOneByOne()
{
    printf("ENTER TEST.\n");
    std::map<std::string, std::string> cmdMap = {{"1", "Start"},         {"2", "Stop"},
                                                 {"3", "StartDiscover"}, {"4", "StopDiscover"},
                                                 {"5", "AddCastDevice"}, {"6", "RemoveCastDevice"}};

    std::string helpNotice = "select steps:    0-quit;\n"
                             "1-Start;         2-Stop;\n"
                             "3-StartDiscover; 4-StopDiscover\n"
                             "5-AddCastDevice; 6-RemoveCastDevice\n";

    std::shared_ptr<WfdSourceDemo> demo = std::make_shared<WfdSourceDemo>();
    if (!demo->CreateClient()) {
        printf("create client failed.\n");
        return -1;
    }
    std::string inputCmd;
    bool exit = false;
    while (!exit) {
        printf("%s.\n", helpNotice.c_str());
        getline(std::cin, inputCmd);
        if (inputCmd == "") {
            continue;
        } else if (inputCmd == "0") {
            exit = true;
        } else {
            if (cmdMap.count(inputCmd) == 0) {
                printf("no cmd: %s, input again.", inputCmd.c_str());
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
    printf("wfd source test start!\n");
    TestOneByOne();
    return 0;
}
