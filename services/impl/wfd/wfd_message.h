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

#ifndef OHOS_SHARING_WFD_MESSAGE_H
#define OHOS_SHARING_WFD_MESSAGE_H

#include <sstream>
#include "protocol/rtsp/include/rtsp_request.h"
#include "protocol/rtsp/include/rtsp_response.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

struct WfdVideoFormatsInfo {
    uint16_t native = 0;
    uint16_t preferredDisplayMode = 0;
    uint16_t profile = 0;
    uint16_t level = 0;
    uint32_t ceaMask = 0;
    uint32_t veaMask = 0;
    uint32_t hhMask = 0;
    uint16_t latency = 0;
    uint16_t minSlice = 0;
    uint16_t sliceEncParam = 0;
    uint16_t frameRateCtlSupport = 0;
};

// WfdRtspM1Request
class WfdRtspM1Request : public RtspRequestOptions {
public:
    explicit WfdRtspM1Request(int32_t cseq) : RtspRequestOptions(cseq, "*") { SetRequire(RTSP_METHOD_WFD); }
};

class WfdRtspM1Response : public RtspResponseOptions {
public:
    WfdRtspM1Response(int32_t cseq, int32_t status);
};

class WfdRtspM2Request : public RtspRequestOptions {
public:
    explicit WfdRtspM2Request(int32_t cseq) : RtspRequestOptions(cseq, "*") { SetRequire(RTSP_METHOD_WFD); }
};

// WfdRtspM2Response
class WfdRtspM2Response : public RtspResponseOptions {
public:
    WfdRtspM2Response(int32_t cseq, int32_t status) : RtspResponseOptions(cseq, status)
    {
        std::stringstream ss;
        ss << RTSP_METHOD_WFD << "," << RTSP_SP << RTSP_METHOD_SETUP << "," << RTSP_SP << RTSP_METHOD_TEARDOWN << ","
           << RTSP_SP << RTSP_METHOD_PLAY << "," << RTSP_SP << RTSP_METHOD_PAUSE << "," << RTSP_SP
           << RTSP_METHOD_GET_PARAMETER << "," << RTSP_SP << RTSP_METHOD_SET_PARAMETER;
        SetPublicItems(ss.str());
    }
};

// WfdRtspM3Request
class WfdRtspM3Request : public RtspRequestParameter {
public:
    WfdRtspM3Request(int32_t cseq, const std::string &url) : RtspRequestParameter(RTSP_METHOD_GET_PARAMETER, cseq, url)
    {
        AddBodyItem(WFD_PARAM_CONTENT_PROTECTION);
        AddBodyItem(WFD_PARAM_VIDEO_FORMATS);
        AddBodyItem(WFD_PARAM_AUDIO_CODECS);
        AddBodyItem(WFD_PARAM_RTP_PORTS);
    }
};

class WfdRtspM3Response : public RtspResponse {
public:
    WfdRtspM3Response() = default;
    ~WfdRtspM3Response() override = default;
    WfdRtspM3Response(int32_t cseq, int32_t status) : RtspResponse(cseq, status) {}

    void SetClientRtpPorts(int32_t port);
    void SetUibcCapability(const std::string &value = "none");
    void SetAudioCodecs(AudioFormat format = AUDIO_48000_16_2);
    void SetCoupledSink(const std::string &value = "00  none");
    void SetContentProtection(const std::string &value = "none");
    void SetVideoFormats(VideoFormat format = VIDEO_1920X1080_30);
    void SetStandbyResumeCapability(const std::string &value = "none");
    void SetCustomParam(const std::string &key, const std::string &value = "none");

    int32_t GetClientRtpPorts();
    std::string GetUibcCapability();
    AudioFormat GetAudioCodecs();
    std::string GetCoupledSink();
    std::string GetContentProtection();
    VideoFormat GetVideoFormats();
    std::string GetStandbyResumeCapability();
    std::string GetCustomParam(const std::string &key);
    VideoFormat GetVideoFormatsByCea(int index);
    std::string Stringify() override;

