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

#ifndef OHOS_SHARING_RTSP_URI_H
#define OHOS_SHARING_RTSP_URI_H

#include <iostream>
#include <regex>
#include <string>

#define RTSP_DEFAULT_PORT 554
namespace OHOS {
namespace Sharing {
class RtspUrl {
public:
    RtspUrl() = default;
    explicit RtspUrl(const std::string &url)
    {
        Parse(url);
    }

    bool Parse(const std::string &url)
    {
        if (url.empty()) {
            return false;
        }

        std::regex match("^rtsp://(([a-zA-Z0-9]+):([a-zA-Z0-9]+)@)?([a-zA-Z0-9.-]+)(:([0-9]+))?");
        std::smatch sm;
        if (!std::regex_search(url, sm, match)) {
            return false;
        }
        if (sm.size() != 7) { // 7:fixed size
            return false;
        }
        username_ = sm[2].str();                       // 2:matching position
        password_ = sm[3].str();                       // 3:matching position
        host_ = sm[4].str();                           // 4:matching position
        int32_t port = std::atoi(sm[6].str().c_str()); // 6:matching position
        if (port > 0) {
            port_ = port;
        }
        path_ = sm.suffix().str();
        if (username_.empty() && path_.find("?username") != std::string::npos) {
            auto ui = path_.find("?username");
            auto pi = path_.find("&password");
            if (pi != std::string::npos) {
                username_ = path_.substr(ui + 10, pi - ui - 10); // 10:matching position
                password_ = path_.substr(pi + 10);               // 10:matching position
            } else {
                username_ = path_.substr(ui + 10); // 10:matching position
            }
            path_ = path_.substr(0, path_.find('?'));
        }

        return true;
    }

    std::string GetHost() const
    {
        return host_;
    }

    std::string GetPath() const
    {
        return path_;
    }

    uint16_t GetPort() const
    {
        return port_;
    }

    std::string GetUsername() const
    {
        return username_;
    }

    std::string GetPassword() const
    {
        return password_;
    }

    std::string GetUrl() const
    {
        return "rtsp://" + host_ + ':' + std::to_string(port_) + path_;
    }

private:
    uint16_t port_ = RTSP_DEFAULT_PORT;

    std::string host_;
    std::string path_;
    std::string username_;
    std::string password_;
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_URI_H
