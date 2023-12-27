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

#ifndef OHOS_SHARING_WFD_RTP_PRODUCER_H
#define OHOS_SHARING_WFD_RTP_PRODUCER_H

#include <atomic>
#include <string>
#include "buffer_dispatcher.h"
#include "codec/include/codec_factory.h"
#include "common/event_comm.h"
#include "event/handle_event_base.h"
#include "frame/frame.h"
#include "mediachannel/base_producer.h"
#include "mediachannel/media_channel_def.h"
#include "network/interfaces/iclient_callback.h"
#include "network/interfaces/inetwork_session.h"
#include "network/network_factory.h"
#include "protocol/rtcp/include/rtcp_context.h"
#include "protocol/rtp/include/rtp_def.h"
#include "protocol/rtp/include/rtp_factory.h"
#include "protocol/rtp/include/rtp_pack.h"
#include "wfd_media_def.h"

namespace OHOS {
namespace Sharing {

class WfdRtpProducer : public BaseProducer,
                       public std::enable_shared_from_this<WfdRtpProducer> {
public:
    using Ptr = std::shared_ptr<WfdRtpProducer>;

    class UdpClient : public IClientCallback,
                      public std::enable_shared_from_this<UdpClient> {
    public:
        using Ptr = std::shared_ptr<UdpClient>;

        explicit UdpClient(bool rtcp);
        ~UdpClient();

        void OnClientClose(int32_t fd) override;
        void OnClientException(int32_t fd) override;
        void OnClientWriteable(int32_t fd) override;
        void OnClientConnect(bool isSuccess) override;
        void OnClientReadData(int32_t fd, DataBuffer::Ptr buf) override;

        void Stop();
        void Release();
        void SetUdpDataListener(std::weak_ptr<WfdRtpProducer> udpDataListener);

        bool SendDataBuffer(const DataBuffer::Ptr &buf);
        bool Connect(const std::string &peerIp, uint16_t peerPort, const std::string &localIp, uint16_t localPort);

    private:
        bool rtcp_ = false;
        std::weak_ptr<WfdRtpProducer> udpDataListener_;
        NetworkFactory::ClientPtr networkClientPtr_ = nullptr;
    };

    explicit WfdRtpProducer();
    ~WfdRtpProducer();

    // impl BaseProducer
    void StartDispatchThread() override;
    void PublishOneFrame(const MediaData::Ptr &mediaData) override;
    void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg) override;

    int32_t Release() override;
    int32_t HandleEvent(SharingEvent &event) override;

    void OnRtcpReadData(int32_t fd, DataBuffer::Ptr buf);

private:
    bool ProducerInit();
    bool SendDataBuffer(const DataBuffer::Ptr &buf, bool audio = true);

    int32_t Stop();
    int32_t Connect();
    int32_t InitUdpClients();
    int32_t InitTsRtpPacker(uint32_t ssrc, size_t mtuSize = 1400, uint32_t sampleRate = 90000, uint8_t pt = 33,
                            RtpPayloadStream ps = RtpPayloadStream::MPEG2_TS);

    void OnRtcpTimeOut();
    void DispatchMediaData();
    void HandleMediaInit(std::shared_ptr<WfdProducerEventMsg> msg);

private:
    std::atomic_bool dispatching_ = false;

    uint16_t localPort_ = MIN_PORT;
    uint16_t primarySinkPort_ = MIN_PORT;

    uint32_t ssrc_ = 0x2000;
    int32_t rtcpCheckInterval_ = 0;
    std::atomic_uint32_t rtcpOvertimes_ = 0;

    std::string localIp_ = "127.0.0.1";
    std::string primarySinkIp_ = "127.0.0.1";

    std::shared_ptr<UdpClient> tsUdpClient_ = nullptr;
    std::shared_ptr<UdpClient> tsRtcpUdpClient_ = nullptr;
    std::shared_ptr<std::thread> dispatchThread_ = nullptr;
    std::shared_ptr<RtcpSenderContext> rtcpSendContext_ = nullptr;

    RtpPack::Ptr tsPacker_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_WFD_RTP_PRODUCER_H
