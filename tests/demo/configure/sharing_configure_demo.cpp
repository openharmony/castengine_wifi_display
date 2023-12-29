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

#include <iostream>
#include <unistd.h>
#include "common/sharing_log.h"
#include "config.h"

using namespace OHOS::Sharing;

int main()
{
    Config::GetInstance().Init();
    sleep(1);
    // SharingConfigureSetConfigTest_001
    SHARING_LOGD("this is sharing_configure_demo!");
    while (1) {
        std::string inputCmd;
        getline(std::cin, inputCmd);
        if (inputCmd == "0") {
            SharingValue::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", "maxrecvport", values);
            values->Print();
            SharingValue::Ptr valueS = std::make_shared<SharingValue>(8888);
            ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", "maxrecvport", valueS);
            valueS->Print();
            sleep(1);
        } else if (inputCmd == "1") {
            SharingDataGroupByModule::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("cfg", values);
            std::string data = std::string("test");
            SharingValue::Ptr valueS = std::make_shared<SharingValue>(data);
            valueS->Print();
            values->PutSharingValue("rtpport111", "minrecv", valueS);
            ret = Config::GetInstance().SetConfig("cfg", values);
            sleep(1);
        }
        // SharingConfigureSetConfigTest_002
        else if (inputCmd == "2") {
            SharingData::Ptr datas = nullptr;
            auto ret = Config::GetInstance().GetConfig(datas);
            ret = Config::GetInstance().SetConfig(datas);
            sleep(1);
        } else if (inputCmd == "3") {
            SharingDataGroupByModule::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("mediachannel", values);
            values->Print();
            ret = Config::GetInstance().SetConfig("mediachannel", values);
            sleep(1);
        } else if (inputCmd == "4") {
            SharingDataGroupByTag::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", values);
            values->Print();
            ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", values);
            sleep(1);
        } else if (inputCmd == "5") {
            SharingDataGroupByTag::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("mediachannel", "rtmpport", values);
            values->Print();
            SharingValue::Ptr valueS = std::make_shared<SharingValue>(8080);
            valueS->Print();
            values->PutSharingValue("tomcat", valueS);

            std::unordered_map<std::string, SharingValue::Ptr> datas;
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
            std::unordered_map<std::string, SharingValue::Ptr> values1;
            values->GetSharingValues(values1);
            SHARING_LOGD("hasKey: %{public}d,IsTag: %{public}d,HasKey: %{public}d,IsTag: %{public}d,size: %{public}d!",
                         bret1, bret2, bret3, bret4, values1.size());

            ret = Config::GetInstance().SetConfig("mediachannel", "rtmpport", values);
            sleep(1);
        } else if (inputCmd == "6") {
            SharingDataGroupByModule::Ptr values = nullptr;
            auto ret = Config::GetInstance().GetConfig("mediachannel", values);
            values->Print();
            SharingValue::Ptr valueS = std::make_shared<SharingValue>(8080);
            valueS->Print();
            values->PutSharingValue("apache", "tomcat", valueS);

            std::unordered_map<std::string, SharingValue::Ptr> datas;
            {
                SharingValue::Ptr valueS1 = std::make_shared<SharingValue>(80);
                datas.emplace(std::make_pair("http", valueS1));
                SharingValue::Ptr valueS2 = std::make_shared<SharingValue>(443);
                datas.emplace(std::make_pair("ssl", valueS2));
                values->PutSharingValues("apache", datas);
            }
            SharingValue::Ptr value1 = values->GetSharingValue("apache", "http");
            value1->Print();
            std::unordered_map<std::string, SharingValue::Ptr> values1;
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
                "HasKey: %{public}d,HasTag: %{public}d,HasKey: %{public}d,HasTag: %{public}d,IsModule: %{public}d,IsModule="
                "%{public}d,size: %{public}d.",
                bret1, bret2, bret3, bret4, bret5, bret6, values1.size());
            ret = Config::GetInstance().SetConfig("mediachannel", values);
            sleep(1);
        } else if (inputCmd == "7") {
            SharingData::Ptr datas = nullptr;
            auto ret = Config::GetInstance().GetConfig(datas);
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
                std::unordered_map<std::string, SharingValue::Ptr> mapdata;
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
                std::unordered_map<std::string, SharingValue::Ptr> mapdata;
                datas->GetSharingValues(mapdata, "tomcat", "web");
                SHARING_LOGD("mapdata size: %{public}d,", mapdata.size());
            }

            bool bret1 = datas->HasKey("http");
            bool bret2 = datas->HasTag("rtmpport");
            bool bret3 = datas->HasKey("http1");
            bool bret4 = datas->HasTag("rtmpport2");
            bool bret5 = datas->HasModule("mediachannel");
            bool bret6 = datas->HasModule("mediachannel2");
            SHARING_LOGD(
                "hasKey: %{public}d,HasTag: %{public}d,HasKey: %{public}d,HasTag: %{public}d,HasModule: %{public}d,"
                "HasModule="
                "%{public}d.",
                bret1, bret2, bret3, bret4, bret5, bret6);

            ret = Config::GetInstance().SetConfig(datas);
            sleep(1);
        }
    }

    sleep(3);

    return 0;
}