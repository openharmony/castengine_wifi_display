/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "wfd_discover_fuzzer.h"
#include "wfd_source.h"

namespace OHOS {
namespace Sharing {
    bool StartAndStopDiscover()
    {
        std::shared_ptr<WfdSource> client = nullptr;
        client = WfdSourceFactory::CreateSource(0, "wfd_discover_fuzzer");
        if (!client) {
            SHARING_LOGE("create wfdSource client error.");
            return false;
        }

        if (client->StartDiscover() != 0) {
            SHARING_LOGE("Source start error.");
            return false;
        }

        if (client->StopDiscover() != 0) {
            SHARING_LOGE("Source stop error.");
            return false;
        }
        return true;
    }
} // namespace sharing
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput()
{
    OHOS::Sharing::StartAndStopDiscover();
    return 0;
}
