/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_MOCK_WFD_SINK_SCENE_H
#define OHOS_SHARING_MOCK_WFD_SINK_SCENE_H

#include <gmock/gmock.h>
#include "mock_ipc_msg_adapter.h"
#include "mock_sharing_adapter.h"
#include "mock_wifi_p2p.h"
#define private public
#define protected public
#define final
#include "wfd_sink_scene.h"
#undef final
#undef private
#undef protected

namespace OHOS {
namespace Sharing {

class MockWfdSinkScene : public WfdSinkScene {
public:
    void MockInitialize();
    std::shared_ptr<Wifi::WifiP2p> MockGetP2pInstance();
    std::shared_ptr<IpcMsgAdapter> MockGetIpcAdapter();
    std::shared_ptr<ISharingAdapter> MockGetSharingAdapter();

public:
    sptr<WfdP2pCallback> wfdP2pCallback_ = nullptr;
    std::shared_ptr<MockWifiP2p> mockWifiP2pInstance_ = nullptr;
    std::shared_ptr<MockIpcMsgAdapter> mockIpcAdapter_ = nullptr;
    std::shared_ptr<MockSharingAdapter> mockSharingAdapter_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
