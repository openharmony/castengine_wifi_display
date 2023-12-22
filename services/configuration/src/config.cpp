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

#include "config.h"
#include <thread>
#include "common/common_macro.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "json_parser.h"

namespace OHOS {
namespace Sharing {
int32_t Config::GetConfig(SharingData::Ptr &datas)
{
    SHARING_LOGD("trace.");
    datas = datas_;
    return CONFIGURE_ERROR_NONE;
}

int32_t Config::GetConfig(const std::string &module, SharingDataGroupByModule::Ptr &values)
{
    SHARING_LOGD("trace.");
    std::shared_lock<std::shared_mutex> lk(mutex_);
    if (datas_ == nullptr || !datas_->HasModule(module)) {
        SHARING_LOGE("data_s is null or has no module named %{public}s.", module.c_str());
        return CONFIGURE_ERROR_NOT_FIND;
    }

    return datas_->GetSharingValues(values, module);
}

int32_t Config::GetConfig(const std::string &module, const std::string &tag, SharingDataGroupByTag::Ptr &values)
{
    SHARING_LOGD("trace.");
    SharingDataGroupByModule::Ptr modelValue = nullptr;
    if (GetConfig(module, modelValue) != CONFIGURE_ERROR_NONE) {
        SHARING_LOGE("module %{public}s error.", module.c_str());
        return CONFIGURE_ERROR_NOT_FIND;
    }

    std::shared_lock<std::shared_mutex> lk(mutex_);
    return datas_->GetSharingValues(values, module, tag);
}

int32_t Config::GetConfig(const std::string &module, const std::string &tag, const std::string &key,
                          SharingValue::Ptr &value)
{
    SHARING_LOGD("trace.");
    SharingDataGroupByTag::Ptr tagValue = nullptr;
    if (GetConfig(module, tag, tagValue) != CONFIGURE_ERROR_NONE) {
        SHARING_LOGE("module %{public}s, tag %{public}s error.", module.c_str(), tag.c_str());
        return CONFIGURE_ERROR_NOT_FIND;
    }

    std::shared_lock<std::shared_mutex> lk(mutex_);
    SharingValue::Ptr valueTmp = datas_->GetSharingValue(key, module, tag);
    if (valueTmp == nullptr) {
        SHARING_LOGE("sharing value is null.");
        return CONFIGURE_ERROR_NOT_FIND;
    }

    value = valueTmp;
    return CONFIGURE_ERROR_NONE;
}

int32_t Config::SetConfig(const SharingData::Ptr &datas)
{
    SHARING_LOGD("trace.");
    datas_ = datas;
    SaveConfig();
    return CONFIGURE_ERROR_NONE;
}

int32_t Config::SetConfig(const std::string &module, const SharingDataGroupByModule::Ptr &values)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lk(mutex_);
    RETURN_INVALID_IF_NULL(datas_);
    auto err = datas_->PutSharingValues(values, module);
    if (err == CONFIGURE_ERROR_NONE) {
        SaveConfig();
    }

    return err;
}

int32_t Config::SetConfig(const std::string &module, const std::string &tag, const SharingDataGroupByTag::Ptr &values)
{
    SHARING_LOGD("trace.");
    SharingDataGroupByModule::Ptr modelValue = nullptr;
    if (GetConfig(module, modelValue) != CONFIGURE_ERROR_NONE) {
        modelValue = std::make_shared<SharingDataGroupByModule>(module);
    }

    if (modelValue == nullptr) {
        SHARING_LOGE("modelValue value is null.");
        return CONFIGURE_ERROR_NOT_FIND;
    }

    std::unique_lock<std::shared_mutex> lk(mutex_);
    auto err = modelValue->PutSharingValues(tag, values);
    if (err == CONFIGURE_ERROR_NONE) {
        SaveConfig();
    }

    return err;
}

int32_t Config::SetConfig(const std::string &module, const std::string &tag, const std::string &key,
                          const SharingValue::Ptr &value)
{
    SHARING_LOGD("trace.");
    if (!datas_->HasModule(module)) {
        auto modelValue = std::make_shared<SharingDataGroupByModule>(module);
        modelValue->PutSharingValue(tag, key, value);
        return SetConfig(module, modelValue);
    }

    if (!datas_->HasTag(module, tag)) {
        auto tagValue = std::make_shared<SharingDataGroupByTag>(tag);
        tagValue->PutSharingValue(key, value);
        return SetConfig(module, tag, tagValue);
    }

    datas_->PutSharingValue(key, value, module, tag);
    SaveConfig();
    return CONFIGURE_ERROR_NONE;
}

bool Config::ReadConfig(void)
{
    SHARING_LOGD("trace.");
    std::thread read([this] {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        status_ = ConfigStatus::CONFIG_STATUS_READING;
        JsonParser parser;
        parser.GetConfig(fileName_, datas_);
        status_ = ConfigStatus::CONFIG_STATUS_READY;
        EmitEvent();
    });

    if (read.joinable()) {
        read.join();
    }

    return true;
}

bool Config::SaveConfig(void)
{
    SHARING_LOGD("trace.");
    std::thread read([this] {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        status_ = ConfigStatus::CONFIG_STATUS_WRITING;
        JsonParser parser;
        std::string ofile = "/data/sharing_config.json";
        parser.SaveConfig(ofile, datas_);
        status_ = ConfigStatus::CONFIG_STATUS_READY;
    });

    read.detach();
    return true;
}

void Config::EmitEvent(const ConfigStatus type, const ModuleType toModule)
{
    SHARING_LOGD("trace.");
    SharingEvent evt;
    evt.eventMsg = std::make_shared<ConfigEventMsg>(type, toModule);
    evt.eventMsg->type = EventType::EVENT_CONFIGURE_READY;
    emiter_.SendEvent(evt);
}

void Config::EmitEvent(const EventType type, const ModuleType toModule, const SharingDataGroupByModule::Ptr &data)
{
    SHARING_LOGD("trace.");
    SharingEvent evt;
    evt.eventMsg = std::make_shared<ConfigEventMsg>(data, toModule);
    evt.eventMsg->type = type;
    emiter_.SendEvent(evt);
}

void Config::Init(void)
{
    SHARING_LOGD("trace.");
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (status_ != ConfigStatus::CONFIG_STATUS_INVALID) {
            SHARING_LOGE("init CONFIG_STATUS_INVALID.");
            return;
        }
        status_ = ConfigStatus::CONFIG_STATUS_INITED;
    }

    ReadConfig();
}

} // namespace Sharing
} // namespace OHOS