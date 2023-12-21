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

#include "sharing_data.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
bool SharingValue::IsInt32()
{
    SHARING_LOGD("trace.");
    if (data_.index() == static_cast<std::size_t>(SharingIndex::VALUE_INDEX_INT32)) {
        return true;
    }

    return false;
}

bool SharingValue::IsBool()
{
    SHARING_LOGD("trace.");
    if (data_.index() == static_cast<std::size_t>(SharingIndex::VALUE_INDEX_BOOL)) {
        return true;
    }

    return false;
}

bool SharingValue::IsString()
{
    SHARING_LOGD("trace.");
    if (data_.index() == static_cast<std::size_t>(SharingIndex::VALUE_INDEX_STRING)) {
        return true;
    }

    return false;
}

bool SharingValue::IsVector()
{
    SHARING_LOGD("trace.");
    if (data_.index() == static_cast<std::size_t>(SharingIndex::VALUE_INDEX_VECTOR)) {
        return true;
    }

    return false;
}

void SharingValue::Print()
{
    SHARING_LOGD("trace.");
    if (IsInt32()) {
        int32_t value = 0;
        GetValue(value);
        SHARING_LOGD("print %{public}d.", value);
    } else if (IsBool()) {
        bool value = 0;
        GetValue(value);
        SHARING_LOGD("print %{public}d.", value);
    } else if (IsString()) {
        std::string value("");
        GetValue(value);
        SHARING_LOGD("print %{public}s.", value.c_str());
    } else if (IsVector()) {
        std::vector<int32_t> value{};
        GetValue(value);
        for (auto &item : value) {
            SHARING_LOGD("print %{public}d.", item);
        }
    }
}

int32_t SharingDataGroupByTag::PutSharingValue(const std::string &key, const SharingValue::Ptr value)
{
    SHARING_LOGD("trace.");
    auto iter = datas_.find(key);
    if (iter == datas_.end()) {
        datas_.emplace(key, value);
    } else {
        iter->second = value;
    }

    return CONFIGURE_ERROR_NONE;
}

int32_t SharingDataGroupByTag::PutSharingValues(const std::unordered_map<std::string, SharingValue::Ptr> &values)
{
    SHARING_LOGD("trace.");
    datas_ = values;
    return CONFIGURE_ERROR_NONE;
}

SharingValue::Ptr SharingDataGroupByTag::GetSharingValue(const std::string &key)
{
    SHARING_LOGD("trace.");
    SharingValue::Ptr value = nullptr;
    auto iter = datas_.find(key);
    if (iter != datas_.end()) {
        value = iter->second;
    }

    return value;
}

int32_t SharingDataGroupByTag::GetSharingValues(std::unordered_map<std::string, SharingValue::Ptr> &values)
{
    SHARING_LOGD("trace.");
    values = datas_;
    return CONFIGURE_ERROR_NONE;
}

bool SharingDataGroupByTag::HasKey(const std::string &key)
{
    SHARING_LOGD("trace.");
    return datas_.find(key) != datas_.end();
}

void SharingDataGroupByTag::ForEach(Each each)
{
    SHARING_LOGD("trace.");
    auto iter = datas_.begin();
    while (iter != datas_.end()) {
        std::string key = iter->first;
        each(key, iter->second);
        iter++;
    }
}

void SharingDataGroupByTag::Print()
{
    SHARING_LOGD("trace.");
    auto iter = datas_.begin();
    while (iter != datas_.end()) {
        std::string key = iter->first;
        SharingValue::Ptr value = iter->second;
        value->Print();
        iter++;
    }
}

int32_t SharingDataGroupByModule::PutSharingValue(const std::string &tag, const std::string &key,
                                                  const SharingValue::Ptr value)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        auto grpData = std::make_shared<SharingDataGroupByTag>(tag);
        if (grpData == nullptr) {
            SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
            return CONFIGURE_ERROR_NOT_FIND;
        }
        grpData->PutSharingValue(key, value);
        datass_.emplace(std::make_pair(tag, grpData));
        return CONFIGURE_ERROR_NONE;
    }

    return iter->second->PutSharingValue(key, value);
}

int32_t SharingDataGroupByModule::PutSharingValues(const std::string &tag,
                                                   const std::unordered_map<std::string, SharingValue::Ptr> &values)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        auto grpData = std::make_shared<SharingDataGroupByTag>(tag);
        if (grpData == nullptr) {
            SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
            return CONFIGURE_ERROR_NOT_FIND;
        }
        grpData->PutSharingValues(values);
        datass_.emplace(std::make_pair(tag, grpData));
        return CONFIGURE_ERROR_NONE;
    }

    return iter->second->PutSharingValues(values);
}

int32_t SharingDataGroupByModule::PutSharingValues(const std::string &tag, const SharingDataGroupByTag::Ptr &value)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        datass_.emplace(std::make_pair(tag, value));
        return CONFIGURE_ERROR_NONE;
    }

    iter->second = value;
    return CONFIGURE_ERROR_NONE;
}

SharingValue::Ptr SharingDataGroupByModule::GetSharingValue(const std::string &tag, const std::string &key)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        SHARING_LOGE("nullptr.");
        return nullptr;
    }

    return iter->second->GetSharingValue(key);
}

