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

#ifndef OHOS_SHARING_DATA_H
#define OHOS_SHARING_DATA_H

#include <inttypes.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace OHOS {
namespace Sharing {

enum ConfigureError { CONFIGURE_ERROR_NONE, CONFIGURE_ERROR_NOT_FIND };

class SharingValue {
public:
    using Ptr = std::shared_ptr<SharingValue>;

    SharingValue(bool data) : data_(data) {}
    SharingValue(int32_t data) : data_(data) {}
    SharingValue(std::string &data) : data_(data) {}
    SharingValue(std::vector<int32_t> &data) : data_(data) {}
    ~SharingValue() = default;

    template <class T>
    bool GetValue(T &value)
    {
        value = std::get<T>(data_);
        return true;
    }

    template <class T>
    void SetValue(T &value)
    {
        data_ = value;
    }

    enum class SharingIndex : uint32_t {
        VALUE_INDEX_INT32 = 0,
        VALUE_INDEX_BOOL,
        VALUE_INDEX_STRING,
        VALUE_INDEX_VECTOR
    };

public:
    void Print();

    bool IsBool();
    bool IsInt32();
    bool IsString();
    bool IsVector();

private:
    std::variant<int32_t, bool, std::string, std::vector<int32_t>> data_;
};

class SharingDataGroupByTag {
public:
    using Ptr = std::shared_ptr<SharingDataGroupByTag>;
    using Each = std::function<void(const std::string &key, const SharingValue::Ptr &value)>;

    explicit SharingDataGroupByTag(const std::string &tag) : tag_(tag) {}

    bool IsTag(const std::string &tag)
    {
        return tag_ == tag;
    }

public:
    int32_t PutSharingValue(const std::string &key, const SharingValue::Ptr value);
    int32_t PutSharingValues(const std::unordered_map<std::string, SharingValue::Ptr> &values);

    int32_t GetSharingValues(std::unordered_map<std::string, SharingValue::Ptr> &values);
    SharingValue::Ptr GetSharingValue(const std::string &key);

    void Print();
    void ForEach(Each each);
    bool HasKey(const std::string &key);

private:
    std::string tag_;
    std::unordered_map<std::string, SharingValue::Ptr> datas_;
};

class SharingDataGroupByModule {
public:
    using Ptr = std::shared_ptr<SharingDataGroupByModule>;
    using Each = std::function<void(const std::string &tag, const SharingDataGroupByTag::Ptr &value)>;

    explicit SharingDataGroupByModule(const std::string &mudule) : mudule_(mudule) {}

    bool IsModule(const std::string &mudule)
    {
        return mudule_ == mudule;
    }

public:
    int32_t PutSharingValues(const std::string &tag, const SharingDataGroupByTag::Ptr &value);
    int32_t PutSharingValue(const std::string &tag, const std::string &key, const SharingValue::Ptr value);
    int32_t PutSharingValues(const std::string &tag, const std::unordered_map<std::string, SharingValue::Ptr> &values);

    int32_t GetSharingValues(const std::string &tag, SharingDataGroupByTag::Ptr &value);
    int32_t GetSharingValues(const std::string &tag, std::unordered_map<std::string, SharingValue::Ptr> &values);
    SharingValue::Ptr GetSharingValue(const std::string &tag, const std::string &key);

    void Print();
    void ForEach(Each each);
    bool HasTag(const std::string &tag);
    bool HasKey(const std::string &tag, const std::string &key);

private:
    std::string mudule_;
    std::unordered_map<std::string, SharingDataGroupByTag::Ptr> datass_;
};

class SharingData {
public:
    using Ptr = std::shared_ptr<SharingData>;
    using Each = std::function<void(const std::string &module, const SharingDataGroupByModule::Ptr &value)>;

    SharingData() = default;
    ~SharingData() = default;

    int32_t PutSharingValues(const SharingDataGroupByModule::Ptr &values, const std::string &module);
    int32_t PutSharingValue(const std::string &key, const SharingValue::Ptr value, const std::string &module,
                            const std::string &tag);
    int32_t PutSharingValues(const std::unordered_map<std::string, SharingValue::Ptr> &values,
                             const std::string &module, const std::string &tag);

    int32_t GetSharingValues(SharingDataGroupByModule::Ptr &values, const std::string &module);
    int32_t GetSharingValues(SharingDataGroupByTag::Ptr &value, const std::string &module, const std::string &tag);
    int32_t GetSharingValues(std::unordered_map<std::string, SharingValue::Ptr> &values, const std::string &module,
                             const std::string &tag);
    SharingValue::Ptr GetSharingValue(const std::string &key, const std::string &module, const std::string &tag);

    void Print();
    void ForEach(Each each);
    bool HasModule(const std::string &module);
    bool HasTag(const std::string &module, const std::string &tag);
    bool HasKey(const std::string &key, const std::string &module, const std::string &tag);

private:
    std::unordered_map<std::string, SharingDataGroupByModule::Ptr> datass_;
};

} // namespace Sharing
} // namespace OHOS
#endif