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

#include <gtest/gtest.h>
#include <iostream>
#include "common/sharing_log.h"
#include "mediachannel/interfaces/channel_manager.h"
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
class MediaChannelUnitTest : public testing::Test {};

namespace {
HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate01, Function | SmallTest | Level2)
{
    MediaChannelInfo info;
    info.mediaSessionClassName = "test";
    auto mediachannel = ChannelManager::GetInstance()->HandleMediaChannelCreate(info);
    EXPECT_EQ(mediachannel, nullptr);

    info.mediaSessionClassName = "NetworkMediaSession";
    mediachannel = ChannelManager::GetInstance()->HandleMediaChannelCreate(info);
    EXPECT_NE(mediachannel, nullptr);
}
} // namespace
} // namespace Sharing
} // namespace OHOS