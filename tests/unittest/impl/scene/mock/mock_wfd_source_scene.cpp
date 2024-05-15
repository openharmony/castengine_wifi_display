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

#include "mock_wfd_source_scene.h"

using namespace std;
using namespace OHOS::Wifi;

namespace OHOS {
namespace Sharing {

void MockWfdSourceScene::MockInitialize()
{
    SHARING_LOGD("trace.");
    wfdP2pCallback_ = new WfdP2pCallback(shared_from_this());
    mockIpcAdapter_ = make_shared<MockIpcMsgAdapter>();
    mockSharingAdapter_ = make_shared<MockSharingAdapter>();
    mockWifiP2pInstance_ = make_shared<MockWifiP2p>();

    SetIpcAdapter(mockIpcAdapter_);
    sharingAdapter_ = mockSharingAdapter_;
    p2pInstance_ = mockWifiP2pInstance_;
}

std::shared_ptr<Wifi::WifiP2p> MockWfdSourceScene::MockGetP2pInstance()
{
    return p2pInstance_;
}

std::shared_ptr<IpcMsgAdapter> MockWfdSourceScene::MockGetIpcAdapter()
{
    return ipcAdapter_.lock();
}

std::shared_ptr<ISharingAdapter> MockWfdSourceScene::MockGetSharingAdapter()
{
    return sharingAdapter_.lock();
}

} // namespace Sharing
} // namespace OHOS
