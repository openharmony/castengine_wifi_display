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

#ifndef OHOS_SHARING_RTSP_DEF_H
#define OHOS_SHARING_RTSP_DEF_H

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace OHOS {
namespace Sharing {

// RTSP methods
#define RTSP_METHOD_OPTIONS       "OPTIONS"
#define RTSP_METHOD_DESCRIBE      "DESCRIBE"
#define RTSP_METHOD_ANNOUNCE      "ANNOUNCE"
#define RTSP_METHOD_SETUP         "SETUP"
#define RTSP_METHOD_PLAY          "PLAY"
#define RTSP_METHOD_PAUSE         "PAUSE"
#define RTSP_METHOD_TEARDOWN      "TEARDOWN"
#define RTSP_METHOD_GET_PARAMETER "GET_PARAMETER"
#define RTSP_METHOD_SET_PARAMETER "SET_PARAMETER"
#define RTSP_METHOD_REDIRECT      "REDIRECT"
#define RTSP_METHOD_RECORD        "RECORD"

// RTSP version
#define RTSP_VERSION "RTSP/1.0"
#define RTSP_CRLF    "\r\n"
#define RTSP_SP      " "

// RTSP token
#define RTSP_TOKEN_CSEQ             "CSeq"
#define RTSP_TOKEN_DATE             "Date"
#define RTSP_TOKEN_PUBLIC           "Public"
#define RTSP_TOKEN_CONTENT_BASE     "Content-Base"
#define RTSP_TOKEN_CONTENT_TYPE     "Content-Type"
#define RTSP_TOKEN_CONTENT_LENGTH   "Content-Length"
#define RTSP_TOKEN_TRANSPORT        "Transport"
#define RTSP_TOKEN_SESSION          "Session"
#define RTSP_TOKEN_UA               "User-Agent"
#define RTSP_TOKEN_ACCEPT           "Accept"
#define RTSP_TOKEN_RANGE            "Range"
#define RTSP_TOKEN_REQUIRE          "Require"
#define RTSP_TOKEN_AUTHORIZATION    "Authorization"
#define RTSP_TOKEN_WWW_AUTHENTICATE "WWW-Authenticate"

#define RTSP_STATUS_CONTINUE         100
#define RTSP_STATUS_OK               200
#define RTSP_STATUS_BAD_REQUEST      400
#define RTSP_STATUS_UNAUTHORIZED     401
#define RTSP_STATUS_STREAM_NOT_FOUND 404

#define RTSP_STATUS_CONTINUE_STR         "Continue"
#define RTSP_STATUS_OK_STR               "OK"
#define RTSP_STATUS_BAD_REQUEST_STR      "Bad Request"
#define RTSP_STATUS_UNAUTHORIZED_STR     "Unauthorized"
#define RTSP_STATUS_STREAM_NOT_FOUND_STR "Stream Not Found"

#define RTSP_STATUS_TO_STR(status) status##_STR

enum class RtspErrorType { OK = 0, INVALID_MESSAGE = 0x1000, INVALID_METHOD, INCOMPLETE_MESSAGE };

struct RtspError {
    RtspErrorType code = RtspErrorType::OK;

    std::string info = "ok";
};

class RtspCommon {
public:
    static std::string GetRtspDate();
    static std::string Trim(const std::string &str);

    static bool VerifyMethod(const std::string &method);
    static std::vector<std::string> Split(const std::string &str, const std::string &delimiter);

    static void SplitParameter(std::list<std::string> &lines, std::list<std::pair<std::string, std::string>> &params);

    static RtspError ParseMessage(const std::string &message, std::vector<std::string> &firstLine,
                                  std::unordered_map<std::string, std::string> &header, std::list<std::string> &body);

    static std::string GenerateAuthorization(const std::string &username, const std::string &realm,
                                             const std::string &password, const std::string &nonce,
                                             const std::string &method, const std::string &url);
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_DEF_H
