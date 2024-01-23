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

#include "wfd_message.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t BIT_OFFSET_TWO = 2;
constexpr int32_t BIT_OFFSET_THREE = 3;
constexpr int32_t BIT_OFFSET_EIGHT = 8;

WfdRtspM1Response::WfdRtspM1Response(int32_t cseq, int32_t status) : RtspResponseOptions(cseq, status)
{
    std::stringstream ss;
    ss << RTSP_METHOD_WFD << "," << RTSP_SP << RTSP_METHOD_SET_PARAMETER << "," << RTSP_SP << RTSP_METHOD_GET_PARAMETER;
    SetPublicItems(ss.str());
}

RtspError WfdRtspM3Response::Parse(const std::string &response)
{
    auto res = RtspResponse::Parse(response);
    if (res.code != RtspErrorType::OK) {
        return res;
    }

    RtspCommon::SplitParameter(body_, params_);
    return {};
}

std::string WfdRtspM3Response::Stringify()
{
    std::stringstream body;
    std::stringstream ss;

    for (auto &param : params_) {
        body << param.first << ":" << RTSP_SP << param.second << RTSP_CRLF;
    }

    ss << RTSP_TOKEN_CONTENT_TYPE << ":" << RTSP_SP << "text/parameters" << RTSP_CRLF;
    ss << RTSP_TOKEN_CONTENT_LENGTH << ":" << RTSP_SP << body.str().length();

    ClearCustomHeader();
    AddCustomHeader(ss.str());

    if (body.str().empty()) {
        return RtspResponse::Stringify();
    }

    return RtspResponse::Stringify() + body.str();
}

void WfdRtspM3Response::SetVideoFormats(VideoFormat format)
{
    std::stringstream ss;
    std::stringstream sinkVideoList;

    uint8_t native = 0;
    uint8_t h264Profile = (1 << (uint8_t)WfdH264Profile::PROFILE_CHP);
    uint8_t h264Level = (1 << (uint8_t)WfdH264Level::LEVEL_42);
    uint32_t ceaResolutionIndex = 0;
    uint32_t vesaResolutionIndex = 0;
    uint32_t hhResolutionIndex = 0;

    switch (format) {
        case VIDEO_1920X1080_30:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1920_1080_P30 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1920_1080_P30);
            break;
        case VIDEO_1920X1080_25:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1920_1080_P25 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1920_1080_P25);
            break;
        case VIDEO_1280X720_30:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1280_720_P30 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1280_720_P30);
            break;
        case VIDEO_1280X720_25:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1280_720_P25 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1280_720_P25);
            break;
        case VIDEO_640X480_60:
        default:
            native =
                (uint8_t)WfdResolutionType::RESOLUTION_CEA | (WfdCeaResolution::CEA_640_480_P60 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_640_480_P60);
            break;
    }

    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << (int32_t)native << RTSP_SP << "00" << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << (int32_t)h264Profile << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << (int32_t)h264Level << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << ceaResolutionIndex << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << vesaResolutionIndex << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << hhResolutionIndex << RTSP_SP;

    ss << "00 0000 0000 00 none none";

    params_.emplace_back(WFD_PARAM_VIDEO_FORMATS, ss.str());
}

void WfdRtspM3Response::SetAudioCodecs(AudioFormat format)
{
    std::stringstream ss;
    switch (format) {
        case AUDIO_44100_8_2:
            ss << "LPCM" << RTSP_SP << "00000001 00";
            break;
        case AUDIO_44100_16_2:
            ss << "LPCM" << RTSP_SP << "00000002 00";
            break;
        case AUDIO_48000_16_2:
            ss << "AAC" << RTSP_SP << "00000001 00";
            break;
        default:
            ss << "AAC" << RTSP_SP << "00000001 00";
            break;
    }

    params_.emplace_back(WFD_PARAM_AUDIO_CODECS, ss.str());
}

void WfdRtspM3Response::SetClientRtpPorts(int32_t port)
{
    std::stringstream ss;
    ss << "RTP/AVP/UDP;unicast" << RTSP_SP << port << RTSP_SP << 0 << RTSP_SP << "mode=play";
    params_.emplace_back(WFD_PARAM_RTP_PORTS, ss.str());
}

void WfdRtspM3Response::SetContentProtection(const std::string &value)
{
    params_.emplace_back(WFD_PARAM_CONTENT_PROTECTION, value);
}

void WfdRtspM3Response::SetCoupledSink(const std::string &value)
{
    params_.emplace_back(WFD_PARAM_COUPLED_SINK, value);
}

void WfdRtspM3Response::SetUibcCapability(const std::string &value)
{
    params_.emplace_back(WFD_PARAM_UIBC_CAPABILITY, value);
}

void WfdRtspM3Response::SetStandbyResumeCapability(const std::string &value)
{
    params_.emplace_back(WFD_PARAM_UIBC_CAPABILITY, value);
}

