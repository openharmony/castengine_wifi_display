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

#include "add_mirror_token_fuzzer.h"
#include "wfd_start_fuzzer.h"
#include "wfd_sink.h"
#include "securec.h"

namespace OHOS {
namespace Sharing {
    bool CreateSinkFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size == 0) {
            return false;
        }
        int32_t type{};
        size_t objectSize = sizeof(type);
        if (objectSize > size) {
            return false;
        }
        errno_t ret = memcpy_s(&type, objectSize, data, objectSize);
        if (ret != EOK) {
            return false;
        }

        std::shared_ptr<WfdSink> client = nullptr;
        client = WfdSinkFactory::CreateSink(type, "wfd_start_fuzzer");
        if (!client) {
            SHARING_LOGE("create wfdSink client error.");
            return false;
        }

        if (client->Start() != 0) {
            SHARING_LOGE("Start error.");
            return false;
        }

        if (client->Stop() != 0) {
            SHARING_LOGE("Stop error.");
            return false;
        }

        return true;
    }
} // namespace sharing
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Sharing::AddMirrorTokenFuzzer::AddMirrorToken();
    OHOS::Sharing::CreateSinkFuzzTest(data, size);
    return 0;
}
