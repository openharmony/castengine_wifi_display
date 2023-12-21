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

#include "json_parser.h"
#include <errors.h>
#include <fstream>
#include <memory>
#include <unistd.h>
#include "common/common_macro.h"
#include "media_log.h"

namespace OHOS {
namespace Sharing {
int32_t JsonParser::GetConfig(std::string &fileName, SharingData::Ptr &value)
{
    SHARING_LOGD("trace.");
    Json::Value root;
    std::ifstream ifs;
    ifs.open(fileName.c_str());

    if (!ifs.is_open()) {
        SHARING_LOGE("cannot open %{public}s.", fileName.c_str());
        return -1;
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    JSONCPP_STRING errs;

    if (!parseFromStream(builder, ifs, &root, &errs)) {
        SHARING_LOGE("parse file error: %{public}s.", errs.c_str());
        return -1;
    }

    SHARING_LOGD("parse json:\n%{public}s.", root.toStyledString().c_str());
    for (auto &modules : root) {
        ReadModuleConfig(modules, value);
    }

    ifs.close();
    return 0;
}

int32_t JsonParser::SaveConfig(std::string &fileName, SharingData::Ptr &value)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(value);
    std::ofstream ofs;
    ofs.open(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

    if (!ofs.is_open()) {
        SHARING_LOGE("open file %{public}s err.", fileName.c_str());
        return -1;
    }

    Json::Value root(Json::ValueType::objectValue);
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "All";
    Json::StreamWriter *writer(builder.newStreamWriter());

    value->ForEach([&](const std::string &moduleName, const SharingDataGroupByModule::Ptr &moduleValue) {
        Json::Value moduleArray(Json::ValueType::arrayValue);
        SaveModuleConfig(moduleArray, moduleValue);
        root[moduleName] = moduleArray;
    });

    MEDIA_LOGD("save json:\n%{public}s.", root.toStyledString().c_str());
    writer->write(root, &ofs);
    ofs.close();
    return 0;
}

int32_t JsonParser::ReadModuleConfig(Json::Value &modules, SharingData::Ptr &value)
{
    SHARING_LOGD("trace.");
    if (modules.empty()) {
        SHARING_LOGE("this module is empty.");
        return -1;
    }

    auto moduleNames = modules.getMemberNames();
    for (auto &moduleName : moduleNames) {
        MEDIA_LOGD("take down %{public}s: %{public}s.", moduleName.c_str(),
                   modules[moduleName].toStyledString().c_str());
        auto moduleValue = std::make_shared<SharingDataGroupByModule>(moduleName);
        auto module = modules[moduleName];
        for (auto &item : module) {
            auto tag = item["tag"].asString();
            auto tagValue = std::make_shared<SharingDataGroupByTag>(tag);
            moduleValue->PutSharingValues(tag, tagValue);
            auto keyNames = item.getMemberNames();
            for (auto &key : keyNames) {
                if (key == "tag") {
                    continue;
                }
                if (key == "isEnable" && tag == "mediaLog") {
                    g_logOn = item[key].asBool();
                }
                SharingValue::Ptr sharingData = nullptr;
                if (item[key].isString()) {
                    std::string value = item[key].asString();
                    sharingData = std::make_shared<SharingValue>(value);
                } else if (item[key].isInt()) {
                    int32_t value = item[key].asInt();
                    sharingData = std::make_shared<SharingValue>(value);
                } else if (item[key].isBool()) {
                    bool value = item[key].asBool();
                    sharingData = std::make_shared<SharingValue>(value);
                } else if (item[key].isArray()) {
                    std::vector<int32_t> value;
                    for (auto &it : item[key]) {
                        value.emplace_back(it.asInt());
                    }
                    sharingData = std::make_shared<SharingValue>(value);
                }
                tagValue->PutSharingValue(key, sharingData);
            }
        }
        value->PutSharingValues(moduleValue, moduleName);
    }

    return 0;
}

int32_t JsonParser::SaveModuleConfig(Json::Value &module, const SharingDataGroupByModule::Ptr &moduleValue)
{
    SHARING_LOGD("trace.");
    moduleValue->ForEach([&](const std::string &tagName, const SharingDataGroupByTag::Ptr &tagValue) {
        Json::Value tagObject(Json::ValueType::objectValue);
        tagObject["tag"] = tagName;
        tagValue->ForEach([&](const std::string &key, const SharingValue::Ptr &value) {
            if (value->IsBool()) {
                bool data;
                if (value->GetValue(data)) {
                    tagObject[key] = data;
                }
            } else if (value->IsInt32()) {
                int32_t data;
                if (value->GetValue(data)) {
                    tagObject[key] = data;
                }
            } else if (value->IsString()) {
                std::string data;
                if (value->GetValue(data)) {
                    tagObject[key] = data;
                }
            } else if (value->IsVector()) {
                std::vector<int32_t> data;
                if (value->GetValue(data)) {
                    Json::Value vec(Json::ValueType::arrayValue);
                    for (auto &item : data) {
                        vec.append(item);
                    }
                    tagObject[key] = vec;
                }
            }
        });
        module.append(tagObject);
    });

    return 0;
}
} // namespace Sharing
} // namespace OHOS