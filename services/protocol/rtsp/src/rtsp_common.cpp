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

#include "rtsp_common.h"
#include <regex>
#include "common/media_log.h"
#include "utils/crypto.h"

namespace OHOS {
namespace Sharing {
std::string RtspCommon::GetRtspDate()
{
    time_t now = time(nullptr);
    if (now <= 0) {
        return {};
    }
    struct tm *t = gmtime(&now);
    if (t == nullptr) {
        return {};
    }
    char buf[32] = {0};
    if (strftime(buf, 128, "%a, %b %d %Y %H:%M:%S GMT", t) < 0) { // 128:fixed size
        return {};
    }
    return buf;
}

std::vector<std::string> RtspCommon::Split(const std::string &str, const std::string &delimiter)
{
    std::regex reg{delimiter};
    return {std::sregex_token_iterator(str.begin(), str.end(), reg, -1), std::sregex_token_iterator()};
}

void RtspCommon::SplitParameter(std::list<std::string> &lines, std::list<std::pair<std::string, std::string>> &params)
{
    for (auto &line : lines) {
        if (line.size() > 3) { // 3:fixed size
            auto index = line.find_first_of(':');
            if (index != 0 && index + 1 != line.length()) {
                auto token = RtspCommon::Trim(line.substr(0, index));
                auto value = RtspCommon::Trim(line.substr(index + 1));
                params.emplace_back(token, value);
            }
        }
    }
}

bool RtspCommon::VerifyMethod(const std::string &method)
{
    return method == RTSP_METHOD_OPTIONS || method == RTSP_METHOD_DESCRIBE || method == RTSP_METHOD_ANNOUNCE ||
           method == RTSP_METHOD_SETUP || method == RTSP_METHOD_PLAY || method == RTSP_METHOD_PAUSE ||
           method == RTSP_METHOD_TEARDOWN || method == RTSP_METHOD_GET_PARAMETER ||
           method == RTSP_METHOD_SET_PARAMETER || method == RTSP_METHOD_REDIRECT || method == RTSP_METHOD_RECORD;
}

std::string RtspCommon::Trim(const std::string &str)
{
    if (str.empty())
        return str;
    auto s = str.find_first_not_of(' ');
    auto e = str.find_last_not_of(' ');
    if (s == std::string::npos || e == std::string::npos) {
        return str;
    }

    return str.substr(s, e + 1 - s);
}

RtspError RtspCommon::ParseMessage(const std::string &message, std::vector<std::string> &firstLine,
                                   std::unordered_map<std::string, std::string> &header, std::list<std::string> &body)
{
    auto messageV = RtspCommon::Split(message, std::string(RTSP_CRLF) + RTSP_CRLF);
    auto headers = messageV[0];

    std::vector<std::string> lines = RtspCommon::Split(headers, RTSP_CRLF);
    if (lines.size() < 2) { // 2:fixed size
        return {RtspErrorType::INVALID_MESSAGE, "invalid message"};
    }

    firstLine = RtspCommon::Split(lines[0], RTSP_SP);

    // parse header
    for (int32_t i = 1; i < (int32_t)lines.size(); ++i) {
        if (lines[i].size() > 3) { // 3:fixed size
            auto index = lines[i].find_first_of(':');
            if (index != 0 && index != std::string::npos && index + 1 != lines[i].length()) {
                auto token = RtspCommon::Trim(lines[i].substr(0, index));
                auto value = RtspCommon::Trim(lines[i].substr(index + 1));
                if (token == "WWW-Authenticate" && value.find("Digest") == std::string::npos) {
                    continue;
                }
                header.emplace(token, value);
            }
        }
    }

    // parse body
    if (messageV.size() == 2 && header.find(RTSP_TOKEN_CONTENT_TYPE) != header.end() && // 2:fixed size
        header.find(RTSP_TOKEN_CONTENT_LENGTH) != header.end()) {
        int32_t length = atoi(header.at(RTSP_TOKEN_CONTENT_LENGTH).c_str());
        if (length == 0) {
            SHARING_LOGW("Content-Length == 0.");
            return {};
        }

        if (length > (int32_t)messageV[1].length()) {
            SHARING_LOGE("invalid body: Content-Length: %{public}d, body length: %{public}zu\n!", length,
                         messageV[1].length());
            return {RtspErrorType::INCOMPLETE_MESSAGE, "body length < Content-Length"};
        }

        if (header.at(RTSP_TOKEN_CONTENT_TYPE).compare(0, 5, "text/") != 0 && // 5:fixed size
            header.at(RTSP_TOKEN_CONTENT_TYPE) != "application/sdp") {
            return {RtspErrorType::INVALID_MESSAGE, "unsupported content"};
        }

        auto bodyStr = messageV[1].substr(0, length);
        auto bodyVec = RtspCommon::Split(bodyStr, RTSP_CRLF);
        for (auto &item : bodyVec) {
            if (!item.empty()) {
                body.emplace_back(item);
            }
        }
    } else {
        if (messageV.size() > 1) {
            SHARING_LOGW("may packet splicing.");
            std::string splicingPart;
            for (int32_t i = 1; i < messageV.size(); ++i) {
                splicingPart += messageV[i];
                if (i != messageV.size() - 1) {
                    splicingPart += "\r\n\r\n";
                }
            }
            splicingPart += '$';
            SHARING_LOGW("splicing \n%{public}s!", splicingPart.c_str());
            return {RtspErrorType::OK, splicingPart};
        }
    }

    return {};
}
std::string RtspCommon::GenerateAuthorization(const std::string &username, const std::string &realm,
                                              const std::string &password, const std::string &nonce,
                                              const std::string &method, const std::string &url)
{
    auto urpMD5 = GetMD5(username + ':' + realm + ':' + password);
    auto pmuMD5 = GetMD5(method + url);
    auto responseMD5 = GetMD5(urpMD5 + ':' + nonce + ':' + pmuMD5);
    auto authorization = "Digest username=\"" + username + "\", realm=\"" + realm + "\", nonce=\"" + nonce +
                         "\", uri=\"" + url + "\", response=\"" + responseMD5 + "\"";
    return authorization;
}
} // namespace Sharing
} // namespace OHOS