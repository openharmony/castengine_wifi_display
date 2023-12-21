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

#ifndef OHOS_SHARING_RTSP_RESPONSE_H
#define OHOS_SHARING_RTSP_RESPONSE_H

#include <list>
#include <string>
#include <unordered_map>
#include "rtsp_common.h"

namespace OHOS {
namespace Sharing {
class RtspResponse {
public:
    RtspResponse() = default;
    RtspResponse(int32_t cseq, int32_t status) : cSeq_(cseq), status_(status) {}
    virtual ~RtspResponse() = default;

    RtspResponse &AddCustomHeader(const std::string &header)
    {
        customHeaders_ += header;
        return *this;
    }

    RtspResponse &SetSession(const std::string &session)
    {
        session_ = session;
        return *this;
    }

    RtspResponse &SetTimeout(int32_t timeout)
    {
        timeout_ = timeout;
        return *this;
    }

    int32_t GetCSeq() const { return cSeq_; }

    std::string GetDate() const { return date_; }

    int32_t GetStatus() const { return status_; }

    std::string GetSession() const { return session_; }

    std::string GetDigestRealm() const { return digestRealm_; }

    std::string GetNonce() const { return nonce_; }

    int32_t GetTimeout() const { return timeout_; }

    std::list<std::string> GetBody() const { return body_; }

    void ClearCustomHeader() { customHeaders_.clear(); }

    virtual std::string Stringify();
    virtual RtspError Parse(const std::string &response);
    std::string GetToken(const std::string &token) const;

protected:
    std::list<std::string> body_;

private:
    int32_t cSeq_ = 0;
    int32_t status_ = 200;
    int32_t timeout_ = -1;

    std::string date_;
    std::string nonce_;
    std::string session_;
    std::string customHeaders_;
    std::string digestRealm_;

    std::unordered_map<std::string, std::string> tokens_;
};

class RtspResponseOptions : public RtspResponse {
public:
    RtspResponseOptions(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
    RtspResponseOptions &SetPublicItems(const std::string &lists)
    {
        publicItems_ = lists;
        return *this;
    }

    std::string Stringify() override;

private:
    std::string publicItems_;
};

class RtspResponseDescribe : public RtspResponse {
public:
    RtspResponseDescribe(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
    std::string Stringify() override;

    RtspResponseDescribe &AddBodyItem(const std::string &line)
    {
        body_.emplace_back(line);
        return *this;
    }

    RtspResponseDescribe &SetUrl(const std::string &url)
    {
        url_ = url;
        return *this;
    }

private:
    std::string url_;
};

class RtspResponseSetup : public RtspResponse {
public:
    RtspResponseSetup(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
    std::string Stringify() override;

    RtspResponseSetup &SetDestination(const std::string &destination)
    {
        destination_ = destination;
        return *this;
    }

    RtspResponseSetup &SetSource(const std::string &source)
    {
        source_ = source;
        return *this;
    }

    RtspResponseSetup &SetClientPort(int32_t minClientPort, int32_t maxClientPort = 0)
    {
        minClientPort_ = minClientPort;
        if (maxClientPort > minClientPort) {
            maxServerPort_ = maxClientPort;
        }

        return *this;
    }

    RtspResponseSetup &SetServerPort(int32_t minServerPort, int32_t maxServerPort = 0)
    {
        minServerPort_ = minServerPort;
        if (maxServerPort > minServerPort) {
            maxServerPort_ = maxServerPort;
        }

        return *this;
    }

private:
    int32_t minClientPort_ = 0;
    int32_t maxClientPort_ = 0;
    int32_t minServerPort_ = 0;
    int32_t maxServerPort_ = 0;

    std::string source_;
    std::string destination_;
};

class RtspResponsePlay : public RtspResponse {
public:
    RtspResponsePlay(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
};

class RtspResponseTeardown : public RtspResponse {
public:
    RtspResponseTeardown(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
};

class RtspResponseGetParameter : public RtspResponse {
public:
    RtspResponseGetParameter(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
    std::string Stringify() override;

    RtspResponseGetParameter &AddBodyItem(const std::string &line)
    {
        body_.emplace_back(line);
        return *this;
    }

private:
    std::list<std::string> body_;
};

class RtspResponseSetParameter : public RtspResponse {
public:
    RtspResponseSetParameter(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_RESPONSE_H
