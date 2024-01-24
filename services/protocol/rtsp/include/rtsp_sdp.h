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

#ifndef OHOS_SHARING_RTSP_SDP_H
#define OHOS_SHARING_RTSP_SDP_H

#include <cstdint>
#include <list>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace OHOS {
namespace Sharing {

/// SDP: Session Description Protocol
/// [[RFC 8866](https://datatracker.ietf.org/doc/html/rfc8866)]

// o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
struct SessionOrigin {
    bool Parse(const std::string &origin);

    uint32_t sessionVersion;

    std::string netType;
    std::string username;
    std::string addrType;
    std::string sessionId;
    std::string unicastAddr;
};

struct SessionTime {
    // r=<repeat interval> <active duration> <offsets from start-time>
    std::vector<std::string> repeat;
    // t=<start-time> <stop-time>
    std::pair<uint64_t, uint64_t> time{UINT64_MAX, UINT64_MAX};
    // z=<adjustment time> <offset> <adjustment time> <offset> ...
    std::string zone;
};

struct SessionDescription {
    uint8_t version;        // v=
    std::string name;       // s=
    std::string info;       // i=
    std::string uri;        // u=
    std::string email;      // e=
    std::string phone;      // p=
    std::string connection; // c=

    SessionTime time;                    // time descriptions
    SessionOrigin origin;                // o=
    std::vector<std::string> bandwidth;  // b=
    std::vector<std::string> attributes; // a=
};

// m=<media_> <port> <proto> <fmt> ...
struct MediaLine {
    bool Parse(const std::string &mediaLine);

    uint16_t port;

    int32_t fmt;

    std::string mediaType;
    std::string protoType;
};

class MediaDescription {
public:
    friend class RtspSdp;

    std::string GetMediaType() const
    {
        return media_.mediaType;
    }

    int32_t GetPayloadType() const
    {
        return media_.fmt;
    }

    std::string GetRtpMap() const;
    std::string GetTrackId() const;

    std::vector<uint8_t> GetVideoSps();
    std::vector<uint8_t> GetVideoPps();
    std::pair<int32_t, int32_t> GetVideoSize();

    int32_t GetAudioChannels() const;
    int32_t GetAudioSamplingRate() const;
    std::string GetAudioConfig() const;

private:
    bool ParseSpsPps();

    int32_t GetSe(uint8_t *buf, uint32_t nLen, uint32_t &pos);
    int32_t GetUe(const uint8_t *buf, uint32_t nLen, uint32_t &pos);
    int32_t GetU(uint8_t bitCount, const uint8_t *buf, uint32_t &pos);

    void ExtractNaluRbsp(uint8_t *buf, uint32_t *bufSize);

private:
    std::string title_;                   // i=
    std::string connection_;              // c=
    std::vector<uint8_t> sps_;
    std::vector<uint8_t> pps_;
    std::vector<std::string> bandwidth_;  // b=
    std::vector<std::string> attributes_; // a=

    MediaLine media_;                     // m=
};

class RtspSdp {
public:
    bool Parse(const std::string &sdpStr);
    bool Parse(const std::list<std::string> &sdpLines);

    SessionOrigin GetOrigin() const
    {
        return session_.origin;
    }

    std::string GetName() const
    {
        return session_.name;
    }

    std::string GetInfo() const
    {
        return session_.info;
    }

    std::string GetUri() const
    {
        return session_.uri;
    }

    std::string GetEmail() const
    {
        return session_.email;
    }

    std::string GetPhone() const
    {
        return session_.phone;
    }

    std::string GetConnection() const
    {
        return session_.connection;
    }

    std::vector<std::string> GetBandwidth() const
    {
        return session_.bandwidth;
    }

    std::pair<uint64_t, uint64_t> GetTime() const
    {
        return session_.time.time;
    }

    std::vector<std::string> getAttributes() const
    {
        return session_.attributes;
    }

    std::shared_ptr<MediaDescription> GetVideoTrack();
    std::shared_ptr<MediaDescription> GetAudioTrack();

private:
    std::vector<std::shared_ptr<MediaDescription>> media_;

    SessionDescription session_;
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_SDP_H