void WfdRtspM3Response::SetCustomParam(const std::string &key, const std::string &value)
{
    params_.emplace_back(key, value);
}

int32_t WfdRtspM3Response::GetClientRtpPorts()
{
    int32_t port0 = -1;
    int32_t port1 = -1;
    std::string profile = "RTP/AVP/UDP;unicast";
    std::string mode = "mode=play";
    std::string value = GetCustomParam(WFD_PARAM_RTP_PORTS);
    std::stringstream ss(value);
    ss >> profile >> port0 >> port1 >> mode;
    return port0;
}

std::string WfdRtspM3Response::GetUibcCapability()
{
    std::string value = GetCustomParam(WFD_PARAM_UIBC_CAPABILITY);
    return value;
}

AudioFormat WfdRtspM3Response::GetAudioCodecs()
{
    std::string value = GetCustomParam(WFD_PARAM_AUDIO_CODECS);
    int32_t audioFormat0 = -1;
    int32_t audioFormat1 = -1;
    std::string format;
    auto audioCaps = RtspCommon::Split(value, ", ");
    for (size_t i = 0; i < audioCaps.size(); i++) {
        std::string audioCap = audioCaps[i];
        std::stringstream ss(audioCap);
        ss >> format >> audioFormat0 >> audioFormat1;
        if (format == "LPCM") { // LPCM
            continue;
        } else if (format == "AAC") { // AAC
            if (audioFormat0 == 1) {
                return AUDIO_48000_16_2;
            }
        } else if (format == "AC3") { // AC3
            if (audioFormat0 == 1) {
            }
            continue;
        }
    }
    return AUDIO_48000_8_2;
}

std::string WfdRtspM3Response::GetCoupledSink()
{
    std::string value = GetCustomParam(WFD_PARAM_COUPLED_SINK);
    return value;
}

std::string WfdRtspM3Response::GetContentProtection()
{
    std::string value = GetCustomParam(WFD_PARAM_CONTENT_PROTECTION);
    return value;
}

VideoFormat WfdRtspM3Response::GetVideoFormatsByCea(int index)
{
    WfdCeaResolution res = static_cast<WfdCeaResolution>(index);
    switch (res) {
        case CEA_640_480_P60:
            return VIDEO_640X480_60;
        case CEA_1280_720_P30:
            return VIDEO_1280X720_30;
        case CEA_1280_720_P60:
            return VIDEO_1280X720_60;
        case CEA_1920_1080_P25:
            return VIDEO_1920X1080_25;
        case CEA_1920_1080_P30:
            return VIDEO_1920X1080_30;
        case CEA_1920_1080_P60:
            return VIDEO_1920X1080_60;
        default:
            return VIDEO_640X480_60;
    }
}

VideoFormat WfdRtspM3Response::GetVideoFormats()
{
    // wfd_video_formats:
    // 1 byte "native"
    // 1 byte "preferred-display-mode-supported" 0 or 1
    // one or more avc codec structures
    //   1 byte profile
    //   1 byte level
    //   4 byte CEA mask
    //   4 byte VESA mask
    //   4 byte HH mask
    //   1 byte latency
    //   2 byte min-slice-slice
    //   2 byte slice-enc-params
    //   1 byte framerate-control-support
    //   max-hres (none or 2 byte)
    //   max-vres (none or 2 byte)
    /**
     * native:  2*2HEXDIG
     * preferred-display-mode-supported: 2*2HEXDIG; 0-not supported, 1-supported, 2-255 reserved
     * profile: 2*2HEXDIG, only one bit set
     * level:   2*2HEXDIG, only one bit set
     * CEA-Support: 8*8HEXDIG, 0-ignore
     * VESA-Support:8*8HEXDIG, 0-ignore
     * HH-Support:  8*8HEXDIG, 0-ignore
     * latency:     2*2HEXDIG, decoder latency in units of 5 msecs
     * min-slice-size: 4*4HEXDIG, number of macroblocks
     * slice-enc-params: 4*4HEXDIG,
     * frame-rate-control-support: 4*4HEXDIG
     * max-hres: 4*4HEXDIG, "none" if preferred-display-mode-supported is 0
     * max-vres: 4*4HEXDIG, "none" if preferred-display-mode-supported is 0
     **/
    std::string value = GetCustomParam(WFD_PARAM_VIDEO_FORMATS);
    if (value.size() < MINIMAL_VIDEO_FORMAT_SIZE) {
        return VIDEO_640X480_60;
    }

    std::string head = value.substr(0, 5); // 5: fix offset
    uint16_t native = 0;
    uint16_t preferredDisplayMode = 0;
    int index = 0;
    std::string temp = "";
    bool run = true;
    std::stringstream ss(head);
    ss >> std::hex >> native >> std::hex >> preferredDisplayMode;
    value = value.substr(5); // 5: fix offset

    while (run) {
        auto nPos = value.find_first_of(",", index + 1);
        if (nPos != std::string::npos) {
            index = nPos;
            temp = value.substr(0, index);
        } else {
            temp = value.substr(index + 1);
            run = false;
        }
        std::stringstream sss(temp);
        WfdVideoFormatsInfo wfdVideoFormatsInfo;
        wfdVideoFormatsInfo.native = native;
        wfdVideoFormatsInfo.preferredDisplayMode = preferredDisplayMode;
        sss >> std::hex >> wfdVideoFormatsInfo.profile >> std::hex >> wfdVideoFormatsInfo.level >> std::hex >>
            wfdVideoFormatsInfo.ceaMask >> std::hex >> wfdVideoFormatsInfo.veaMask >> std::hex >>
            wfdVideoFormatsInfo.hhMask >> std::hex >> wfdVideoFormatsInfo.latency >> std::hex >>
            wfdVideoFormatsInfo.minSlice >> std::hex >> wfdVideoFormatsInfo.sliceEncParam >> std::hex >>
            wfdVideoFormatsInfo.frameRateCtlSupport;
        vWfdVideoFormatsInfo_.push_back(wfdVideoFormatsInfo);
    }

    uint8_t tableSelection = vWfdVideoFormatsInfo_[0].native & 0x7;
    index = vWfdVideoFormatsInfo_[0].native >> BIT_OFFSET_THREE;
    switch (tableSelection) {
        case 0:
            return GetVideoFormatsByCea(index);
        default:
            return VIDEO_640X480_60;
    }
    return VIDEO_640X480_60;
}

