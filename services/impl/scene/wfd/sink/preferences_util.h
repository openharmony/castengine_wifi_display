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

#ifndef OHOS_SHARING_PREFERENCES_UTIL_H
#define OHOS_SHARING_PREFERENCES_UTIL_H

#include <shared_mutex>
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"

namespace OHOS {
namespace Sharing {
class PreferencesUtil {
public:
    explicit PreferencesUtil(const std::string &path);

    int PutString(const std::string &key, const std::string &valut);
    std::string GetString(const std::string &key);
    bool HasKey(const std::string &key);
    int DeleteKey(const std::string &key);
    int Clear();
    void Flush();
    int FlushSync();

private:
    std::shared_ptr<NativePreferences::Preferences> GetProfiles(const std::string &path, int &errCode);

private:
    std::shared_mutex preferencesMutex_;
    int errCode_ = NativePreferences::E_OK;
    std::string path_;
};
} // namespace Sharing
} // namespace OHOS
#endif