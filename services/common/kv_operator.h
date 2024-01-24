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

#ifndef DEVCIES_MEADIA_LIBRARY_INFO_INTERACTION_H
#define DEVCIES_MEADIA_LIBRARY_INFO_INTERACTION_H

#include "distributed_kv_data_manager.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Sharing {

constexpr char SHARING_INFOS[] = "sharingInfos";
constexpr char VALID_PID[] = "validPid";
constexpr char ENABLE_HARDWARE[] = "enableHardware";

class KvOperator final : public DistributedKv::KvStoreSyncCallback,
                                  public std::enable_shared_from_this<KvOperator> {
public:
    KvOperator() = default;
    KvOperator(const KvOperator&) = delete;
    KvOperator(const KvOperator&&) = delete;
    virtual ~KvOperator() = default;

    KvOperator& operator=(const KvOperator&) = delete;
    KvOperator& operator=(const KvOperator&&) = delete;

    void OpenKvStore();
    void CloseKvStore();
    void SyncKvStore(const std::string &key, const std::string &deviceId);
    void SyncCompleted(const std::map<std::string, DistributedKv::Status> &results) override;

    bool GetValues(const std::string &key,  std::string &val);
    bool PutValues(const std::string &key, std::string values);

    bool StringToJson(const std::string &str, nlohmann::json &jsonObj);
    bool JsonToString(const nlohmann::json &jsonObj, std::string &str);
    bool QueryFromJson(const nlohmann::json &jsonObj, const std::string &key, std::string &val);

private:
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_;

    DistributedKv::DistributedKvDataManager dataManager_;
};

} // namespace Media
} // namespace OHOS
#endif
