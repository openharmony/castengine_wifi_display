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

#ifndef OHOS_SHARING_IDENTIFIER_H
#define OHOS_SHARING_IDENTIFIER_H

#include <atomic>
#include <singleton.h>
#include <string>
namespace OHOS {
namespace Sharing {

class Identifier : public Singleton<Identifier> {
    friend class Singleton<Identifier>;

public:
    Identifier() {}
    ~Identifier() {}

    uint32_t MakeIdentifier()
    {
        return ++key_;
    }

private:
    std::atomic<uint32_t> key_ = 0;
};

class IdentifierInfo {
public:
    IdentifierInfo()
    {
        id_ = Identifier::GetInstance().MakeIdentifier();
    }

    std::string GetIdentifier()
    {
        return std::to_string(id_);
    }

    uint32_t GetId()
    {
        return id_;
    }

private:
    uint32_t id_ = -1;
};

} // namespace Sharing
} // namespace OHOS
#endif
