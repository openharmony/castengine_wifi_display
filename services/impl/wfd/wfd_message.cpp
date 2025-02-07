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
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <securec.h>
#include <sstream>
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t BIT_OFFSET_TWO = 2;
constexpr int32_t BIT_OFFSET_THREE = 3;
constexpr int32_t BIT_OFFSET_FOUR = 4;
constexpr int32_t BIT_OFFSET_EIGHT = 8;
constexpr int32_t BIT_OFFSET_TWELVE = 12;

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
    if (format == VIDEO_NONE) {
        ceaResolutionIndex =
            GetSupportVideoResolution(VIDEO_R1_RESOLUTION_SIZE, std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));
    } else {
        ceaResolutionIndex = GetVideoFormatResolution(format);
        native = static_cast<uint8_t>(WfdResolutionType::RESOLUTION_CEA) | (ceaResolutionIndex << BIT_OFFSET_THREE);
    }
    if (ceaResolutionIndex == 0) {
        ceaResolutionIndex = (1 << CEA_640_480_P60);
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

uint32_t WfdRtspM3Response::GetVideoFormatResolution(VideoFormat format)
{
    uint32_t ceaResolutionIndex = 0;
    switch (format) {
        case VIDEO_1920X1080_60:
            ceaResolutionIndex = CEA_1920_1080_P60;
            break;
        case VIDEO_1920X1080_30:
            ceaResolutionIndex = CEA_1920_1080_P30;
            break;
        case VIDEO_1920X1080_25:
            ceaResolutionIndex = CEA_1920_1080_P25;
            break;
        case VIDEO_1280X720_60:
            ceaResolutionIndex = CEA_1280_720_P60;
            break;
        case VIDEO_1280X720_30:
            ceaResolutionIndex = CEA_1280_720_P30;
            break;
        case VIDEO_1280X720_25:
            ceaResolutionIndex = CEA_1280_720_P25;
            break;
        case VIDEO_640X480_60:
        default:
            ceaResolutionIndex = CEA_640_480_P60;
            break;
    }

    std::shared_ptr<MediaAVCodec::AVCodecList> avCodecList = MediaAVCodec::AVCodecListFactory::CreateAVCodecList();
    if (avCodecList != nullptr) {
        MediaAVCodec::CapabilityData *capData = avCodecList->GetCapability(
            std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC), false, MediaAVCodec::AVCodecCategory::AVCODEC_NONE);
        std::shared_ptr<MediaAVCodec::VideoCaps> codecInfo = std::make_shared<MediaAVCodec::VideoCaps>(capData);
        ceaResolutionIndex =
            IsVideoResolutionSupport(codecInfo, ceaResolutionIndex) ? ceaResolutionIndex : CEA_640_480_P60;
    }
    return (1 << ceaResolutionIndex);
}

uint32_t WfdRtspM3Response::GetSupportVideoResolution(int32_t maxIndex, std::string mimeType)
{
    uint32_t ceaResolutionIndex = 0;
    std::shared_ptr<MediaAVCodec::AVCodecList> avCodecList = MediaAVCodec::AVCodecListFactory::CreateAVCodecList();
    if (avCodecList != nullptr) {
        MediaAVCodec::CapabilityData *capData =
            avCodecList->GetCapability(mimeType, false, MediaAVCodec::AVCodecCategory::AVCODEC_NONE);
        std::shared_ptr<MediaAVCodec::VideoCaps> codecInfo = std::make_shared<MediaAVCodec::VideoCaps>(capData);
        for (int32_t i = 0; i < maxIndex; i++) {
            if (IsVideoResolutionSupport(codecInfo, i)) {
                ceaResolutionIndex = ceaResolutionIndex | (1 << i);
            }
        }
    } else {
        ceaResolutionIndex = (1 << CEA_640_480_P60);
    }
    return ceaResolutionIndex;
}

bool WfdRtspM3Response::IsVideoResolutionSupport(std::shared_ptr<MediaAVCodec::VideoCaps> codecInfo, uint32_t index)
{
    if (codecInfo == nullptr) {
        SHARING_LOGE("codecInfo is null");
        return false;
    }
    if (index < 0 || index >= VIDEO_RESOLUTION_TABLE_LENGTH) {
        SHARING_LOGE("invalid index");
        return false;
    }
    const VideoConfigTable *config = &VIDEO_RESOLUTION_TABLE[TYPE_CEA][index];
    if (config == nullptr || config->width == 0) {
        return false;
    }
    return codecInfo->IsSizeAndRateSupported(config->width, config->height, config->frameRate);
}

