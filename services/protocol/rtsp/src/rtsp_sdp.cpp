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
#include <cmath>
#include <securec.h>
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
    sessionId = sm[2].str();                    // 2:byte offset
    sessionVersion = atoi(sm[3].str().c_str()); // 3:byte offset
    netType = sm[4].str();                      // 4:byte offset
    addrType = sm[5].str();                     // 5:byte offset
    unicastAddr = sm[6].str();                  // 6:byte offset
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
    if (sm.size() != 6) { // 6:fixed length
        return false;
    }
    mediaType = sm[1].str();
    port = atoi(sm[2].str().c_str()); // 2:byte offset
    protoType = sm[4].str();          // 4:byte offset
    fmt = atoi(sm[5].str().c_str());  // 5:byte offset
    return true;
}

std::string MediaDescription::GetTrackId() const
{
    for (auto &a : attributes_) {
        auto index = a.find("control:");
        if (index != std::string::npos) {
            return a.substr(8); // 8:fixed length
        }
    }

    return {};
}

std::string MediaDescription::GetRtpMap() const
{
    for (auto &a : attributes_) {
        auto index = a.find("rtpmap:");
        if (index != std::string::npos) {
            return a.substr(7); // 7:fixed length
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
    while (pos < nLen * 8) {                      // 8:unit
        if (buf[pos / 8] & (0x80 >> (pos % 8))) { // 8:unit
            break;
        }
        nZeroNum++;
        pos++;
    }
    pos++;

    uint64_t dwRet = 0;
    for (uint32_t i = 0; i < nZeroNum; i++) {
        dwRet <<= 1;
        if (buf[pos / 8] & (0x80 >> (pos % 8))) { // 8:unit
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
    int32_t nValue = ceil(k / 2); // 2:unit
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
        if (buf[pos / 8] & (0x80 >> (pos % 8))) { // 8:unit
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
    for (uint32_t i = 0; i < (tmpBufSize - 2); i++) {             // 2:unit
        if (!tmpPtr[i] && !tmpPtr[i + 1] && tmpPtr[i + 2] == 3) { // 2:unit, 3:unit
            for (uint32_t j = i + 2; j < tmpBufSize - 1; j++) {   // 2:unit
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
    if (sps.size() < 10) { // 10:fixed length
        return {width, height};
    }

    ExtractNaluRbsp(buf, &nLen);
    // forbidden_zero_bit
    GetU(1, buf, cursor);
    // nal_ref_idc
    GetU(2, buf, cursor);                       // 2:fixed size
    int32_t nalUnitType = GetU(5, buf, cursor); // 5:fixed size
    if (nalUnitType != 7) {                     // 7:fixed size
        return {width, height};
    }

    int32_t profileIdc = GetU(8, buf, cursor); // 8:fixed size
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
    GetU(2, buf, cursor); // 2:fixed size
    // level_idc
    GetU(8, buf, cursor); // 8:fixed size
    // seq_parameter_set_id
    GetUe(buf, nLen, cursor);

    if (profileIdc == 100       // 100:profile
        || profileIdc == 110    // 110:profile
        || profileIdc == 122    // 122:profile
        || profileIdc == 244    // 244:profile
        || profileIdc == 44     // 44:profile
        || profileIdc == 83     // 83:profile
        || profileIdc == 86     // 86:profile
        || profileIdc == 118    // 118:profile
        || profileIdc == 128    // 128:profile
        || profileIdc == 138    // 138:profile
        || profileIdc == 139    // 139:profile
        || profileIdc == 134    // 134:profile
        || profileIdc == 135) { // 135:profile
        int32_t chromaFormatIdc = GetUe(buf, nLen, cursor);
        if (chromaFormatIdc == 3) { // 3:format
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

        int32_t seqScalingListPresentFlag[12]; // 12:fixed size
        if (seqScalingMatrixPresentFlag) {
            int32_t lastScale = 8; // 8:fixed size
            int32_t nextScale = 8; // 8:fixed size
            int32_t sizeOfScalingList;
            for (int32_t i = 0; i < ((chromaFormatIdc != 3) ? 8 : 12); i++) { // 3:format, 8:fixed size, 12:fixed size
                seqScalingListPresentFlag[i] = GetU(1, buf, cursor);
                if (seqScalingListPresentFlag[i]) {
                    lastScale = 8;                       // 8:fixed size
                    nextScale = 8;                       // 8:fixed size
                    sizeOfScalingList = i < 6 ? 16 : 64; // 6:fixed size, 16:fixed size, 64:fixed size
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
        if (numRefFramesInPicOrderCntCycle == 0 || numRefFramesInPicOrderCntCycle < 0) {
            return {width, height};
        }
        int32_t *offsetForRefFrame = new int32_t[numRefFramesInPicOrderCntCycle];
        if (offsetForRefFrame == nullptr) {
            for (int32_t i = 0; i < numRefFramesInPicOrderCntCycle; i++) {
                GetSe(buf, nLen, cursor);
            }
        } else {
            for (int32_t i = 0; i < numRefFramesInPicOrderCntCycle; i++) {
                offsetForRefFrame[i] = GetSe(buf, nLen, cursor);
            }
        }

        delete[] offsetForRefFrame;
    }
    // max_num_ref_frames =
    GetUe(buf, nLen, cursor);
    // gaps_in_frame_num_value_allowed_flag =
    GetU(1, buf, cursor);
    int32_t picWidthMinMbsMinus1 = GetUe(buf, nLen, cursor);
    int32_t picHeightInMapUnitsMinus1 = GetUe(buf, nLen, cursor);

    width = (picWidthMinMbsMinus1 + 1) * 16;       // 16:fixed size
    height = (picHeightInMapUnitsMinus1 + 1) * 16; // 16:fixed size

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

        int32_t cropUnitX = 2;                          // 2:fixed size
        int32_t cropUnitY = 2 * (2 - frameMbsOnlyFlag); // 2:fixed size
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
            std::string sps_pps = a.substr(index + 21); // 21:fixed size

            index = sps_pps.find(',');
            if (index != std::string::npos) {
                auto spsBase64 = sps_pps.substr(0, index);
                auto ppsBase64 = sps_pps.substr(index + 1);

                uint8_t *spsBuffer = (uint8_t *)malloc(spsBase64.size() * 3 / 4 + 1); // 3:fixed size, 4:fixed size
                memset_s(spsBuffer, spsBase64.size() * 3 / 4 + 1, 0,
                         spsBase64.size() * 3 / 4 + 1); // 3:fixed size, 4:fixed size
                uint32_t spsLength = Base64::Decode(spsBase64.c_str(), spsBase64.size(), spsBuffer);
                sps_.reserve(spsLength);
                for (uint32_t i = 0; i < spsLength; ++i) {
                    sps_.push_back(spsBuffer[i]);
                }

                uint8_t *ppsBuffer = (uint8_t *)malloc(ppsBase64.size() * 3 / 4 + 1); // 3:fixed size, 4:fixed size
                memset_s(ppsBuffer, ppsBase64.size() * 3 / 4 + 1, 0, ppsBase64.size() * 3 / 4 + 1);
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

    return atoi(sm[3].str().c_str()); // 3:fixed size
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

    return atoi(sm[4].str().c_str()); // 4:fixed size
}

std::string MediaDescription::GetAudioConfig() const
{
    if (media_.mediaType != "audio") {
        return {};
    }

    for (auto &a : attributes_) {
        auto index = a.find("config=");
        if (index != std::string::npos) {
            std::string config = a.substr(index + 7); // 7:fixed size
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
        if (line.size() < 3 || line[1] != '=') { // 3:fixed size
            continue;
        }

        char key = line[0];
        std::string value = line.substr(2); // 2:fixed size
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
                    session_.time.time.second = atol(sm[2].str().c_str()); // 2:fixed size
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