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

#include "wfd_sink_set_listener_fuzzer.h"

namespace OHOS {
namespace Sharing {
    void WfdListener::OnInfo(std::shared_ptr<BaseMsg> &msg) {}

    bool SetSinkListener()
    {
        std::shared_ptr<WfdSink> client = nullptr;
        std::shared_ptr<WfdListener> listener = std::make_shared<WfdListener>();

        client = WfdSinkFactory::CreateSink(0, "wfd_sink_set_listener_fuzzer");
        if (!client) {
            SHARING_LOGE("create wfdSink client error.");
            return false;
        }

        if (client->SetListener(listener) != 0) {
            SHARING_LOGE("set sink listener error.");
            return false;
        }
        return true;
    }
} // namespace sharing
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput()
{
    OHOS::Sharing::SetSinkListener();
    return 0;
}