void WfdRtspM3Response::SetAudioCodecs(AudioFormat format)
{
    std::string audioCodec;
    if (format == AUDIO_NONE) {
        audioCodec = GetSupportAudioCodecList();
    } else {
        audioCodec = GetAudioFormat(format);
    }
    params_.emplace_back(WFD_PARAM_AUDIO_CODECS, audioCodec);
}

std::string WfdRtspM3Response::GetAudioFormat(AudioFormat format)
{
    std::stringstream ss;
    uint32_t type = TYPE_AAC;
    uint32_t mode;
    std::string codec = AAC;
    switch (format) {
        case AUDIO_44100_8_2:
            type = TYPE_LPCM;
            ss << "LPCM" << RTSP_SP << "00000001 00";
            break;
        case AUDIO_44100_16_2:
            type = TYPE_LPCM;
            ss << "LPCM" << RTSP_SP << "00000002 00";
            break;
        case AUDIO_48000_16_2:
            mode = AAC_48000_16_2;
            break;
        case AUDIO_48000_16_4:
            mode = AAC_48000_16_4;
            break;
        case AUDIO_48000_16_6:
            mode = AAC_48000_16_6;
            break;
        case AUDIO_48000_16_8:
            mode = AAC_48000_16_8;
            break;
        default:
            mode = AAC_48000_16_2;
            break;
    }
    if (type == TYPE_AAC) {
        std::shared_ptr<MediaAVCodec::AVCodecList> avCodecList = MediaAVCodec::AVCodecListFactory::CreateAVCodecList();
        MediaAVCodec::CapabilityData *capData = avCodecList->GetCapability(
            std::string(MediaAVCodec::CodecMimeType::AUDIO_AAC), false, MediaAVCodec::AVCodecCategory::AVCODEC_NONE);
        mode = IsSupportAudioModes(TYPE_AAC, mode, capData) ? mode : AAC_48000_16_2;
        mode = (1 << mode);
        ss << codec << RTSP_SP << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex << mode << RTSP_SP
           << "00";
    }
    return ss.str();
}

std::string WfdRtspM3Response::GetSupportAudioCodecList()
{
    std::shared_ptr<MediaAVCodec::AVCodecList> avCodecList = MediaAVCodec::AVCodecListFactory::CreateAVCodecList();
    if (avCodecList == nullptr) {
        return "";
    }
    const std::vector<std::string> coderName = {std::string(MediaAVCodec::CodecMimeType::AUDIO_AAC)};
    std::stringstream ss;
    bool isFirst = true;
    for (auto &coder : coderName) {
        MediaAVCodec::CapabilityData *capData =
            avCodecList->GetCapability(coder, false, MediaAVCodec::AVCodecCategory::AVCODEC_NONE);
        if (capData != nullptr) {
            int32_t audioCodecType;
            int32_t length;
            std::string codec;
            if (capData->codecName == MediaAVCodec::AVCodecCodecName::AUDIO_DECODER_AAC_NAME) {
                audioCodecType = TYPE_AAC;
                codec = AAC;
                length = AUDIO_AAC_MODE_LENGTH;
            } else {
                SHARING_LOGE("audioCodecType is unknown codecName = %{public}s", capData->codecName.c_str());
                continue;
            }
            std::string modes = GetSupportAudioModes(audioCodecType, capData, length);
            if (!isFirst) {
                ss << "," << RTSP_SP;
            }
            ss << codec << RTSP_SP << std::setfill('0') << std::setw(BIT_OFFSET_EIGHT) << std::hex
               << std::stoi(modes.c_str(), nullptr, BIT_OFFSET_TWO) << RTSP_SP << "00";
        }
        isFirst = false;
    }
    if (ss.str().empty()) {
        ss << "AAC" << RTSP_SP << "00000001 00";
    }
    return ss.str();
}

std::string WfdRtspM3Response::GetSupportAudioModes(int32_t type, MediaAVCodec::CapabilityData *capData, int32_t length)
{
    char audioModes[length + 1];
    audioModes[length] = '\0';
    memset_s(audioModes, length, '0', length);
    for (int32_t i = 0; i < length; i++) {
        if (IsSupportAudioModes(type, i, capData)) {
            audioModes[length - i - 1] = '1';
        }
    }
    return audioModes;
}

