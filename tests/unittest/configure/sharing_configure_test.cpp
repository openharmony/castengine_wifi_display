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
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_001, TestSize.Level1)
{
    usleep(1000000);
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("cfg", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_002, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("module", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_003, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_004, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("interaction", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_005, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("window", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_006, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("context", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_007, TestSize.Level1)
{
    SharingData::Ptr datas = nullptr;
    auto ret = Config::GetInstance().GetConfig(datas);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    datas->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_008, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("cfg", "1", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_009, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("cfg", "rtpport22", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_010, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("module", "rtpport", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_011, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtpport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_012, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtpport1", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_013, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel1", "rtpport", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_014, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel1", "maxrecvport1", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_015, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("interaction", "tag1", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_016, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("interaction", "key2", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_017, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("window", "tag1", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_Error_018, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("window", "key2", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_019, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("context", "tag1", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
}

HWTEST_F(SharingConfigureTest, SharingConfigureGetConfigTest_020, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("context", "key2", values);
    ASSERT_TRUE(ret != CONFIGURE_ERROR_NONE);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_001, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig(CONFIGURE_CFG_ALL, values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    ret = Config::GetInstance().SetConfig(CONFIGURE_CFG_ALL, values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000); // 1000000 for 1s
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_002, TestSize.Level1)
{
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(8888);
    auto ret = Config::GetInstance().SetConfig("newmodule", "newtag", "newkey", valueS);
    valueS->Print();
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_003, TestSize.Level1)
{
    SharingValue::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", "maxrecvport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(8888);
    ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", "maxrecvport", valueS);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    valueS->Print();
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_004, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("cfg", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    std::string data = std::string("test");
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(data);
    valueS->Print();
    values->PutSharingValue("rtpport111", "minrecv", valueS);
    ret = Config::GetInstance().SetConfig("cfg", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_005, TestSize.Level1)
{
    SharingData::Ptr datas = nullptr;
    auto ret = Config::GetInstance().GetConfig(datas);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    ret = Config::GetInstance().SetConfig(datas);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_006, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
    ret = Config::GetInstance().SetConfig("mediachannel", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_007, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
    ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_008, TestSize.Level1)
{
    SharingDataGroupByTag::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(8080);
    valueS->Print();
    values->PutSharingValue("tomcat", valueS);

    std::map<std::string, SharingValue::Ptr> datas;
    {
        SharingValue::Ptr valueS1 = std::make_shared<SharingValue>(8080);
        datas.emplace(std::make_pair("http", valueS1));
        SharingValue::Ptr valueS2 = std::make_shared<SharingValue>(443);
        datas.emplace(std::make_pair("ssl", valueS2));
        values->PutSharingValues(datas);
    }
    bool bret1 = values->HasKey("http");
    bool bret2 = values->IsTag("rtmpport");
    bool bret3 = values->HasKey("http1");
    bool bret4 = values->IsTag("rtmpport2");
    SharingValue::Ptr value1 = values->GetSharingValue("http");
    value1->Print();
    std::map<std::string, SharingValue::Ptr> values1;
    values->GetSharingValues(values1);
    SHARING_LOGD("hasKey: %{public}d,IsTag: %{public}d,HasKey: %{public}d,IsTag: %{public}d,size: %{public}d!", bret1,
                 bret2, bret3, bret4, values1.size());

    ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_009, TestSize.Level1)
{
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    values->Print();
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(8080);
    valueS->Print();
    values->PutSharingValue("apache", "tomcat", valueS);

    std::map<std::string, SharingValue::Ptr> datas;
    {
        SharingValue::Ptr valueS1 = std::make_shared<SharingValue>(80);
        datas.emplace(std::make_pair("http", valueS1));
        SharingValue::Ptr valueS2 = std::make_shared<SharingValue>(443);
        datas.emplace(std::make_pair("ssl", valueS2));
        values->PutSharingValues("apache", datas);
    }
    SharingValue::Ptr value1 = values->GetSharingValue("apache", "http");
    value1->Print();
    std::map<std::string, SharingValue::Ptr> values1;
    values->GetSharingValues("apache", values1);
    SharingDataGroupByTag::Ptr value3 = nullptr;
    values->GetSharingValues("apache", value3);
    value3->Print();
    {
        SharingValue::Ptr valueS = std::make_shared<SharingValue>(3306);
        valueS->Print();
        value3->PutSharingValue("mysql", valueS);
        value3->Print();
    }
    values->PutSharingValues("apache", value3);
    values->Print();

    bool bret1 = values->HasKey("", "http");
    bool bret2 = values->HasTag("rtmpport");
    bool bret3 = values->HasKey("", "http1");
    bool bret4 = values->HasTag("rtmpport2");
    bool bret5 = values->IsModule("mediachannel");
    bool bret6 = values->IsModule("mediachannel2");
    SHARING_LOGD(
        "hasKey: %{public}d,HasTag: %{public}d,HasKey: %{public}d,HasTag: %{public}d,IsModule: %{public}d,IsModule="
        "%{public}d,size: %{public}d",
        bret1, bret2, bret3, bret4, bret5, bret6, values1.size());
    ret = Config::GetInstance().SetConfig("mediachannel", values);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}

HWTEST_F(SharingConfigureTest, SharingConfigureSetConfigTest_010, TestSize.Level1)
{
    SharingData::Ptr datas = nullptr;
    auto ret = Config::GetInstance().GetConfig(datas);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    datas->Print();

    std::string anyValue = std::string("007");
    SharingValue::Ptr valueS = std::make_shared<SharingValue>(anyValue);
    datas->PutSharingValue("id", valueS, "context", "newtag");
    datas->Print();

    {
        SharingValue::Ptr valueS = std::make_shared<SharingValue>(800);
        valueS->Print();
        datas->PutSharingValue("age", valueS, "context", "newtag");
        datas->Print();
    }
    {
        SharingValue::Ptr valueS = std::make_shared<SharingValue>(1);
        valueS->Print();
        datas->PutSharingValue("female", valueS, "context", "newtag");
        datas->Print();
    }
    {
        SharingValue::Ptr valueS = std::make_shared<SharingValue>(1);
        valueS->Print();
        datas->PutSharingValue("class", valueS, "context", "newtag");
        datas->Print();
    }

    {
        std::map<std::string, SharingValue::Ptr> mapdata;
        SharingValue::Ptr valueS1 = std::make_shared<SharingValue>(80);
        mapdata.emplace(std::make_pair("http", valueS1));
        SharingValue::Ptr valueS2 = std::make_shared<SharingValue>(443);
        mapdata.emplace(std::make_pair("ssl", valueS2));
        datas->PutSharingValues(mapdata, "tomcat", "web");
        SHARING_LOGD("mapdata size: %{public}d,", mapdata.size());
    }

    {
        SharingValue::Ptr value = nullptr;
        value = datas->GetSharingValue("ssl", "tomcat", "web");
        value->Print();
    }
    {
        SharingDataGroupByModule::Ptr values = nullptr;
        datas->GetSharingValues(values, "tomcat");
        values->Print();
    }
    {
        SharingDataGroupByTag::Ptr values = nullptr;
        datas->GetSharingValues(values, "tomcat", "web");
        values->Print();
    }
    {
        std::map<std::string, SharingValue::Ptr> mapdata;
        datas->GetSharingValues(mapdata, "tomcat", "web");
        SHARING_LOGD("mapdata size: %{public}d,", mapdata.size());
    }

    bool bret1 = datas->HasKey("http");
    bool bret2 = datas->HasTag("rtmpport");
    bool bret3 = datas->HasKey("http1");
    bool bret4 = datas->HasTag("rtmpport2");
    bool bret5 = datas->HasModule("mediachannel");
    bool bret6 = datas->HasModule("mediachannel2");
    SHARING_LOGD("hasKey: %{public}d,HasTag: %{public}d,HasKey: %{public}d,HasTag: %{public}d,HasModule: %{public}d,"
                 "HasModule="
                 "%{public}d",
                 bret1, bret2, bret3, bret4, bret5, bret6);

    ret = Config::GetInstance().SetConfig(datas);
    ASSERT_TRUE(ret == CONFIGURE_ERROR_NONE);
    usleep(1000000);
}
} // namespace