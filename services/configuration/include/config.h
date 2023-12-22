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

#ifndef OHOS_SHARING_CONFIG_H
#define OHOS_SHARING_CONFIG_H

#include <shared_mutex>
#include <singleton.h>
#include <string>
#include "event/event_base.h"
#include "sharing_data.h"

namespace OHOS {
namespace Sharing {

enum class ConfigStatus {
    CONFIG_STATUS_INVALID,
    CONFIG_STATUS_INITED,
    CONFIG_STATUS_READING,
    CONFIG_STATUS_WRITING,
    CONFIG_STATUS_READY
};

struct ConfigEventMsg : EventMsg {
    ConfigEventMsg(const ConfigStatus status, const ModuleType toMudule)
    {
        this->status = status;
        this->toMgr = toMudule;
        this->fromMgr = ModuleType::MODULE_CONFIGURE;
        this->data = nullptr;
    }

    ConfigEventMsg(const SharingDataGroupByModule::Ptr &data, const ModuleType toMudule)
    {
        this->status = ConfigStatus::CONFIG_STATUS_READY;
        this->data = data;
        this->fromMgr = ModuleType::MODULE_CONFIGURE;
        this->toMgr = toMudule;
    }

    ConfigStatus status;
    SharingDataGroupByModule::Ptr data;
};

class Config : public Singleton<Config> {
public:
    Config() = default;
    ~Config() = default;

    void Init(void);
    int32_t GetConfig(SharingData::Ptr &datas);
    int32_t GetConfig(const std::string &module, SharingDataGroupByModule::Ptr &values);
    int32_t GetConfig(const std::string &module, const std::string &tag, SharingDataGroupByTag::Ptr &values);
    int32_t GetConfig(const std::string &module, const std::string &tag, const std::string &key,
                      SharingValue::Ptr &value);

    int32_t SetConfig(const SharingData::Ptr &datas);
    int32_t SetConfig(const std::string &module, const SharingDataGroupByModule::Ptr &values);
    int32_t SetConfig(const std::string &module, const std::string &tag, const SharingDataGroupByTag::Ptr &values);
    int32_t SetConfig(const std::string &module, const std::string &tag, const std::string &key,
                      const SharingValue::Ptr &value);

private:
    bool ReadConfig(void);
    bool SaveConfig(void);
    void EmitEvent(const EventType type, const ModuleType toModule, const SharingDataGroupByModule::Ptr &data);
    void EmitEvent(const ConfigStatus type = ConfigStatus::CONFIG_STATUS_READY,
                   const ModuleType toModule = ModuleType::MODULE_DATACENTER);

private:
    std::shared_mutex mutex_;
    std::string fileName_ = "/etc/sharing_config.json";

    EventEmitter emiter_;
    SharingData::Ptr datas_ = std::make_shared<SharingData>();
    ConfigStatus status_ = ConfigStatus::CONFIG_STATUS_INVALID;
};

} // namespace Sharing
} // namespace OHOS
#endif