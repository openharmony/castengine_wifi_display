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

#ifndef OHOS_SHARING_HISYSEVENT_H
#define OHOS_SHARING_HISYSEVENT_H

#include <string>
#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {
const std::string SHARING_INIT = "SHARING_INIT";
const std::string SHARING_OPERATION_FAIL = "SHARING_OPERATION_FAIL";
const std::string SHARING_FORWARD_EVENT = "SHARING_FORWARD_EVENT";

enum HisyseventErrorCode {
    ERR = -1,
    SUC
};

enum FrameworkModules {
    INTERACTIONMANAGER,
    INTERACTION,
    AGENT,
    SCENE,
    EVENT_MEDIA_CHANNEL
};

std::string CreateMsg(const char *format, ...) __attribute__((__format__(printf, 1, 2)));

void ReportSaEvent(int32_t saId, const std::string &errMsg);
void ReportSaFail(int32_t errCode, int32_t saId, const std::string &errMsg);
void ReportForwardSharingEvent(const std::string &sharingEvent, const std::string &errMsg);
void ReportOptFail(int32_t errCode, const std::string &operation, const std::string &errMsg);

} // namespace Sharing
} // namespace OHOS
#endif