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

#include "reflect_registration.h"
#include "media_log.h"
bool g_logOn = false;
namespace OHOS {
namespace Sharing {

std::shared_ptr<void> ReflectRegistration::CreateObject(const std::string &descriptor)
{
    std::lock_guard<std::mutex> lockGuard(creatorMutex_);
    if (creators_.find(descriptor) == creators_.end()) {
        return nullptr;
    }

    return creators_[descriptor]();
}

bool ReflectRegistration::Register(const std::string &descriptor, Constructor creator)
{
    if (descriptor.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lockGuard(creatorMutex_);
    auto it = creators_.find(descriptor);
    if (it == creators_.end()) {
        return creators_.insert({descriptor, creator}).second;
    }

    return false;
}

void ReflectRegistration::Unregister(const std::string &descriptor)
{
    std::lock_guard<std::mutex> lockGuard(creatorMutex_);
    if (!descriptor.empty()) {
        auto it = creators_.find(descriptor);
        if (it != creators_.end()) {
            creators_.erase(it);
        }
    }
}

ReflectRegistration::~ReflectRegistration()
{
    std::lock_guard<std::mutex> lockGuard(creatorMutex_);
    for (auto it = creators_.begin(); it != creators_.end();) {
        it = creators_.erase(it);
    }
}

} // namespace Sharing
} // namespace OHOS