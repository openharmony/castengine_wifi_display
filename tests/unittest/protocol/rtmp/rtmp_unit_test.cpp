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
#include <sstream>
#include <unistd.h>
#include "common/sharing_log.h"
#include "protocol/rtmp/include/rtmp_client.h"

using namespace std;
using namespace OHOS::Sharing;
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
class RtmpUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

namespace {
HWTEST_F(RtmpUnitTest, RtmpUnitTest_001, TestSize.Level0)
{
    const std::string url = "rtmp://192.168.62.35/live/0";
    auto rtmpClient = std::make_shared<RtmpClient>(url);
    ASSERT_TRUE(rtmpClient != nullptr);
}
} // namespace
} // namespace Sharing
} // namespace OHOS