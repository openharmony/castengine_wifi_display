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

#ifndef OHOS_SHARING_MEDIA_LOG_H
#define OHOS_SHARING_MEDIA_LOG_H

#include <cinttypes>
#include "sharing_log.h"
extern bool g_logOn;
namespace OHOS {
namespace Sharing {

#define MEDIA_LOGD(fmt, ...)                  \
    do {                                      \
        if (g_logOn) {                        \
            SHARING_LOGD(fmt, ##__VA_ARGS__); \
        }                                     \
    } while (0)

#define MEDIA_LOGI(fmt, ...)                  \
    do {                                      \
        if (g_logOn) {                        \
            SHARING_LOGI(fmt, ##__VA_ARGS__); \
        }                                     \
    } while (0)

#define MEDIA_LOGW(fmt, ...)                  \
    do {                                      \
        if (g_logOn) {                        \
            SHARING_LOGW(fmt, ##__VA_ARGS__); \
        }                                     \
    } while (0)

#define MEDIA_LOGE(fmt, ...)                  \
    do {                                      \
        if (1) {                              \
            SHARING_LOGE(fmt, ##__VA_ARGS__); \
        }                                     \
    } while (0)

#define MEDIA_LOGF(fmt, ...)                  \
    do {                                      \
        if (g_logOn) {                        \
            SHARING_LOGF(fmt, ##__VA_ARGS__); \
        }                                     \
    } while (0)

} // namespace Sharing
} // namespace OHOS
#endif