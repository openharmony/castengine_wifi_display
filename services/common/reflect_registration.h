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

#ifndef OHOS_SHARING_REFLECT_REGISTRATION_H
#define OHOS_SHARING_REFLECT_REGISTRATION_H

#include <atomic>
#include <functional>
#include <mutex>
#include <singleton.h>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace Sharing {

using Constructor = std::function<std::shared_ptr<void>()>;

class ReflectRegistration : public Singleton<ReflectRegistration> {
    friend class Singleton<ReflectRegistration>;

public:
    ReflectRegistration() = default;
    explicit ReflectRegistration(ReflectRegistration &&ref) {}
    explicit ReflectRegistration(const ReflectRegistration &ref) {}
    ~ReflectRegistration();

    void Unregister(const std::string &descriptor);
    bool Register(const std::string &descriptor, Constructor creator);
    std::shared_ptr<void> CreateObject(const std::string &descriptor);

private:
    ReflectRegistration &operator=(ReflectRegistration &&) = delete;
    ReflectRegistration &operator=(const ReflectRegistration &) = delete;

private:
    std::mutex creatorMutex_;
    std::unordered_map<std::string, Constructor> creators_;
};

class ReflectClassRegister {
public:
    ReflectClassRegister(const std::string &name, Constructor func)
    {
        ReflectRegistration::GetInstance().Register(name, func);
    };

private:
    ReflectClassRegister() = delete;
};

#define REGISTER_CLASS_REFLECTOR(className)                    \
    static std::shared_ptr<className> className##Creator(void) \
    {                                                          \
        auto obj = std::make_shared<className>();              \
        obj->SetClassName(#className);                         \
        return obj;                                            \
    }                                                          \
    const ReflectClassRegister className##Register(#className, className##Creator)

template <class T>
class ClassReflector {
public:
    static std::shared_ptr<T> Class2Instance(const std::string &name)
    {
        return std::static_pointer_cast<T>(ReflectRegistration::GetInstance().CreateObject(name));
    };
};

} // namespace Sharing
} // namespace OHOS
#endif