std::string WfdRtspM3Response::GetStandbyResumeCapability()
{
    std::string value = GetCustomParam(WFD_PARAM_STANDBY_RESUME);
    return value;
}

std::string WfdRtspM3Response::GetCustomParam(const std::string &key)
{
    auto it = std::find_if(params_.begin(), params_.end(),
                           [=](std::pair<std::string, std::string> value) { return value.first == key; });
    if (it != params_.end()) {
        return it->second;
    }

    return "";
}

void WfdRtspM4Request::SetClientRtpPorts(int32_t port)
{
    std::stringstream ss;
    ss << WFD_PARAM_RTP_PORTS << ":" << RTSP_SP << "RTP/AVP/UDP;unicast" << RTSP_SP << port << RTSP_SP << 0 << RTSP_SP
       << "mode=play";
    AddBodyItem(ss.str());
}

void WfdRtspM4Request::SetAudioCodecs(AudioFormat format)
{
    std::stringstream ss;
    ss << WFD_PARAM_AUDIO_CODECS << ":" << RTSP_SP;
    switch (format) {
        case AUDIO_44100_8_2:
            ss << "LPCM" << RTSP_SP << "00000001 00";
            break;
        case AUDIO_44100_16_2:
            ss << "LPCM" << RTSP_SP << "00000002 00";
            break;
        case AUDIO_48000_16_2:
            ss << "AAC" << RTSP_SP << "00000001 00";
            break;
        default:
            ss << "AAC" << RTSP_SP << "00000001 00";
            break;
    }
    AddBodyItem(ss.str());
}

void WfdRtspM4Request::SetPresentationUrl(const std::string &ip)
{
    std::stringstream ss;
    ss << WFD_PARAM_PRESENTATION_URL << ":" << RTSP_SP << "rtsp://" << ip.c_str() << "/wfd1.0/streamid=0 none";
    AddBodyItem(ss.str());
}

void WfdRtspM4Request::SetVideoFormats(const WfdVideoFormatsInfo &wfdVideoFormatsInfo, VideoFormat format)
{
    std::stringstream ss;
    uint32_t native = wfdVideoFormatsInfo.native;
    uint32_t h264Profile = wfdVideoFormatsInfo.profile;
    uint32_t h264Level = wfdVideoFormatsInfo.level;
    uint32_t ceaResolutionIndex = 0;
    uint32_t vesaResolutionIndex = 0;
    uint32_t hhResolutionIndex = 0;
    (void)ceaResolutionIndex;
    (void)vesaResolutionIndex;
    (void)hhResolutionIndex;
    (void)h264Profile;
    (void)h264Level;

    switch (format) {
        case VIDEO_1920X1080_60:
        case VIDEO_1920X1080_30:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1920_1080_P30 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1920_1080_P30);
            break;
        case VIDEO_1920X1080_25:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1920_1080_P25 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1920_1080_P25);
            break;
        case VIDEO_1280X720_30:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1280_720_P30 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1280_720_P30);
            break;
        case VIDEO_1280X720_25:
            native = (uint8_t)WfdResolutionType::RESOLUTION_CEA | (CEA_1280_720_P25 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_1280_720_P25);
            break;
        case VIDEO_640X480_60:
        default:
            native =
                (uint8_t)WfdResolutionType::RESOLUTION_CEA | (WfdCeaResolution::CEA_640_480_P60 << BIT_OFFSET_THREE);
            ceaResolutionIndex = (1 << CEA_640_480_P60);
            break;
    }

    ss << WFD_PARAM_VIDEO_FORMATS << ":" << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << native << RTSP_SP << "00" << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << h264Profile << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << h264Level << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << ceaResolutionIndex << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << vesaResolutionIndex << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << hhResolutionIndex << RTSP_SP;
    ss << "00 0000 0000 00 none none";
    AddBodyItem(ss.str());
}

