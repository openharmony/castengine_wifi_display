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

#include "scene_format.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {

bool SceneFormat::PutBoolValue(const std::string_view &key, bool value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_BOOL;
    data.val.boolVal = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutUint16Value(const std::string_view &key, uint16_t value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_UINT16;
    data.val.uint16Val = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutIntValue(const std::string_view &key, int32_t value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_INT32;
    data.val.int32Val = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutUint32Value(const std::string_view &key, uint32_t value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_UINT32;
    data.val.uint32Val = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutInt64Value(const std::string_view &key, int64_t value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_INT64;
    data.val.int64Val = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutUint64Value(const std::string_view &key, uint64_t value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_UINT64;
    data.val.uint64Val = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutFloatValue(const std::string_view &key, float value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_FLOAT;
    data.val.floatVal = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutDoubleValue(const std::string_view &key, double value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_DOUBLE;
    data.val.doubleVal = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutStringValue(const std::string_view &key, const std::string_view &value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_STRING;
    data.stringVal = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::PutObjectValue(const std::string_view &key, sptr<IRemoteObject> value)
{
    SHARING_LOGD("trace.");
    FormatData data;
    data.type = FORMAT_TYPE_OBJECT;
    data.objectVal = value;
    auto ret = formatMap_.insert(std::make_pair(key, data));

    return ret.second;
}

bool SceneFormat::GetBoolValue(const std::string_view &key, bool &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_BOOL) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.boolVal;

    return true;
}

bool SceneFormat::GetUint16Value(const std::string_view &key, uint16_t &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_UINT16) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.uint16Val;
    return true;
}

bool SceneFormat::GetIntValue(const std::string_view &key, int32_t &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_INT32) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.int32Val;
    return true;
}

bool SceneFormat::GetUint32Value(const std::string_view &key, uint32_t &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_UINT32) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.uint32Val;
    return true;
}

bool SceneFormat::GetInt64Value(const std::string_view &key, int64_t &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_INT64) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.int64Val;
    return true;
}

bool SceneFormat::GetUint64Value(const std::string_view &key, uint64_t &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_UINT64) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.uint64Val;
    return true;
}

bool SceneFormat::GetFloatValue(const std::string_view &key, float &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_FLOAT) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.floatVal;
    return true;
}

bool SceneFormat::GetDoubleValue(const std::string_view &key, double &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_DOUBLE) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.val.doubleVal;
    return true;
}

bool SceneFormat::GetStringValue(const std::string_view &key, std::string &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_STRING) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.stringVal;
    return true;
}

bool SceneFormat::GetObjectValue(const std::string_view &key, sptr<IRemoteObject> &value) const
{
    SHARING_LOGD("trace.");
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second.type != FORMAT_TYPE_OBJECT) {
        SHARING_LOGE("GetFormat failed. Key: %{public}s.", key.data());
        return false;
    }

    value = iter->second.objectVal;
    return true;
}

bool SceneFormatParcel::Marshalling(MessageParcel &parcel, const SceneFormat &format)
{
    SHARING_LOGD("trace.");
    auto dataMap = format.GetFormatMap();
    (void)parcel.WriteUint32(dataMap.size());

    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        (void)parcel.WriteString(it->first);
        (void)parcel.WriteUint32(it->second.type);
        switch (it->second.type) {
            case FORMAT_TYPE_UINT16:
                (void)parcel.WriteUint16(it->second.val.uint16Val);
                break;
            case FORMAT_TYPE_BOOL:
                (void)parcel.WriteBool(it->second.val.boolVal);
                break;
            case FORMAT_TYPE_INT32:
                (void)parcel.WriteInt32(it->second.val.int32Val);
                break;
            case FORMAT_TYPE_UINT32:
                (void)parcel.WriteUint32(it->second.val.uint32Val);
                break;
            case FORMAT_TYPE_INT64:
                (void)parcel.WriteInt64(it->second.val.int64Val);
                break;
            case FORMAT_TYPE_UINT64:
                (void)parcel.WriteInt64(it->second.val.uint64Val);
                break;
            case FORMAT_TYPE_FLOAT:
                (void)parcel.WriteFloat(it->second.val.floatVal);
                break;
            case FORMAT_TYPE_DOUBLE:
                (void)parcel.WriteDouble(it->second.val.doubleVal);
                break;
            case FORMAT_TYPE_STRING:
                (void)parcel.WriteString(it->second.stringVal);
                break;
            case FORMAT_TYPE_OBJECT:
                (void)parcel.WriteRemoteObject(it->second.objectVal);
                break;
            default:
                SHARING_LOGE("fail to Marshalling Key: %{public}s.", it->first.c_str());
                return false;
        }
        SHARING_LOGD("success to Marshalling Key: %{public}s.", it->first.c_str());
    }

    return true;
}

bool SceneFormatParcel::Unmarshalling(MessageParcel &parcel, SceneFormat &format)
{
    SHARING_LOGD("trace.");
    uint32_t size = parcel.ReadUint32();
    for (uint32_t index = 0; index < size; index++) {
        std::string key = parcel.ReadString();
        uint32_t valType = parcel.ReadUint32();
        switch (valType) {
            case FORMAT_TYPE_UINT16:
                (void)format.PutUint16Value(key, parcel.ReadUint16());
                break;
            case FORMAT_TYPE_BOOL:
                (void)format.PutBoolValue(key, parcel.ReadBool());
                break;
            case FORMAT_TYPE_INT32:
                (void)format.PutIntValue(key, parcel.ReadInt32());
                break;
            case FORMAT_TYPE_UINT32:
                (void)format.PutUint32Value(key, parcel.ReadUint32());
                break;
            case FORMAT_TYPE_INT64:
                (void)format.PutInt64Value(key, parcel.ReadInt64());
                break;
            case FORMAT_TYPE_UINT64:
                (void)format.PutUint64Value(key, parcel.ReadUint64());
                break;
            case FORMAT_TYPE_FLOAT:
                (void)format.PutFloatValue(key, parcel.ReadFloat());
                break;
            case FORMAT_TYPE_DOUBLE:
                (void)format.PutDoubleValue(key, parcel.ReadDouble());
                break;
            case FORMAT_TYPE_STRING:
                (void)format.PutStringValue(key, parcel.ReadString());
                break;
            case FORMAT_TYPE_OBJECT:
                (void)format.PutObjectValue(key, parcel.ReadRemoteObject());
                break;
            default:
                SHARING_LOGE("fail to Unmarshalling Key: %{public}s.", key.c_str());
                return false;
        }
        SHARING_LOGD("success to Unmarshalling Key: %{public}s.", key.c_str());
    }

    return true;
}

} // namespace Sharing
} // namespace OHOS