bool WfdRtspM3Response::IsSupportAudioModes(int32_t type, int32_t index, MediaAVCodec::CapabilityData *capData)
{
    const AudioConfigTable *config = &AUDIO_CONFIG_TABLE[type][index];
    if (config == nullptr || config->sampleRate == 0) {
        return false;
    }
    bool isChannelsSupport =
        config->channels >= capData->channels.minVal && config->channels <= capData->channels.maxVal;
    bool isSampleRateSupport = std::find(capData->sampleRate.begin(), capData->sampleRate.end(), config->sampleRate) !=
                               capData->sampleRate.end();
    return isChannelsSupport && isSampleRateSupport;
}

void WfdRtspM3Response::SetVideoFormats2FromSystem()
{
    uint32_t h264Profile = (1 << static_cast<uint32_t>(WfdH264Profile::PROFILE_CHP));
    uint32_t h264Level = (1 << static_cast<uint32_t>(WfdH264Level::LEVEL_51));
    uint32_t h264CodecType = (1 << static_cast<uint32_t>(WfdVideoCodec::CODEC_H264));
    uint64_t ceaResolutionIndex =
        GetSupportVideoResolution(VIDEO_R2_RESOLUTION_SIZE, std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << std::uppercase << 0 << RTSP_SP;
    std::string videoAvcCap = GetVideo2Cap(h264CodecType, h264Profile, h264Level, ceaResolutionIndex);
    ss << videoAvcCap << RTSP_SP << "00";

    params_.emplace_back(WFD_PARAM_VIDEO_FORMATS_2, ss.str());
}

std::string WfdRtspM3Response::GetVideo2Cap(uint32_t codecType, uint32_t profile, uint32_t level,
                                            uint64_t ceaResolution)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << std::uppercase << codecType << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWO) << std::hex << std::uppercase << profile << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_FOUR) << std::hex << std::uppercase << level << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWELVE) << std::hex << std::uppercase << ceaResolution << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWELVE) << std::hex << std::uppercase << 0 << RTSP_SP;
    ss << std::setfill('0') << std::setw(BIT_OFFSET_TWELVE) << std::hex << std::uppercase << 0 << RTSP_SP;
    ss << "00 0000 0000 00";
    return ss.str();
}

void WfdRtspM3Response::SetAudioCodec2FromSystem()
{
    std::string audioCodec = GetSupportAudioCodecList();
    params_.emplace_back(WFD_PARAM_AUDIO_CODECS_2, audioCodec);
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

AudioFormat WfdRtspM3Response::GetAudioCodecs(WfdAudioCodec &codec)
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
            if (codec.codecId != CODEC_AAC && audioFormat0 > 1) {
                codec.codecId = CODEC_PCM;
                codec.format = AUDIO_48000_16_2;
            }
        } else if (format == "AAC") { // AAC
            codec.codecId = CODEC_AAC;
            codec.format = AUDIO_48000_16_2;
        } else if (format == "AC3") { // AC3
            if (audioFormat0 == 1) {
            }
            continue;
        }
    }

    return codec.format;
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
            index = static_cast<int>(nPos);
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

