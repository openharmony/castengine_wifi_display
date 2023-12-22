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

#ifndef OHOS_SHARING_WFD_RTP_CONSUMER_H
#define OHOS_SHARING_WFD_RTP_CONSUMER_H

#include <chrono>
#include "common/common_macro.h"
#include "common/object.h"
#include "mediachannel/base_consumer.h"
#include "mediachannel/buffer_dispatcher.h"
#include "network/interfaces/iclient_callback.h"
#include "network/network_factory.h"
#include "protocol/rtcp/include/rtcp_context.h"
#include "protocol/rtp/include/rtp_def.h"
#include "protocol/rtp/include/rtp_factory.h"
#include "protocol/rtp/include/rtp_unpack.h"

namespace OHOS {
namespace Sharing {

class WfdRtpConsumer final : public BaseConsumer,
                             public IServerCallback,
                             public std::enable_shared_from_this<WfdRtpConsumer> {
public:
    WfdRtpConsumer();
    ~WfdRtpConsumer();

    // impl BaseConsumer
    void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg) override;

    // impl IServerCallback
    void OnServerClose(int32_t fd) override {}
    void OnServerWriteable(int32_t fd) override {}
    void OnServerException(int32_t fd) override {}
    void OnAccept(std::weak_ptr<INetworkSession> session) override {}
    void OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session = nullptr) override;

    int32_t Release() override;
    int32_t HandleEvent(SharingEvent &event) override;

private:
    void HandleProsumerInitState(SharingEvent &event);

    void OnRtpUnpackNotify(int32_t errCode);
    void OnRtpUnpackCallback(uint32_t ssrc, const Frame::Ptr &frame);

    bool Init();
    bool Stop();
    bool Start();
    bool InitRtpUnpacker();
    bool StartNetworkServer(uint16_t port, NetworkFactory::ServerPtr &server, int32_t &fd);

private:
    bool isFirstPacket_ = true;
    bool isFirstKeyFrame_ = true;

    uint16_t port_ = 0;
    int32_t frameNums_ = 1;
    uint32_t contextId_ = 0;

    std::chrono::steady_clock::time_point gopInterval_;
    std::pair<int32_t, NetworkFactory::ServerPtr> rtpServer_ = {0, nullptr};

    MediaType mediaTypePaused_ = MEDIA_TYPE_AV;

    RtpUnpack::Ptr rtpUnpacker_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
