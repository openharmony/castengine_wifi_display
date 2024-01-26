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

#include "sharing_log.h"
#include "sharing_hisysevent.h"
#include "hisysevent.h"
#include "securec.h"

constexpr uint32_t MAX_STRING_SIZE = 256;
static constexpr char SHARING[] = "SHARING";

namespace OHOS {
namespace Sharing {

void ReportOptFailReportOptFail(int32_t errCode, const std::string &operation, const std::string &errMsg)
{
    HiSysEventWrite(
        SHARING,
        SHARING_OPERATION_FAIL,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERRCODE", errCode,
        "OPERATION", operation,
        "MSG", errMsg);
}

void ReportSaFail(int32_t errCode, int32_t saId, const std::string &errMsg)
{
    HiSysEventWrite(
        SHARING,
        SHARING_INIT,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERRCODE", errCode,
        "SAID", saId,
        "MSG", errMsg);
}

void ReportSaEvent(int32_t saId, const std::string &errMsg)
{
    HiSysEventWrite(
        SHARING,
        SHARING_INIT,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SAID", saId,
        "MSG", errMsg);
}

void ReportForwardSharingEvent(const std::string &sharingEvent, const std::string &errMsg)
{
    HiSysEventWrite(
        SHARING,
        SHARING_FORWARD_EVENT,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "EVENT", sharingEvent,
        "MSG", errMsg);
}

std::string CreateMsg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char msg[MAX_STRING_SIZE] = {0};
    if (vsnprintf_s(msg, sizeof(msg), sizeof(msg) - 1, format, args) < 0) {
        SHARING_LOGE("failed to call vsnprintf_s.");
        va_end(args);
    }

    va_end(args);
    return std::string(msg);
}

} // namespace Sharing
} // namespace OHOS