void WfdRtspM4Request::SetAudioCodecs(WfdAudioCodec &codec)
{
    std::stringstream ss;
    ss << WFD_PARAM_AUDIO_CODECS << ":" << RTSP_SP;

    switch (codec.codecId) {
        case CODEC_PCM:
            ss << "LPCM" << RTSP_SP << "00000002 00";
            break;
        case CODEC_AAC:
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

void WfdRtspM4Request::GetVideoTrack(VideoTrack &videoTrack)
{
    std::string wfdVideoFormatParam = GetParameterValue(WFD_PARAM_VIDEO_FORMATS_2);
    if (wfdVideoFormatParam.empty()) {
        wfdVideoFormatParam = GetParameterValue(WFD_PARAM_VIDEO_FORMATS);
    }
    if (wfdVideoFormatParam.empty()) {
        SHARING_LOGE("wfdVideoFormatParam is null.");
        return;
    }
    std::vector<std::string> videoFormats = RtspCommon::Split(wfdVideoFormatParam, "\\s+");
    if (videoFormats.size() <= INDEX_HH) {
        SHARING_LOGE("video formats is invalid.");
        return;
    }
    SHARING_LOGD("wfdVideoFormats native:%{public}s", videoFormats.at(0).c_str());
    errno = 0;
    char *endPtr = nullptr;
    int nativeValue = std::strtol(videoFormats.front().c_str(), &endPtr, HEX_LENGTH);
    if (errno == ERANGE || endPtr == videoFormats.front().c_str() || *endPtr != '\0') {
        SHARING_LOGE("%{public}s error : %{public}s!", __FUNCTION__, strerror(errno));
        return;
    }
    
    std::string resolutionStr = {""};
    nativeValue &= 0x07;
    if (nativeValue == TYPE_CEA) {
        resolutionStr = videoFormats.at(INDEX_CEA);
    } else if (nativeValue == TYPE_VESA) {
        resolutionStr = videoFormats.at(INDEX_VESA);
    } else if (nativeValue == TYPE_HH) {
        resolutionStr = videoFormats.at(INDEX_HH);
    } else {
        SHARING_LOGE("get wfd_video_formats error.");
        return;
    }
    GetVideoResolution(videoTrack, resolutionStr, nativeValue);
}

void WfdRtspM4Request::GetVideoResolution(VideoTrack &videoTrack, std::string resolutionStr, int type)
{
    uint32_t result = strtol(resolutionStr.c_str(), nullptr, HEX_LENGTH);
    if (result <= 0) {
        SHARING_LOGE("get video resolution failed.");
        return;
    }
    uint32_t index = log(result) / log(LOG_BASE);
    if ((type == TYPE_CEA && index > MAX_CEA_INDEX) || (type == TYPE_VESA && index > MAX_VESA_INDEX) ||
        (type == TYPE_HH && index > MAX_HH_INDEX)) {
        SHARING_LOGE("get video resolution index error.");
    }
    const VideoConfigTable *config = &VIDEO_RESOLUTION_TABLE[type][index];
    videoTrack.codecId = CODEC_H264;
    videoTrack.width = config->width;
    videoTrack.height = config->height;
    videoTrack.frameRate = config->frameRate;
    SHARING_LOGI("get video resolution width: %{public}d, height: %{public}d, frameRate: %{public}d,", videoTrack.width,
                 videoTrack.height, videoTrack.frameRate);
}

void WfdRtspM4Request::GetAudioTrack(AudioTrack &audioTrack)
{
    std::string wfdAudioFormatParam = GetParameterValue(WFD_PARAM_AUDIO_CODECS_2);
    if (wfdAudioFormatParam.empty()) {
        wfdAudioFormatParam = GetParameterValue(WFD_PARAM_AUDIO_CODECS);
    }
    if (wfdAudioFormatParam.empty()) {
        SHARING_LOGE("wfdAudioFormatParam is null.");
        return;
    }
    std::vector<std::string> audioFormats = RtspCommon::Split(wfdAudioFormatParam, "\\s+");
    if (audioFormats.size() <= AUDIO_MODE_INDEX) {
        SHARING_LOGE("audioFormats is error.");
        return;
    }
    int32_t type;
    std::string format = audioFormats.at(AUDIO_TYPE_INDEX);
    if (format == LPCM) {
        type = TYPE_LPCM;
    } else if (format == AAC) {
        type = TYPE_AAC;
    } else if (format == AC3) {
        type = TYPE_AC3;
    } else {
        type = TYPE_AAC;
    }
    int32_t result = std::strtol(audioFormats.at(AUDIO_MODE_INDEX).c_str(), nullptr, HEX_LENGTH);
    if (result <= 0) {
        SHARING_LOGE("get mode failed.");
        return;
    }
    int32_t index = log(result) / log(LOG_BASE);
    const AudioConfigTable *config = &AUDIO_CONFIG_TABLE[type][index];
    audioTrack.codecId = CodecId::CODEC_AAC;
    audioTrack.sampleRate = config->sampleRate;
    audioTrack.sampleFormat = config->sampleFormat;
    audioTrack.channels = config->channels;
    SHARING_LOGI("get audio format sampleRate: %{public}d, sampleFormat: %{public}d, channels: %{public}d,",
                 audioTrack.sampleRate, audioTrack.sampleFormat, audioTrack.channels);
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
