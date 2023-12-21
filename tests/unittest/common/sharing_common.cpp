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

#include <config.h>
#include <gtest/gtest.h>
#include "common/sharing_log.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

const string CONFIGURE_CFG_ALL = "cfg";

namespace {
class SharingConfigureTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(SharingConfigureTest, SharingConfigureInitTest_001, TestSize.Level1)
{
    SHARING_LOGD("trace");
    Config::GetInstance().Init();
    usleep(1000000); // 1000000 for 1s
}
} // namespace