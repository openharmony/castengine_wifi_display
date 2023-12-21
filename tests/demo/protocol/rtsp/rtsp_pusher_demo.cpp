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

#include <condition_variable>
#include <memory>
#include <mutex>
#include <sstream>
#include "rtsp_client.h"
#include "sharing_log.h"

using namespace OHOS::Sharing;

int main()
{
    auto rtspClient = std::make_shared<RtspClient>();
    std::vector<SdpTrack::Ptr> medias;
    uint32_t trackIndex = 0;
    std::mutex mutex;
    std::condition_variable signal;
    rtspClient->SetUrl("rtsp://wowzaec2demo.streamlock.net:554/vod/mp4:BigBuckBunny_115k.mp4");
    rtspClient->SetOnRecvRtspPacket([&](const std::string &method, const RtspMessage &rsp) {
        if (method == "OPTION") {
            auto err = rtspClient->HandleOptionRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_UNAUTHED:
                    rtspClient->SendOption();
                    break;
                case RtspRspResult::RSP_OK: {
                    std::stringstream ss;
                    ss << "v=0"
                       << "\r\n";
                    ss << "o=- 0 0 IN IP4 127.0.0.1"
                       << "\r\n";
                    ss << "s=No Name"
                       << "\r\n";
                    ss << "c=IN IP4 192.168.159.128"
                       << "\r\n";
                    ss << "t=0 0"
                       << "\r\n";
                    ss << "a=tool:libavformat 59.17.101"
                       << "\r\n";
                    ss << "m=video 0 RTP/AVP 96"
                       << "\r\n";
                    ss << "a=rtpmap:96 H264/90000"
                       << "\r\n";
                    ss << "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z0LAKIyNQDwDTf+A0ADRgPCIRqA=,aM48gA==; profile-level-id=42C028"
                       << "\r\n";
                    ss << "a=control:streamid=0"
                       << "\r\n";
                    ss << "m=audio 0 RTP/AVP 97"
                       << "\r\n";
                    ss << "b=AS:128"
                       << "\r\n";
                    ss << "a=rtpmap:97 MPEG4-GENERIC/44100/2"
                       << "\r\n";
                    ss << "a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500"
                       << "\r\n";
                    ss << "a=control:streamid=1"
                       << "\r\n";
                    rtspClient->SetSdp(ss.str());
                    rtspClient->SendAnnounce();
                    break;
                }
                default: {
                    SHARING_LOGE("setOnRecvRtspPacket ret err");
                    signal.notify_one();
                    break;
                }
            }

        } else if (method == "ANNOUNCE") {
            auto err = rtspClient->HandleAnnounceRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_UNAUTHED:
                    rtspClient->SendAnnounce();
                    break;
                case RtspRspResult::RSP_OK: {
                    auto mediaTemp = rtspClient->GetAllMedia();
                    for (auto media : mediaTemp) {
                        medias.emplace_back(media);
                    }
                    rtspClient->SendSetup(trackIndex++);
                    break;
                }
                default: {
                    SHARING_LOGE("setOnRecvRtspPacket ret err");
                    signal.notify_one();
                    break;
                }
            }
        } else if (method == "SETUP") {
            rtspClient->HandleSetupRsp(rsp);
            auto err = rtspClient->HandleDescribeRsp(rsp);
            switch (err) {
                case RtspRspResult::RSP_OK:
                    if (trackIndex < medias.size() - 1) {
                        rtspClient->SendSetup(trackIndex++);
                    } else {
                        rtspClient->SendPlay();
                        trackIndex = 0;
                    }
                    break;
                default: {
                    SHARING_LOGE("setOnRecvRtspPacket ret err");
                    signal.notify_one();
                } break;
            }
        } else if (method == "RECORD") {
            rtspClient->HandlePlayRsp(rsp);
            signal.notify_one();
        }
    });
    rtspClient->SetExceptionCb([&](const RtspExceptType except) {
        SHARING_LOGE("onRtspException err");
        signal.notify_one();
    });

    rtspClient->SetRtspReadyCb([&]() { rtspClient->SendOption(); });

    if (!rtspClient->Open()) {
        SHARING_LOGE("openRtspClient err");
        return 0;
    }
    std::unique_lock<std::mutex> lk(mutex);
    signal.wait(lk);
    return 0;
};