RtspError WfdRtspM4Request::Parse(const std::string &request)
{
    auto res = RtspRequest::Parse(request);
    if (res.code != RtspErrorType::OK) {
        return res;
    }

    RtspCommon::SplitParameter(body_, params_);
    return {};
}

std::string WfdRtspM4Request::GetParameterValue(const std::string &param)
{
    auto it = std::find_if(params_.begin(), params_.end(),
                           [=](std::pair<std::string, std::string> value) { return value.first == param; });
    if (it != params_.end()) {
        return it->second;
    }

    return "";
}

std::string WfdRtspM4Request::GetPresentationUrl()
{
    std::string urls = GetParameterValue(WFD_PARAM_PRESENTATION_URL);
    if (urls.empty()) {
        return "";
    }

    auto urlVec = RtspCommon::Split(urls, " ");
    if (!urlVec.empty()) {
        return urlVec[0];
    }

    return "";
}

int32_t WfdRtspM4Request::GetRtpPort()
{
    auto ports = GetParameterValue(WFD_PARAM_RTP_PORTS);
    auto resVec = RtspCommon::Split(ports, " ");
    if (resVec.size() > 2) { // 2: fix offset
        return atoi(resVec[1].c_str());
    }

    return 0;
}

void WfdRtspM5Request::SetTriggerMethod(const std::string &method)
{
    std::stringstream ss;
    ss << WFD_PARAM_TRIGGER << ":" << RTSP_SP << method;

    body_.emplace_back(ss.str());
}

std::string WfdRtspM5Request::GetTriggerMethod()
{
    std::list<std::pair<std::string, std::string>> params;
    RtspCommon::SplitParameter(body_, params);

    auto it = std::find_if(params.begin(), params.end(),
                           [](std::pair<std::string, std::string> value) { return value.first == WFD_PARAM_TRIGGER; });
    if (it != params.end()) {
        return it->second;
    }

    return {};
}

int32_t WfdRtspM6Response::GetClientPort()
{
    auto transport = GetToken(RTSP_TOKEN_TRANSPORT);
    if (transport.empty()) {
        return 0;
    }

    auto tsVec = RtspCommon::Split(transport, ";");
    for (auto &item : tsVec) {
        if (item.find("client_port=") != std::string::npos) {
            auto val = item.substr(item.find('=') + 1);
            return atoi(val.c_str());
        }
    }

    return 0;
}

int32_t WfdRtspM6Response::GetServerPort()
{
    auto transport = GetToken(RTSP_TOKEN_TRANSPORT);
    if (transport.empty()) {
        return 0;
    }

    auto tsVec = RtspCommon::Split(transport, ";");
    for (auto &item : tsVec) {
        if (item.find("server_port=") != std::string::npos) {
            auto val = item.substr(item.find('=') + 1);
            return atoi(val.c_str());
        }
    }

    return 0;
}

void WfdRtspM6Response::SetClientPort(int port)
{
    clientPort_ = port;
}

void WfdRtspM6Response::SetServerPort(int port)
{
    serverPort_ = port;
}

std::string WfdRtspM6Response::StringifyEx()
{
    std::stringstream ss;
    auto message = Stringify();
    std::string temp = RTSP_CRLF;
    auto nPos = message.find_last_of(RTSP_CRLF);
    if (nPos != std::string::npos) {
        message = message.substr(0, message.size() - temp.size());
    }
    ss << message << "Transport: RTP/AVP/UDP;unicast;client_port=" << clientPort_ << ";server_port=" << serverPort_
       << RTSP_CRLF;
    ss << RTSP_CRLF;
    return ss.str();
}

std::string WfdRtspM7Response::StringifyEx()
{
    std::stringstream ss;
    auto message = Stringify();
    std::string temp = RTSP_CRLF;
    auto nPos = message.find_last_of(RTSP_CRLF);
    if (nPos != std::string::npos) {
        message = message.substr(0, message.size() - temp.size());
    }
    ss << message << "Range: npt=now-" << RTSP_CRLF;
    ss << RTSP_CRLF;
    return ss.str();
}

} // namespace Sharing
} // namespace OHOS
