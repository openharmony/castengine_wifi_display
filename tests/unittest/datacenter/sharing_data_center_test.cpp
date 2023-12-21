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

#include <any>
#include <gtest/gtest.h>
#include "config.h"
#include "data_center.h"
#include "data_manager.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Sharing;

const string CONFIGURE_CFG_ALL = "cfg";
namespace {
class SharingDataCenterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(SharingDataCenterTest, SharingDataCenterTest_001, TestSize.Level1)
{
    DataCenter::GetInstance();
    Config::GetInstance().Init();
}
HWTEST_F(SharingDataCenterTest, SharingDataCenterTest_002, TestSize.Level1)
{
    std::map<std::string, SharingValue::Ptr> values;
    auto ret = DataCenter::GetInstance().GetSharingValues(values);
    ASSERT_TRUE(ret);
}

HWTEST_F(SharingDataCenterTest, SharingDataCenterTest_003, TestSize.Level1)
{
    std::map<std::string, SharingValue::Ptr> values;
    auto ret = DataCenter::GetInstance().GetSharingValues(values);
    ASSERT_TRUE(ret == 0);
    std::string value("test");
    std::any anyValue(value);
    values.emplace("test", std::make_shared<SharingValue>(anyValue));
    auto setRet = DataCenter::GetInstance().PutSharingValues(values);
    ASSERT_TRUE(setRet);
}

HWTEST_F(SharingDataCenterTest, SharingDataManagerTest_001, TestSize.Level1)
{
    auto &ret = DataManager::GetInstance();
    (void)ret;
}
} // namespace