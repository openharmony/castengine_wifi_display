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

#include "rtsp_response.h"
#include <iostream>
#include <sstream>

namespace OHOS {
namespace Sharing {
std::string RtspResponse::Stringify()
{
    std::stringstream ss;
    ss << RTSP_VERSION << RTSP_SP << status_ << RTSP_SP;

    switch (status_) {
        case RTSP_STATUS_OK:
            ss << RTSP_STATUS_TO_STR(RTSP_STATUS_OK);
            break;
        case RTSP_STATUS_CONTINUE:
            ss << RTSP_STATUS_TO_STR(RTSP_STATUS_CONTINUE);
            break;
        case RTSP_STATUS_BAD_REQUEST:
            ss << RTSP_STATUS_TO_STR(RTSP_STATUS_BAD_REQUEST);
            break;
        case RTSP_STATUS_UNAUTHORIZED:
            ss << RTSP_STATUS_TO_STR(RTSP_STATUS_UNAUTHORIZED);
            break;
        default:
            ss << RTSP_STATUS_TO_STR(RTSP_STATUS_OK);
            break;
    }

    ss << RTSP_CRLF;

    ss << RTSP_TOKEN_CSEQ << ":" << RTSP_SP << cSeq_ << RTSP_CRLF;

    ss << RTSP_TOKEN_DATE << ":" << RTSP_SP << RtspCommon::GetRtspDate() << RTSP_CRLF;

    if (!customHeaders_.empty()) {
        ss << customHeaders_ << RTSP_CRLF;
    }
    if (!session_.empty()) {
        ss << RTSP_TOKEN_SESSION << ":" << RTSP_SP << session_;
        if (timeout_ > 0) {
            ss << ";timeout=" << timeout_;
        }
        ss << RTSP_CRLF;
    }
    ss << RTSP_CRLF;
    return ss.str();
}

RtspError RtspResponse::Parse(const std::string &response)
{
    std::vector<std::string> firstLine;
    auto result = RtspCommon::ParseMessage(response, firstLine, tokens_, body_);
    if (result.code != RtspErrorType::OK || firstLine.size() < 2 || tokens_.empty()) { // 2:rstp line
        tokens_.clear();
        body_.clear();
        return {RtspErrorType::INVALID_MESSAGE, "invalid message"};
    }

    // "RTSP/1.0 200 OK"
    if (firstLine.size() < 3 || firstLine[0] != RTSP_VERSION) { // 3:rstp line
        tokens_.clear();
        body_.clear();
        return {RtspErrorType::INVALID_MESSAGE, "invalid message"};
    }

    status_ = atoi(firstLine[1].c_str());

    if (tokens_.find(RTSP_TOKEN_CSEQ) != tokens_.end()) {
        cSeq_ = (int32_t)strtol(tokens_.at(RTSP_TOKEN_CSEQ).c_str(), nullptr, 10); // 10:unit
    }

    if (tokens_.find(RTSP_TOKEN_DATE) != tokens_.end()) {
        date_ = tokens_.at(RTSP_TOKEN_DATE);
    }

    if (tokens_.find(RTSP_TOKEN_SESSION) != tokens_.end()) {
        session_ = tokens_.at(RTSP_TOKEN_SESSION);
        auto si = session_.find(';');
        if (si != std::string::npos) {
            std::string to = session_.substr(si + 1);
            auto ti = to.find('=');
            if (ti != std::string::npos) {
                auto timeoutStr = to.substr(ti + 1);
                timeout_ = atoi(timeoutStr.c_str());
            }

            session_ = session_.substr(0, si);
        }
    }

    if (tokens_.find(RTSP_TOKEN_WWW_AUTHENTICATE) != tokens_.end()) {
        auto authenticate = RtspCommon::Split(tokens_.at(RTSP_TOKEN_WWW_AUTHENTICATE), ", ");
        for (auto &item : authenticate) {
            auto separator = item.find('=');
            if (separator != std::string::npos) {
                auto key = item.substr(0, separator);
                if (key == "Digest realm") {
                    auto value = item.substr(separator + 2); // 2:fixed size
                    value.pop_back();
                    digestRealm_ = value;
                } else if (key == "nonce") {
                    auto value = item.substr(separator + 2); // 2:fixed size
                    value.pop_back();
                    nonce_ = value;
                }
            }
        }
    }

    if (result.info.size() > 2 && result.info.back() == '$') { // 2:fixed size
        return result;
    }

    return {};
}

std::string RtspResponse::GetToken(const std::string &token) const
{
    if (tokens_.find(token) != tokens_.end()) {
        return tokens_.at(token);
    }

    return {};
}

std::string RtspResponseOptions::Stringify()
{
    std::stringstream ss;
    ss << RTSP_TOKEN_PUBLIC << ":" << RTSP_SP << publicItems_;
    ClearCustomHeader();
    AddCustomHeader(ss.str());
    return RtspResponse::Stringify();
}

std::string RtspResponseDescribe::Stringify()
{
    std::stringstream body;
    for (auto &item : body_) {
        body << item << RTSP_CRLF;
    }

    std::stringstream ss;
    if (!url_.empty()) {
        ss << RTSP_TOKEN_CONTENT_BASE << ":" << RTSP_SP << url_ << RTSP_CRLF;
    }
    ss << RTSP_TOKEN_CONTENT_TYPE << ":" << RTSP_SP << "application/sdp" << RTSP_CRLF;
    ss << RTSP_TOKEN_CONTENT_LENGTH << ":" << RTSP_SP << body.str().length();
    ClearCustomHeader();
    AddCustomHeader(ss.str());

    return RtspResponse::Stringify() + body.str();
}

std::string RtspResponseSetup::Stringify()
{
    std::stringstream ss;

    ss << RTSP_TOKEN_TRANSPORT << ":" << RTSP_SP << "RTP/AVP;unicast;";
    if (!destination_.empty()) {
        ss << "destination=" << destination_ << ";";
    }
    if (!source_.empty()) {
        ss << "source=" << source_ << ";";
    }
    if (minClientPort_ != 0) {
        ss << "client_port=" << minClientPort_ << "-"
           << (maxClientPort_ > minClientPort_ ? maxClientPort_ : (minClientPort_ + 1)) << ";";
    }

    if (minServerPort_ != 0) {
        ss << "server_port=" << minServerPort_ << "-"
           << (maxServerPort_ > minServerPort_ ? maxServerPort_ : (minServerPort_ + 1)) << ";";
    }
    ss << RTSP_CRLF;
    ClearCustomHeader();
    AddCustomHeader(ss.str());
    return RtspResponse::Stringify();
}

std::string RtspResponseGetParameter::Stringify()
{
    std::stringstream body;
    for (auto &item : body_) {
        body << item << RTSP_CRLF;
    }

    std::stringstream ss;
    ss << RTSP_TOKEN_CONTENT_TYPE << ":" << RTSP_SP << "text/parameters" << RTSP_CRLF;
    ss << RTSP_TOKEN_CONTENT_LENGTH << ":" << RTSP_SP << body.str().length();
    ClearCustomHeader();
    AddCustomHeader(ss.str());

    if (body.str().empty()) {
        return RtspResponse::Stringify();
    }

    return RtspResponse::Stringify() + body.str();
}
} // namespace Sharing
} // namespace OHOS