int32_t SharingDataGroupByModule::GetSharingValues(const std::string &tag,
                                                   std::unordered_map<std::string, SharingValue::Ptr> &values)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
        return CONFIGURE_ERROR_NOT_FIND;
    }

    return iter->second->GetSharingValues(values);
}

int32_t SharingDataGroupByModule::GetSharingValues(const std::string &tag, SharingDataGroupByTag::Ptr &value)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
        return CONFIGURE_ERROR_NOT_FIND;
    }

    value = iter->second;
    return CONFIGURE_ERROR_NONE;
}

bool SharingDataGroupByModule::HasKey(const std::string &tag, const std::string &key)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(tag);
    if (iter == datass_.end()) {
        return false;
    }

    return iter->second->HasKey(key);
}

bool SharingDataGroupByModule::HasTag(const std::string &tag)
{
    SHARING_LOGD("trace.");
    return datass_.find(tag) != datass_.end();
}

void SharingDataGroupByModule::ForEach(Each each)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.begin();
    while (iter != datass_.end()) {
        std::string key = iter->first;
        each(key, iter->second);
        iter++;
    }
}

void SharingDataGroupByModule::Print()
{
    SHARING_LOGD("trace.");
    auto iter = datass_.begin();
    while (iter != datass_.end()) {
        SHARING_LOGD("forEach print %{public}s.", iter->first.c_str());
        SharingDataGroupByTag::Ptr value = iter->second;
        value->Print();
        iter++;
    }
}

int32_t SharingData::PutSharingValue(const std::string &key, const SharingValue::Ptr value, const std::string &module,
                                     const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        auto moduleData = std::make_shared<SharingDataGroupByModule>(module);
        if (moduleData == nullptr) {
            SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
            return CONFIGURE_ERROR_NOT_FIND;
        }
        moduleData->PutSharingValue(tag, key, value);
        datass_.emplace(std::make_pair(module, moduleData));
        return CONFIGURE_ERROR_NONE;
    }

    return iter->second->PutSharingValue(tag, key, value);
}
int32_t SharingData::PutSharingValues(const SharingDataGroupByModule::Ptr &values, const std::string &module)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        datass_.emplace(std::make_pair(module, values));
        return CONFIGURE_ERROR_NONE;
    }

    iter->second = values;
    return CONFIGURE_ERROR_NONE;
}

int32_t SharingData::PutSharingValues(const std::unordered_map<std::string, SharingValue::Ptr> &values,
                                      const std::string &module, const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        auto moduleData = std::make_shared<SharingDataGroupByModule>(module);
        if (moduleData == nullptr) {
            SHARING_LOGE("CONFIGURE_ERROR_NOT_FIND.");
            return CONFIGURE_ERROR_NOT_FIND;
        }
        moduleData->PutSharingValues(tag, values);
        datass_.emplace(std::make_pair(module, moduleData));
        return CONFIGURE_ERROR_NONE;
    }

    return iter->second->PutSharingValues(tag, values);
}

SharingValue::Ptr SharingData::GetSharingValue(const std::string &key, const std::string &module,
                                               const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return nullptr;
    }

    return iter->second->GetSharingValue(tag, key);
}

int32_t SharingData::GetSharingValues(SharingDataGroupByModule::Ptr &values, const std::string &module)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return CONFIGURE_ERROR_NOT_FIND;
    }

    values = iter->second;
    return CONFIGURE_ERROR_NONE;
}

int32_t SharingData::GetSharingValues(std::unordered_map<std::string, SharingValue::Ptr> &values,
                                      const std::string &module, const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return CONFIGURE_ERROR_NOT_FIND;
    }

    return iter->second->GetSharingValues(tag, values);
}

int32_t SharingData::GetSharingValues(SharingDataGroupByTag::Ptr &value, const std::string &module,
                                      const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return CONFIGURE_ERROR_NOT_FIND;
    }

    return iter->second->GetSharingValues(tag, value);
}

bool SharingData::HasKey(const std::string &key, const std::string &module, const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return false;
    }

    return iter->second->HasKey(tag, key);
}

bool SharingData::HasModule(const std::string &module)
{
    SHARING_LOGD("trace.");
    return datass_.find(module) != datass_.end();
}

bool SharingData::HasTag(const std::string &module, const std::string &tag)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.find(module);
    if (iter == datass_.end()) {
        return false;
    }

    return iter->second->HasTag(tag);
}

void SharingData::ForEach(Each each)
{
    SHARING_LOGD("trace.");
    auto iter = datass_.begin();
    while (iter != datass_.end()) {
        std::string key = iter->first;
        SHARING_LOGD("forEach %{public}s.", key.c_str());
        each(key, iter->second);
        iter++;
    }
}

void SharingData::Print()
{
    SHARING_LOGD("trace.");
    auto iter = datass_.begin();
    while (iter != datass_.end()) {
        SHARING_LOGD("forEach print %{public}s.", iter->first.c_str());
        SharingDataGroupByModule::Ptr value = iter->second;
        value->Print();
        iter++;
    }
}

} // namespace Sharing
} // namespace OHOS