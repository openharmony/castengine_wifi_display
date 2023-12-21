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

#ifndef OHOS_INTER_IPC_FORMAT_H
#define OHOS_INTER_IPC_FORMAT_H

#include <map>
#include <string>
#include <vector>
#include "iremote_object.h"
#include "message_parcel.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {

enum FormatDataType : uint32_t {
    FORMAT_TYPE_NONE,
    FORMAT_TYPE_BOOL,
    FORMAT_TYPE_UINT16,
    FORMAT_TYPE_INT32,
    FORMAT_TYPE_UINT32,
    FORMAT_TYPE_INT64,
    FORMAT_TYPE_UINT64,
    FORMAT_TYPE_FLOAT,
    FORMAT_TYPE_DOUBLE,
    FORMAT_TYPE_STRING,
    FORMAT_TYPE_ADDR,
    FORMAT_TYPE_OBJECT
};

struct FormatData {
    union Val {
        bool boolVal;
        uint16_t uint16Val;
        int32_t int32Val;
        uint32_t uint32Val;
        int64_t int64Val;
        uint64_t uint64Val;
        float floatVal;
        double doubleVal;
    } val = {0};

    size_t size = 0;
    uint8_t *addr = nullptr;
    std::string stringVal = "";
    sptr<IRemoteObject> objectVal = nullptr;
    FormatDataType type = FORMAT_TYPE_NONE;
};

class SceneFormat {
public:
    using Ptr = std::shared_ptr<SceneFormat>;
    using FormatDataMap = std::map<std::string, FormatData, std::less<>>;

    const FormatDataMap &GetFormatMap() const
    {
        SHARING_LOGD("trace.");
        return formatMap_;
    }

public:
    bool PutBoolValue(const std::string_view &key, bool value);
    bool PutIntValue(const std::string_view &key, int32_t value);
    bool PutFloatValue(const std::string_view &key, float value);
    bool PutDoubleValue(const std::string_view &key, double value);
    bool PutInt64Value(const std::string_view &key, int64_t value);
    bool PutUint16Value(const std::string_view &key, uint16_t value);
    bool PutUint32Value(const std::string_view &key, uint32_t value);
    bool PutUint64Value(const std::string_view &key, uint64_t value);
    bool PutObjectValue(const std::string_view &key, sptr<IRemoteObject> value);
    bool PutStringValue(const std::string_view &key, const std::string_view &value);

    bool GetBoolValue(const std::string_view &key, bool &value) const;
    bool GetIntValue(const std::string_view &key, int32_t &value) const;
    bool GetFloatValue(const std::string_view &key, float &value) const;
    bool GetDoubleValue(const std::string_view &key, double &value) const;
    bool GetInt64Value(const std::string_view &key, int64_t &value) const;
    bool GetUint16Value(const std::string_view &key, uint16_t &value) const;
    bool GetUint32Value(const std::string_view &key, uint32_t &value) const;
    bool GetUint64Value(const std::string_view &key, uint64_t &value) const;
    bool GetStringValue(const std::string_view &key, std::string &value) const;
    bool GetObjectValue(const std::string_view &key, sptr<IRemoteObject> &value) const;

private:
    FormatDataMap formatMap_;
};

class SceneFormatParcel {
public:
    static bool Unmarshalling(MessageParcel &parcel, SceneFormat &format);
    static bool Marshalling(MessageParcel &parcel, const SceneFormat &format);
};

} // namespace Sharing
} // namespace OHOS
#endif