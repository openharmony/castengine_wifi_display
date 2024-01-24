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

#include "rtsp_request.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace OHOS {
namespace Sharing {
std::string RtspRequest::Stringify()
{
    std::stringstream ss;
    ss << method_ << RTSP_SP << url_ << RTSP_SP << RTSP_VERSION << RTSP_CRLF;
    ss << RTSP_TOKEN_CSEQ << ":" << RTSP_SP << cSeq_ << RTSP_CRLF;
    if (!authorization_.empty()) {
        ss << RTSP_TOKEN_AUTHORIZATION << ":" << RTSP_SP << authorization_ << RTSP_CRLF;
    }
    ss << RTSP_TOKEN_UA << ":" << RTSP_SP << userAgent_ << RTSP_CRLF;
    if (!session_.empty()) {
        ss << RTSP_TOKEN_SESSION << ":" << RTSP_SP << session_;
        if (timeout_ > 0) {
            ss << ";timeout=" << timeout_;
        }
        ss << RTSP_CRLF;
    }

    if (!customHeaders_.empty()) {
        ss << customHeaders_;
    }
    ss << RTSP_CRLF;
    return ss.str();
}

RtspError RtspRequest::Parse(const std::string &request)
{
    std::vector<std::string> firstLine;
    auto result = RtspCommon::ParseMessage(request, firstLine, tokens_, body_);
    if (result.code != RtspErrorType::OK) {
        return result;
    }

    if (firstLine.size() < 2 || tokens_.empty()) { // 2:rtsp line
        tokens_.clear();
        body_.clear();
        return {RtspErrorType::INVALID_MESSAGE, "invalid message"};
    }

    // "METHOD URL VERSION"
    if (firstLine.size() != 3 || firstLine[2] != RTSP_VERSION) { // 2:rtsp line, 3:rtsp line
        tokens_.clear();
        body_.clear();
        return {RtspErrorType::INVALID_MESSAGE, "invalid message"};
    }

    if (!RtspCommon::VerifyMethod(firstLine[0])) {
        tokens_.clear();
        body_.clear();
        return {RtspErrorType::INVALID_METHOD, "invalid method"};
    }

    method_ = firstLine[0];
    url_ = firstLine[1];

    if (tokens_.find(RTSP_TOKEN_CSEQ) != tokens_.end()) {
        cSeq_ = atoi(tokens_.at(RTSP_TOKEN_CSEQ).c_str());
    }

    if (tokens_.find(RTSP_TOKEN_UA) != tokens_.end()) {
        userAgent_ = tokens_.at(RTSP_TOKEN_UA);
    }

    if (tokens_.find(RTSP_TOKEN_SESSION) != tokens_.end()) {
        session_ = tokens_.at(RTSP_TOKEN_SESSION);
    }

    return {};
}

std::string RtspRequest::GetToken(const std::string &token)
{
    if (tokens_.find(token) != tokens_.end()) {
        return tokens_.at(token);
    }

    return {};
}

std::string RtspRequestOptions::Stringify()
{
    if (!require_.empty()) {
        std::stringstream ss;
        ss << RTSP_TOKEN_REQUIRE << ":" << RTSP_SP << require_ << RTSP_CRLF;
        ClearCustomHeader();
        AddCustomHeader(ss.str());
    }

    return RtspRequest::Stringify();
}

std::string RtspRequestDescribe::Stringify()
{
    std::stringstream ss;
    ss << RTSP_TOKEN_ACCEPT << ":" << RTSP_SP << acceptType_ << RTSP_CRLF;
    ClearCustomHeader();
    AddCustomHeader(ss.str());
    return RtspRequest::Stringify();
}

std::string RtspRequestSetup::Stringify()
{
    std::stringstream ss;
    ss << RTSP_TOKEN_TRANSPORT << ":" << RTSP_SP << "RTP/AVP;unicast;client_port=" << minPort_;
    if (maxPort_ > 0) {
        ss << "-" << maxPort_;
    }
    ss << RTSP_CRLF;
    ClearCustomHeader();
    AddCustomHeader(ss.str());
    return RtspRequest::Stringify();
}

std::string RtspRequestPlay::Stringify()
{
    if (range_ >= 0) {
        std::stringstream ss;
        ss << RTSP_TOKEN_RANGE << ":" << RTSP_SP << "npt=";
        ss << std::fixed << std::setprecision(3) << range_ << "-" << RTSP_CRLF; // 3:precision
        ClearCustomHeader();
        AddCustomHeader(ss.str());
    }

    return RtspRequest::Stringify();
}

std::string RtspRequestParameter::Stringify()
{
    std::stringstream body;
    for (auto &item : body_) {
        body << item << RTSP_CRLF;
    }

    if (body.str().empty()) {
        return RtspRequest::Stringify();
    } else {
        std::stringstream ss;
        ss << RTSP_TOKEN_CONTENT_TYPE << ":" << RTSP_SP << "text/parameters" << RTSP_CRLF;
        ss << RTSP_TOKEN_CONTENT_LENGTH << ":" << RTSP_SP << body.str().length() << RTSP_CRLF;
        ClearCustomHeader();
        AddCustomHeader(ss.str());
        return RtspRequest::Stringify() + body.str();
    }
}
} // namespace Sharing
} // namespace OHOS