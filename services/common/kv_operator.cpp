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

#include "kv_operator.h"
#include "directory_ex.h"
#include "media_log.h"

namespace {
    const OHOS::DistributedKv::AppId KVSTORE_APPID = {"com.ohos.sharing.codec"};
    const OHOS::DistributedKv::StoreId KVSTORE_STOREID = {"media_sharingcodec"};
}

static constexpr int32_t FILE_PERMISSION = 0777;

namespace OHOS {
namespace Sharing {

void KvOperator::OpenKvStore()
{
    DistributedKv::Options options = {
        .createIfMissing = true,
        .encrypt = false,
        .autoSync = false,
        .area = DistributedKv::Area::EL1,
        .kvStoreType = DistributedKv::KvStoreType::SINGLE_VERSION,
        .baseDir = "/data/service/el1/public/database/sharingcodec"
    };

    DistributedKv::AppId appId = { KVSTORE_APPID };
    DistributedKv::StoreId storeId = { KVSTORE_STOREID };
    mkdir(options.baseDir.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
    DistributedKv::Status status = dataManager_.GetSingleKvStore(options, appId, storeId, kvStorePtr_);
    if (status != DistributedKv::Status::SUCCESS) {
        MEDIA_LOGE("KvStore get failed! %{public}d.", status);
        return;
    }

    OHOS::ChangeModeDirectory(options.baseDir, FILE_PERMISSION);
    MEDIA_LOGI("KvStore using for %{public}s init success!", KVSTORE_APPID.appId.c_str());
}

void KvOperator::CloseKvStore()
{
    if (kvStorePtr_ != nullptr) {
        dataManager_.CloseKvStore({ KVSTORE_APPID }, kvStorePtr_);
        kvStorePtr_ = nullptr;
    }

    MEDIA_LOGI("Close KvStore for %{public}s init success!", KVSTORE_APPID.appId.c_str());
}

bool KvOperator::GetValues(const std::string &key,  std::string &val)
{
    if (kvStorePtr_ == nullptr) {
        MEDIA_LOGE("kvstore is nullptr.");
        return false;
    }

    DistributedKv::Key k(key);
    DistributedKv::Value v;
    DistributedKv::Status status = kvStorePtr_->Get(k, v);
    if (status != DistributedKv::Status::SUCCESS) {
        MEDIA_LOGE("get kvstore failed %{public}d.", status);
        val = "";
        return false;
    }

    val = v.ToString();

    MEDIA_LOGI("get kvstore success!  val %{public}s.", val.c_str());
    return true;
}

bool KvOperator::PutValues(const std::string &key, std::string values)
{
    if (kvStorePtr_ == nullptr) {
        MEDIA_LOGE("kvstore is nullptr.");
        return false;
    }

    DistributedKv::Key k(key);
    DistributedKv::Value v(values);
    DistributedKv::Status status = kvStorePtr_->Put(k, v);
    if (status != DistributedKv::Status::SUCCESS) {
        MEDIA_LOGE("put kvstore failed %{public}d.", status);
        return false;
    }
    MEDIA_LOGI("put kvstore success!, val %{public}s.", values.c_str());
    return true;
}


void KvOperator::SyncKvStore(const std::string &key, const std::string &deviceId)
{
    if (kvStorePtr_ == nullptr) {
        MEDIA_LOGE("kvstore is nullptr.");
        return;
    }

    DistributedKv::DataQuery dataQuery;
    dataQuery.KeyPrefix(key);
    std::vector<std::string> deviceIds = { deviceId };
    DistributedKv::Status status = kvStorePtr_->Sync(deviceIds, DistributedKv::SyncMode::PULL,
        dataQuery, shared_from_this());
    MEDIA_LOGE("kvstore sync end, status %{public}d.", status);
}

void KvOperator::SyncCompleted(const std::map<std::string, DistributedKv::Status> &results)
{
    MEDIA_LOGI("kvstore sync completed.");
}

bool KvOperator::StringToJson(const std::string &str, nlohmann::json &jsonObj)
{
    jsonObj = nlohmann::json::parse(str);
    if (jsonObj.is_discarded()) {
        MEDIA_LOGE("parse json failed.");
        return false;
    }

    return true;
}

bool KvOperator::JsonToString(const nlohmann::json &jsonObj, std::string &str)
{
    str = jsonObj.dump();
    return true;
}

bool KvOperator::QueryFromJson(const nlohmann::json &jsonObj, const std::string &key, std::string &val)
{
    val = jsonObj.at(key.c_str());
    return true;
}

} // namespace Media
} // namespace OHOS
