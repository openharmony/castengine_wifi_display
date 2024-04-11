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

#ifndef OHOS_SHARING_LOG_H
#define OHOS_SHARING_LOG_H

#include <cinttypes>
#include "hilog/log.h"

namespace OHOS {
namespace Sharing {
#ifndef SHARING_LOG_TAG
#define SHARING_LOG_TAG "sharing"
#endif

#ifndef SHARING_LOG_DOMAIN
#define SHARING_LOG_DOMAIN 0xD002B09
#define SHARING_SERVICE_TEMP_SA_ID  5527
#define SHARING_SERVICE_DOMAIN_TEMP_SA_ID   5528
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel SHARING_LABEL = {LOG_CORE, SHARING_LOG_DOMAIN, SHARING_LOG_TAG};

#define R_FILENAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define SHARING_LOG(func, fmt, args...)                                                                            \
    do {                                                                                                             \
        (void)func(SHARING_LABEL, "[Sharing][%{public}s()][%{public}s:%{public}d] " fmt, __FUNCTION__, R_FILENAME,   \
                   __LINE__, ##args);                                                                                \
    } while (0)

#define SHARING_LOGD(fmt, ...) SHARING_LOG(OHOS::HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
#define SHARING_LOGI(fmt, ...) SHARING_LOG(OHOS::HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define SHARING_LOGW(fmt, ...) SHARING_LOG(OHOS::HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define SHARING_LOGE(fmt, ...) SHARING_LOG(OHOS::HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define SHARING_LOGF(fmt, ...) SHARING_LOG(OHOS::HiviewDFX::HiLog::Fatal, fmt, ##__VA_ARGS__)

#define CHECK_AND_RETURN(cond)                                \
    do {                                                      \
        if (!(cond)) {                                        \
            SHARING_LOGE("%{public}s, check failed!", #cond); \
            return;                                           \
        }                                                     \
    } while (0)

#define CHECK_AND_RETURN_RET(cond, ret)                                              \
    do {                                                                             \
        if (!(cond)) {                                                               \
            SHARING_LOGE("%{public}s, check failed! ret: %{public}s.", #cond, #ret);  \
            return ret;                                                              \
        }                                                                            \
    } while (0)

#define CHECK_AND_RETURN_RET_LOG(cond, ret, fmt, ...) \
    do {                                              \
        if (!(cond)) {                                \
            SHARING_LOGE(fmt, ##__VA_ARGS__);         \
            return ret;                               \
        }                                             \
    } while (0)

#define CHECK_AND_RETURN_LOG(cond, fmt, ...)  \
    do {                                      \
        if (!(cond)) {                        \
            SHARING_LOGE(fmt, ##__VA_ARGS__); \
            return;                           \
        }                                     \
    } while (0)

} // namespace Sharing
} // namespace OHOS
#endif