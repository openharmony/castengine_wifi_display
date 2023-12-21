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

#include "rtsp_sdp.h"
#include <math.h>
#include "rtsp_common.h"
#include "utils/base64.h"

namespace OHOS {
namespace Sharing {
bool SessionOrigin::Parse(const std::string &origin)
{
    std::regex pattern("([a-zA-Z0-9-]+) ([a-zA-Z0-9-]+) ([a-zA-Z0-9-]+) (IN) (IP4|IP6) ([0-9a-f.:]+)");
    std::smatch sm;
    if (!std::regex_search(origin, sm, pattern)) {
        return false;
    }

    username = sm[1].str();
    sessionId = sm[2].str();
    sessionVersion = atoi(sm[3].str().c_str());
    netType = sm[4].str();
    addrType = sm[5].str();
    unicastAddr = sm[6].str();
    return true;
}

bool MediaLine::Parse(const std::string &mediaLine)
{
    std::regex pattern("^(video|audio|text|application|message) ([0-9]+)(/[0-9]+)? "
                       "(udp|RTP/AVP|RTP/SAVP|RTP/SAVPF) ([0-9]+)");
    std::smatch sm;
    if (!std::regex_search(mediaLine, sm, pattern)) {
        return false;
    }
    assert(sm.size() == 6);
    mediaType = sm[1].str();
    port = atoi(sm[2].str().c_str());
    protoType = sm[4].str();
    fmt = atoi(sm[5].str().c_str());
    return true;
}

std::string MediaDescription::GetTrackId() const
{
    for (auto &a : attributes_) {
        auto index = a.find("control:");
        if (index != std::string::npos) {
            return a.substr(8);
        }
    }

    return {};
}

std::string MediaDescription::GetRtpMap() const
{
    for (auto &a : attributes_) {
        auto index = a.find("rtpmap:");
        if (index != std::string::npos) {
            return a.substr(7);
        }
    }

    return {};
}

std::vector<uint8_t> MediaDescription::GetVideoSps()
{
    if (media_.mediaType != "video") {
        return {};
    }

    if (sps_.empty()) {
        ParseSpsPps();
    }

    return sps_;
}

std::vector<uint8_t> MediaDescription::GetVideoPps()
{
    if (media_.mediaType != "video") {
        return {};
    }

    if (pps_.empty()) {
        ParseSpsPps();
    }

    return pps_;
}

int32_t MediaDescription::GetUe(const uint8_t *buf, uint32_t nLen, uint32_t &pos)
{
    uint32_t nZeroNum = 0;
    while (pos < nLen * 8) {
        if (buf[pos / 8] & (0x80 >> (pos % 8))) {
            break;
        }
        nZeroNum++;
        pos++;
    }
    pos++;

    uint64_t dwRet = 0;
    for (uint32_t i = 0; i < nZeroNum; i++) {
        dwRet <<= 1;
        if (buf[pos / 8] & (0x80 >> (pos % 8))) {
            dwRet += 1;
        }
        pos++;
    }

    return (int32_t)((1 << nZeroNum) - 1 + dwRet);
}

int32_t MediaDescription::GetSe(uint8_t *buf, uint32_t nLen, uint32_t &pos)
{
    int32_t UeVal = GetUe(buf, nLen, pos);
    double k = UeVal;
    int32_t nValue = ceil(k / 2);
    if (UeVal % 2 == 0) {
        nValue = -nValue;
    }

    return nValue;
}

int32_t MediaDescription::GetU(uint8_t bitCount, const uint8_t *buf, uint32_t &pos)
{
    int32_t value = 0;
    for (uint32_t i = 0; i < bitCount; i++) {
        value <<= 1;
        if (buf[pos / 8] & (0x80 >> (pos % 8))) {
            value += 1;
        }
        pos++;
    }

    return value;
}

void MediaDescription::ExtractNaluRbsp(uint8_t *buf, uint32_t *bufSize)
{
    uint8_t *tmpPtr = buf;
    uint32_t tmpBufSize = *bufSize;
    for (uint32_t i = 0; i < (tmpBufSize - 2); i++) {
        if (!tmpPtr[i] && !tmpPtr[i + 1] && tmpPtr[i + 2] == 3) {
            for (uint32_t j = i + 2; j < tmpBufSize - 1; j++) {
                tmpPtr[j] = tmpPtr[j + 1];
            }
            (*bufSize)--;
        }
    }
}

std::pair<int32_t, int32_t> MediaDescription::GetVideoSize()
{
    auto sps = GetVideoSps();
    int32_t width = 0;
    int32_t height = 0;
    uint8_t *buf = sps.data();
    uint32_t nLen = sps.size();
    uint32_t cursor = 0;
    if (sps.size() < 10) {
        return {width, height};
    }

    ExtractNaluRbsp(buf, &nLen);
    // forbidden_zero_bit
    GetU(1, buf, cursor);
    // nal_ref_idc
    GetU(2, buf, cursor);
    int32_t nalUnitType = GetU(5, buf, cursor);
    if (nalUnitType != 7) {
        return {width, height};
    }

    int32_t profileIdc = GetU(8, buf, cursor);
    // constraint_set0_flag
    GetU(1, buf, cursor);
    // constraint_set1_flag
    GetU(1, buf, cursor);
    // constraint_set2_flag
    GetU(1, buf, cursor);
    // constraint_set3_flag
    GetU(1, buf, cursor);
    // constraint_set4_flag
    GetU(1, buf, cursor);

    // constraint_set5_flag
    GetU(1, buf, cursor);
    // reserved_zero_2bits
    GetU(2, buf, cursor);
    // level_idc
    GetU(8, buf, cursor);
    // seq_parameter_set_id
    GetUe(buf, nLen, cursor);

    if (profileIdc == 100 || profileIdc == 110 || profileIdc == 122 || profileIdc == 244 || profileIdc == 44 ||
        profileIdc == 83 || profileIdc == 86 || profileIdc == 118 || profileIdc == 128 || profileIdc == 138 ||
        profileIdc == 139 || profileIdc == 134 || profileIdc == 135) {
        int32_t chromaFormatIdc = GetUe(buf, nLen, cursor);
        if (chromaFormatIdc == 3) {
            // separate_colour_plane_flag
            GetU(1, buf, cursor);
        }
        // bit_depth_luma_minus8
        GetUe(buf, nLen, cursor);
        // bit_depth_chroma_minus8
        GetUe(buf, nLen, cursor);
        // qpprime_y_zero_transform_bypass_flag
        GetU(1, buf, cursor);
        int32_t seqScalingMatrixPresentFlag = GetU(1, buf, cursor);

        int32_t seqScalingListPresentFlag[12];
        if (seqScalingMatrixPresentFlag) {
            int32_t lastScale = 8;
            int32_t nextScale = 8;
            int32_t sizeOfScalingList;
            for (int32_t i = 0; i < ((chromaFormatIdc != 3) ? 8 : 12); i++) {
                seqScalingListPresentFlag[i] = GetU(1, buf, cursor);
                if (seqScalingListPresentFlag[i]) {
                    lastScale = 8;
                    nextScale = 8;
                    sizeOfScalingList = i < 6 ? 16 : 64;
                    for (int32_t j = 0; j < sizeOfScalingList; j++) {
                        if (nextScale != 0) {
                            int32_t deltaScale = GetSe(buf, nLen, cursor);
                            nextScale = (lastScale + deltaScale) & 0xff;
                        }
                        lastScale = nextScale == 0 ? lastScale : nextScale;
                    }
                }
            }
        }
    }
    // log2_max_frame_num_minus4
    GetUe(buf, nLen, cursor);
    int32_t picOrderCntType = GetUe(buf, nLen, cursor);
    if (picOrderCntType == 0) {
        // log2_max_pic_order_cnt_lsb_minus4
        GetUe(buf, nLen, cursor);
    } else if (picOrderCntType == 1) {
        // delta_pic_order_always_zero_flag
        GetU(1, buf, cursor);
        // offset_for_non_ref_pic
        GetSe(buf, nLen, cursor);
        // offset_for_top_to_bottom_field
        GetSe(buf, nLen, cursor);
        int32_t numRefFramesInPicOrderCntCycle = GetUe(buf, nLen, cursor);

        int32_t *offsetForRefFrame = new int32_t[numRefFramesInPicOrderCntCycle];
        for (int32_t i = 0; i < numRefFramesInPicOrderCntCycle; i++) {
            offsetForRefFrame[i] = GetSe(buf, nLen, cursor);
        }
        delete[] offsetForRefFrame;
    }
    // max_num_ref_frames =
    GetUe(buf, nLen, cursor);
    // gaps_in_frame_num_value_allowed_flag =
    GetU(1, buf, cursor);
    int32_t picWidthMinMbsMinus1 = GetUe(buf, nLen, cursor);
    int32_t picHeightInMapUnitsMinus1 = GetUe(buf, nLen, cursor);

    width = (picWidthMinMbsMinus1 + 1) * 16;
    height = (picHeightInMapUnitsMinus1 + 1) * 16;

    int32_t frameMbsOnlyFlag = GetU(1, buf, cursor);
    if (!frameMbsOnlyFlag) {
        // mb_adaptive_frame_field_flag
        GetU(1, buf, cursor);
    }

    // direct_8x8_inference_flag
    GetU(1, buf, cursor);
    int32_t frameCroppingFlag = GetU(1, buf, cursor);
    if (frameCroppingFlag) {
        int32_t frameCropLeftOffset = GetUe(buf, nLen, cursor);
        int32_t frameCropRightOffset = GetUe(buf, nLen, cursor);
        int32_t frameCropTopOffset = GetUe(buf, nLen, cursor);
        int32_t frameCropBottomOffset = GetUe(buf, nLen, cursor);

        int32_t cropUnitX = 2;
        int32_t cropUnitY = 2 * (2 - frameMbsOnlyFlag);
        width -= cropUnitX * (frameCropLeftOffset + frameCropRightOffset);
        height -= cropUnitY * (frameCropTopOffset + frameCropBottomOffset);
    }

    return {width, height};
}

bool MediaDescription::ParseSpsPps()
{
    for (auto &a : attributes_) {
        auto index = a.find("sprop-parameter-sets=");
        if (index != std::string::npos) {
            std::string sps_pps = a.substr(index + 21);

            index = sps_pps.find(',');
            if (index != std::string::npos) {
                auto spsBase64 = sps_pps.substr(0, index);
                auto ppsBase64 = sps_pps.substr(index + 1);

                uint8_t *spsBuffer = (uint8_t *)malloc(spsBase64.size() * 3 / 4 + 1);
                memset(spsBuffer, 0, spsBase64.size() * 3 / 4 + 1);
                uint32_t spsLength = Base64::Decode(spsBase64.c_str(), spsBase64.size(), spsBuffer);
                sps_.reserve(spsLength);
                for (uint32_t i = 0; i < spsLength; ++i) {
                    sps_.push_back(spsBuffer[i]);
                }

                uint8_t *ppsBuffer = (uint8_t *)malloc(ppsBase64.size() * 3 / 4 + 1);
                memset(ppsBuffer, 0, ppsBase64.size() * 3 / 4 + 1);
                uint32_t ppsLength = Base64::Decode(ppsBase64.c_str(), ppsBase64.size(), ppsBuffer);
                sps_.reserve(ppsLength);
                for (uint32_t i = 0; i < ppsLength; ++i) {
                    pps_.push_back(ppsBuffer[i]);
                }
            }
        }
    }

    return true;
}

int32_t MediaDescription::GetAudioSamplingRate() const
{
    if (media_.mediaType != "audio") {
        return 0;
    }
    std::string rtpMap = GetRtpMap();
    std::regex pattern("([0-9]+) ([a-zA-Z0-9-]+)/([0-9]+)/([1-9]{1})");
    std::smatch sm;
    if (!std::regex_search(rtpMap, sm, pattern)) {
        return false;
    }

    return atoi(sm[3].str().c_str());
}

int32_t MediaDescription::GetAudioChannels() const
{
    if (media_.mediaType != "audio") {
        return 0;
    }
    std::string rtpMap = GetRtpMap();
    std::regex pattern("([0-9]+) ([a-zA-Z0-9-]+)/([0-9]+)/([1-9]{1})");
    std::smatch sm;
    if (!std::regex_search(rtpMap, sm, pattern)) {
        return false;
    }

    return atoi(sm[4].str().c_str());
}

std::string MediaDescription::GetAudioConfig() const
{
    if (media_.mediaType != "audio") {
        return {};
    }

    for (auto &a : attributes_) {
        auto index = a.find("config=");
        if (index != std::string::npos) {
            std::string config = a.substr(index + 7);
            auto semicolon = config.find(';');
            if (semicolon != std::string::npos) {
                config = config.substr(0, semicolon);
            }

            return config;
        }
    }

    return {};
}

bool RtspSdp::Parse(const std::string &sdpStr)
{
    auto bodyVec = RtspCommon::Split(sdpStr, RTSP_CRLF);
    std::list<std::string> lines;
    for (auto &item : bodyVec) {
        if (!item.empty()) {
            lines.emplace_back(item);
        }
    }

    return Parse(lines);
}

bool RtspSdp::Parse(const std::list<std::string> &sdpLines)
{
    std::shared_ptr<MediaDescription> track = nullptr;

    for (auto &line : sdpLines) {
        if (line.size() < 3 || line[1] != '=') {
            continue;
        }

        char key = line[0];
        std::string value = line.substr(2);
        switch (key) {
            case 'v':
                session_.version = atoi(value.c_str());
                break;
            case 'o':
                session_.origin.Parse(value);
                break;
            case 's':
                session_.name = value;
                break;
            case 'i':
                if (!track) {
                    session_.info = value;
                } else {
                    track->title_ = value;
                }
                break;
            case 'u':
                session_.uri = value;
                break;
            case 'e':
                session_.email = value;
                break;
            case 'p':
                session_.phone = value;
                break;
            case 't': {
                std::regex match("([0-9]+) ([0-9]+)");
                std::smatch sm;
                if (std::regex_search(value, sm, match)) {
                    session_.time.time.first = atol(sm[1].str().c_str());
                    session_.time.time.second = atol(sm[2].str().c_str());
                }
                break;
            }
            case 'r':
                session_.time.repeat.push_back(value);
                break;
            case 'z':
                session_.time.zone = value;
                break;
            case 'a':
                if (!track) {
                    session_.attributes.push_back(value);
                } else {
                    track->attributes_.push_back(value);
                }
                break;
            case 'm': {
                auto mediaTrack = std::make_shared<MediaDescription>();
                mediaTrack->media_.Parse(value);
                media_.emplace_back(mediaTrack);
                track = media_[media_.size() - 1];
                break;
            }
            case 'c': {
                if (!track) {
                    session_.connection = value;
                } else {
                    track->connection_ = value;
                }
                break;
            }
            case 'b': {
                if (!track) {
                    session_.bandwidth.push_back(value);
                } else {
                    track->bandwidth_.push_back(value);
                }
                break;
            }
            default:
                break;
        }
    }

    return true;
}

std::shared_ptr<MediaDescription> RtspSdp::GetVideoTrack()
{
    for (auto &track : media_) {
        if (track->media_.mediaType == "video") {
            return track;
        }
    }

    return nullptr;
}

std::shared_ptr<MediaDescription> RtspSdp::GetAudioTrack()
{
    for (auto &track : media_) {
        if (track->media_.mediaType == "audio") {
            return track;
        }
    }

    return nullptr;
}
} // namespace Sharing
} // namespace OHOS