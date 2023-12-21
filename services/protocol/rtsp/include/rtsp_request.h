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

#ifndef OHOS_SHARING_RTSP_REQUEST_H
#define OHOS_SHARING_RTSP_REQUEST_H

#include <list>
#include <string>
#include <unordered_map>
#include "rtsp_common.h"

namespace OHOS {
namespace Sharing {
class RtspRequest {
public:
    RtspRequest() = default;
    explicit RtspRequest(const std::string &request) { Parse(request); }

    RtspRequest(const std::string &method, int32_t cseq, const std::string &url)
        : cSeq_(cseq), method_(method), url_(url){};
    virtual ~RtspRequest() = default;

    RtspRequest &SetMethod(const std::string &method)
    {
        method_ = method;
        return *this;
    }

    std::string GetMethod() const { return method_; }

    RtspRequest &SetCSeq(int32_t cSeq)
    {
        cSeq_ = cSeq;
        return *this;
    }

    int32_t GetCSeq() const { return cSeq_; }

    RtspRequest &SetUrl(const std::string &url)
    {
        url_ = url;
        return *this;
    }

    std::string GetUrl() const { return url_; }

    RtspRequest &SetAuthorization(const std::string &authorization)
    {
        authorization_ = authorization;
        return *this;
    }

    std::string GetAuthorization() { return authorization_; }

    RtspRequest &SetUserAgent(const std::string &userAgent)
    {
        userAgent_ = userAgent;
        return *this;
    }

    std::string GetUserAgent() const { return userAgent_; }

    RtspRequest &SetSession(const std::string &session)
    {
        session_ = session;
        return *this;
    }

    RtspRequest &SetTimeout(int32_t timeout)
    {
        timeout_ = timeout;
        return *this;
    }

    std::string GetSession() const { return session_; }

    RtspRequest &AddCustomHeader(const std::string &header)
    {
        customHeaders_ += header;
        return *this;
    }

    void ClearCustomHeader() { customHeaders_.clear(); }

    std::list<std::string> GetBody() const { return body_; }

    virtual std::string Stringify();
    virtual RtspError Parse(const std::string &request);

    std::string GetToken(const std::string &token);

protected:
    std::list<std::string> body_;

private:
    int32_t cSeq_ = 0;
    int32_t timeout_ = -1;
    std::string method_;
    std::string session_;
    std::string url_ = "*";
    std::string authorization_;
    std::string customHeaders_;
    std::string userAgent_ = "KaihongOS";

    std::unordered_map<std::string, std::string> tokens_;
};

class RtspRequestOptions : public RtspRequest {
public:
    RtspRequestOptions(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_OPTIONS, cseq, url) {}
    RtspRequestOptions &SetRequire(const std::string &methods)
    {
        if (!methods.empty()) {
            require_ = methods;
        }

        return *this;
    }

    std::string Stringify() override;

private:
    std::string require_;
};

class RtspRequestDescribe : public RtspRequest {
public:
    RtspRequestDescribe(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_DESCRIBE, cseq, url) {}
    RtspRequestDescribe &SetAcceptType(const std::string &type)
    {
        if (!type.empty()) {
            acceptType_ = type;
        }

        return *this;
    }

    std::string Stringify() override;

private:
    std::string acceptType_ = "application/sdp";
};

class RtspRequestSetup : public RtspRequest {
public:
    RtspRequestSetup() = default;
    RtspRequestSetup(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_SETUP, cseq, url) {}

    RtspRequestSetup &SetClientPort(int32_t minPort, int32_t maxPort = 0)
    {
        if (minPort > 0) {
            minPort_ = minPort;
        }
        if (maxPort > minPort) {
            maxPort_ = maxPort;
        }

        return *this;
    }

    std::string Stringify() override;

private:
    int32_t minPort_ = 0;
    int32_t maxPort_ = 0;
};

class RtspRequestPlay : public RtspRequest {
public:
    RtspRequestPlay() = default;
    RtspRequestPlay(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_PLAY, cseq, url) {}
    RtspRequestPlay &SetRangeStart(float start)
    {
        range_ = start;
        return *this;
    }

    std::string Stringify() override;

private:
    float range_ = -1;
};

class RtspRequestPause : public RtspRequest {
public:
    RtspRequestPause() = default;
    RtspRequestPause(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_PAUSE, cseq, url) {}
};

class RtspRequestTeardown : public RtspRequest {
public:
    RtspRequestTeardown() = default;
    RtspRequestTeardown(int32_t cseq, const std::string &url) : RtspRequest(RTSP_METHOD_TEARDOWN, cseq, url) {}
};

class RtspRequestParameter : public RtspRequest {
public:
    RtspRequestParameter() = default;
    RtspRequestParameter(const std::string &method, int32_t cseq, const std::string &url)
        : RtspRequest(method, cseq, url){};
    ~RtspRequestParameter() override = default;

    RtspRequestParameter &AddBodyItem(const std::string &line)
    {
        body_.emplace_back(line);
        return *this;
    }

    std::string Stringify() override;
};

class RtspRequestGetParameter : public RtspRequestParameter {
public:
    RtspRequestGetParameter(int32_t cseq, const std::string &url)
        : RtspRequestParameter(RTSP_METHOD_GET_PARAMETER, cseq, url){};
};

class RtspRequestSetParameter : public RtspRequestParameter {
public:
    RtspRequestSetParameter(int32_t cseq, const std::string &url)
        : RtspRequestParameter(RTSP_METHOD_SET_PARAMETER, cseq, url){};
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_REQUEST_H