    RtspError Parse(const std::string &response) override;
    WfdVideoFormatsInfo GetWfdVideoFormatsInfo() { return vWfdVideoFormatsInfo_[0]; }

private:
    std::list<std::pair<std::string, std::string>> params_;
    std::vector<WfdVideoFormatsInfo> vWfdVideoFormatsInfo_;
};

class WfdRtspM4Request : public RtspRequestParameter {
public:
    WfdRtspM4Request() = default;
    ~WfdRtspM4Request() override = default;
    WfdRtspM4Request(int32_t cseq, const std::string &url) : RtspRequestParameter(RTSP_METHOD_SET_PARAMETER, cseq, url)
    {
    }

    void SetClientRtpPorts(int32_t port);
    void SetAudioCodecs(AudioFormat format = AUDIO_48000_16_2);
    void SetPresentationUrl(const std::string &ip);
    void SetVideoFormats(const WfdVideoFormatsInfo &wfdVideoFormatsInfo, VideoFormat format = VIDEO_1920X1080_30);

    int32_t GetRtpPort();

    std::string GetPresentationUrl();
    std::string GetParameterValue(const std::string &param);

    RtspError Parse(const std::string &request) override;

private:
    std::list<std::pair<std::string, std::string>> params_;
};

using WfdRtspM4Response = RtspResponse;

// WfdRtspM5Request
class WfdRtspM5Request : public RtspRequestParameter {
public:
    WfdRtspM5Request() = default;
    explicit WfdRtspM5Request(int32_t cseq) : RtspRequestParameter(RTSP_METHOD_SET_PARAMETER,
        cseq, WFD_RTSP_URL_DEFAULT) {}

    void SetTriggerMethod(const std::string &method);
    std::string GetTriggerMethod();
};

using WfdRtspM5Response = RtspResponse;
using WfdRtspM6Request = RtspRequestSetup;

// WfdRtspM6Response
class WfdRtspM6Response : public RtspResponse {
public:
    WfdRtspM6Response() = default;
    WfdRtspM6Response(int32_t cseq, int32_t status, std::string sessionID, int32_t timeOut) : RtspResponse(cseq, status)
    {
        SetSession(sessionID);
        SetTimeout(timeOut);
    }
    std::string StringifyEx();
    int32_t GetClientPort();
    int32_t GetServerPort();
    void SetClientPort(int port);
    void SetServerPort(int port);

private:
    int32_t clientPort_ = 0;
    int32_t serverPort_ = 0;
};

using WfdRtspM7Request = RtspRequestPlay;
using WfdRtspM8Request = RtspRequestTeardown;

class WfdRtspIDRRequest : public RtspRequestParameter {
public:
    WfdRtspIDRRequest() = default;
    explicit WfdRtspIDRRequest(int32_t cseq, const std::string &url)
        : RtspRequestParameter(RTSP_METHOD_SET_PARAMETER, cseq, url)
    {
        AddBodyItem(WFD_PARAM_IDR_REQUEST);
    }
};

// WfdRtspM7Response
class WfdRtspM7Response : public RtspResponse {
public:
    WfdRtspM7Response() = default;
    WfdRtspM7Response(int32_t cseq, int32_t status, std::string sessionID) : RtspResponse(cseq, status)
    {
        SetSession(sessionID);
    }
    std::string StringifyEx();
};

using WfdRtspM8Response = RtspResponse;

class WfdRtspM16Request : public RtspRequestParameter {
public:
    WfdRtspM16Request(int32_t cseq, const std::string &url, std::string sessionID)
        : RtspRequestParameter(RTSP_METHOD_GET_PARAMETER, cseq, url)
    {
        SetSession(sessionID);
    }
};

using WfdRtspM16Response = RtspResponse;

} // namespace Sharing
} // namespace OHOS
#endif
