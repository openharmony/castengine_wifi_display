/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "data_share_helper.h"
#include "common/sharing_log.h"
#include "iservice_registry.h"
#include "os_account_manager.h"

namespace OHOS {
namespace Sharing {
const std::string SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
const std::string SETTINGS_DATA_BASE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
const std::string SETTINGS_DATA_SECURE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_";
const std::string SETTINGS_GENERAL_DEVICE_NAME = "settings.general.device_name";
const std::string SETTINGS_DISPLAY_DEVICE_NAME = "settings.general.display_device_name";
constexpr int32_t DEFAULT_OS_ACCOUNT_ID = 100;

DataShareHelper &DataShareHelper::GetInstance()
{
    static DataShareHelper instance{};
    return instance;
}

DataShareHelper::~DataShareHelper()
{
    if (helper_ != nullptr) {
        helper_->Release();
        helper_ = nullptr;
    }
}

int32_t DataShareHelper::RegisterObserver(const sptr<AAFwk::IDataAbilityObserver> &observer)
{
    if (helper_ == nullptr && !Init()) {
        SHARING_LOGE("fail to register observer, helper is null");
        return DATA_SHARE_ERROR;
    }
    helper_->RegisterObserver(GetDefaultNamerUri(), observer);
    helper_->RegisterObserver(GetDisplayNameUri(), observer);
    return DATA_SHARE_SUCCESS;
}

int32_t DataShareHelper::UnregisterObserver(const sptr<AAFwk::IDataAbilityObserver> &observer)
{
    if (helper_ == nullptr && !Init()) {
        SHARING_LOGE("fail to unregister observer, helper is null");
        return DATA_SHARE_ERROR;
    }
    helper_->UnregisterObserver(GetDefaultNamerUri(), observer);
    helper_->UnregisterObserver(GetDisplayNameUri(), observer);
    return DATA_SHARE_SUCCESS;
}

bool DataShareHelper::Init()
{
    SHARING_LOGI("init enter");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        SHARING_LOGE("faile to init samgr is null");
        return false;
    }

    auto remoteObj = samgr->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    if (remoteObj == nullptr) {
        SHARING_LOGE("faile to init remoteObj is null");
        return false;
    }
    helper_ = DataShare::DataShareHelper::Creator(remoteObj, SETTINGS_DATA_BASE_URI, SETTINGS_DATA_EXT_URI);
    if (helper_ == nullptr) {
        SHARING_LOGE("faile to init helper_ is null");
        return false;
    }
    SHARING_LOGI("init end");
    return true;
}

Uri DataShareHelper::GetDefaultNamerUri()
{
    return Uri(SETTINGS_DATA_BASE_URI + "&key=" + SETTINGS_GENERAL_DEVICE_NAME);
}

Uri DataShareHelper::GetDisplayNameUri()
{
    std::string osAccountId = std::to_string(GetCurrentActiveAccountUserId());
    return Uri(SETTINGS_DATA_SECURE_URI + osAccountId + "?Proxy=true&key=" + SETTINGS_DISPLAY_DEVICE_NAME);
}

int32_t DataShareHelper::GetCurrentActiveAccountUserId()
{
    std::vector<int32_t> activatedOsAccountIds;
    ErrCode ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (ret != ERR_OK || activatedOsAccountIds.empty()) {
        SHARING_LOGE("faile to get active userId, ret=%{public}d", ret);
        return DEFAULT_OS_ACCOUNT_ID;
    }
    SHARING_LOGI("active user=%{public}d", activatedOsAccountIds[0]);
    return activatedOsAccountIds[0];
}
} // namespace Sharing
} // namespace OHOS