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

#include "rtcp.h"
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <securec.h>
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {

//------------------------------ RtcpHeader ------------------------------//

int32_t RtcpHeader::GetSize() const
{
    return (1 + ntohs(length_)) << 2; // 2:byte offset
}

void RtcpHeader::SetSize(int32_t size)
{
    length_ = htons((size >> 2) - 1); // 2:byte offset
}

int32_t RtcpHeader::GetPaddingSize() const
{
    if (!padding_) {
        return 0;
    }

    return ((uint8_t *)this)[GetSize() - 1];
}

//------------------------------ common ------------------------------//

static inline int32_t AlignSize(int32_t bytes)
{
    return static_cast<int32_t>(((bytes + 3) >> 2) << 2); // 2:byte offset, 3:byte length
}

static void SetupHeader(RtcpHeader *rtcp, RtcpType type, int32_t reportCount, int32_t totalBytes)
{
    RETURN_IF_NULL(rtcp);
    rtcp->version_ = 2; // 2:byte offset
    rtcp->padding_ = 0;
    if (reportCount > 0x1F) {
        SHARING_LOGD("reportCount is too large.");
    }

    rtcp->reportCount_ = reportCount;
    rtcp->pt_ = (uint8_t)type;
    rtcp->SetSize(totalBytes);
}

static void SetupPadding(RtcpHeader *rtcp, size_t paddingSize)
{
    RETURN_IF_NULL(rtcp);

    if (paddingSize) {
        rtcp->padding_ = 1;
        ((uint8_t *)rtcp)[rtcp->GetSize() - 1] = paddingSize & 0xFF;
    } else {
        rtcp->padding_ = 0;
    }
}

//------------------------------ RtcpSR ------------------------------//

std::shared_ptr<RtcpSR> RtcpSR::Create(int32_t itemCount)
{
    auto realSize = sizeof(RtcpSR) - sizeof(ReportItem) + itemCount * sizeof(ReportItem);
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpSR *)new char[bytes];
    if (ptr == nullptr) {
        return nullptr;
    }
    SetupHeader(ptr, RtcpType::RTCP_SR, itemCount, bytes);
    SetupPadding(ptr, bytes - realSize);

    return std::shared_ptr<RtcpSR>(ptr, [](RtcpSR *ptr) { delete[] (char *)ptr; });
}

RtcpSR &RtcpSR::SetNtpStamp(timeval tv)
{
    ntpmsw_ = htonl(tv.tv_sec + 0x83AA7E80); /* 0x83AA7E80 is the number of seconds from 1900 to 1970 */
    ntplsw_ = htonl((uint32_t)((double)tv.tv_usec * (double)(((uint64_t)1) << 32) * 1.0e-6)); // 32:byte offset
    return *this;
}

RtcpSR &RtcpSR::SetNtpStamp(uint64_t unixStampMs)
{
    timeval tv;
    tv.tv_sec = unixStampMs / 1000;           // 1000:unit
    tv.tv_usec = (unixStampMs % 1000) * 1000; // 1000:unit
    return SetNtpStamp(tv);
}

std::string RtcpSR::GetNtpStamp() const
{
    struct timeval tv;
    tv.tv_sec = ntpmsw_ - 0x83AA7E80;
    tv.tv_usec = (decltype(tv.tv_usec))(ntplsw_ / ((double)(((uint64_t)1) << 32) * 1.0e-6)); // 32:byte offset, 6:-

    char ts[30] = {0};
    auto tm = localtime(&tv.tv_sec);
    if (tm == nullptr) {
        return {};
    }
    if (strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm) < 0) {
        return {};
    }
    if (sprintf_s(ts + strlen(ts), sizeof(ts) - strlen(ts), ".%06lld", static_cast<long long>(tv.tv_usec)) < 0) {
        return {};
    }
    // format: "2022-09-02 17:27:04.021154"
    return std::string(ts);
}

uint64_t RtcpSR::GetNtpUnixStampMS() const
{
    if (ntpmsw_ < 0x83AA7E80 /*1970 year*/) {
        return 0;
    }
    struct timeval tv;
    tv.tv_sec = ntpmsw_ - 0x83AA7E80;
    tv.tv_usec = (decltype(tv.tv_usec))(ntplsw_ / ((double)(((uint64_t)1) << 32) * 1.0e-6)); // 32:byte offset, 6:-

    return 1000 * tv.tv_sec + tv.tv_usec / 1000; // 1000:unit
}

RtcpSR &RtcpSR::SetSsrc(uint32_t ssrc)
{
    ssrc_ = htonl(ssrc);
    return *this;
}

uint32_t RtcpSR::GetSsrc() const
{
    return ntohl(ssrc_);
}

RtcpSR &RtcpSR::SetPacketCount(uint32_t count)
{
    packetCount_ = htonl(count);
    return *this;
}

uint32_t RtcpSR::GetPacketCount() const
{
    return ntohl(packetCount_);
}

std::vector<ReportItem *> RtcpSR::GetItemList()
{
    std::vector<ReportItem *> list;
    ReportItem *ptr = &items_;
    for (int32_t i = 0; i < (int32_t)reportCount_; ++i) {
        list.emplace_back(ptr);
        ++ptr;
    }

    return list;
}

//------------------------------ RtcpRR ------------------------------//

std::shared_ptr<RtcpRR> RtcpRR::Create(size_t itemCount)
{
    auto realSize = sizeof(RtcpRR) - sizeof(ReportItem) + itemCount * sizeof(ReportItem);
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpRR *)new char[bytes];
    if (ptr == nullptr) {
        return nullptr;
    }
    SetupHeader(ptr, RtcpType::RTCP_RR, itemCount, bytes);
    SetupPadding(ptr, bytes - realSize);
    return std::shared_ptr<RtcpRR>(ptr, [](RtcpRR *ptr) { delete[] (char *)ptr; });
}

std::vector<ReportItem *> RtcpRR::GetItemList()
{
    std::vector<ReportItem *> list;
    ReportItem *ptr = &items_;
    for (int32_t i = 0; i < (int32_t)reportCount_; ++i) {
        list.emplace_back(ptr);
        ++ptr;
    }

    return list;
}

//------------------------------ RtcpSdes ------------------------------//

size_t SdesChunk::TotalBytes() const
{
    return AlignSize(MinSize() + txtLen_);
}

// chunk length without variable length information
size_t SdesChunk::MinSize()
{
    return sizeof(SdesChunk) - sizeof(text_);
}

std::shared_ptr<RtcpSdes> RtcpSdes::Create(const std::vector<std::string> &itemText)
{
    size_t itemTotalSize = 0;
    for (auto &text : itemText) {
        itemTotalSize += AlignSize(SdesChunk::MinSize() + (0xFF & text.size()));
    }
    auto realSize = sizeof(RtcpSdes) - sizeof(SdesChunk) + itemTotalSize;
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpSdes *)new char[bytes];
    if (ptr == nullptr) {
        return nullptr;
    }
    memset_s(ptr, bytes, 0, bytes);

    auto itemPtr = &ptr->chunks_;
    for (auto &text : itemText) {
        itemPtr->txtLen_ = (0xFF & text.size());
        auto ret = memcpy_s(itemPtr->text_, itemPtr->txtLen_ + 1, text.data(), itemPtr->txtLen_ + 1);
        if (ret != EOK) {
            return nullptr;
        }
        itemPtr = (SdesChunk *)((char *)itemPtr + itemPtr->TotalBytes());
    }

    SetupHeader(ptr, RtcpType::RTCP_SDES, itemText.size(), bytes);
    SetupPadding(ptr, bytes - realSize);
    return std::shared_ptr<RtcpSdes>(ptr, [](RtcpSdes *ptr) { delete[] (char *)ptr; });
}

std::vector<SdesChunk *> RtcpSdes::GetChunkList()
{
    std::vector<SdesChunk *> list;
    SdesChunk *ptr = &chunks_;
    for (int32_t i = 0; i < (int32_t)reportCount_; ++i) {
        list.emplace_back(ptr);
        ptr = (SdesChunk *)((char *)ptr + ptr->TotalBytes());
    }

    return list;
}

//------------------------------ RtcpFB ------------------------------//

std::shared_ptr<RtcpFB> RtcpFB::Create(PsfbType fmt, const void *fci, size_t fciLen)
{
    return RtcpFB::CreateInner(RtcpType::RTCP_PSFB, (int32_t)fmt, fci, fciLen);
}

std::shared_ptr<RtcpFB> RtcpFB::Create(RtpfbType fmt, const void *fci, size_t fciLen)
{
    return RtcpFB::CreateInner(RtcpType::RTCP_RTPFB, (int32_t)fmt, fci, fciLen);
}

const uint8_t *RtcpFB::GetFciPtr() const
{
    return (uint8_t *)&ssrcMedia_ + sizeof(ssrcMedia_);
}

int32_t RtcpFB::GetFciSize() const
{
    return GetSize() - GetPaddingSize() - sizeof(RtcpFB);
}

std::shared_ptr<RtcpFB> RtcpFB::CreateInner(RtcpType type, int32_t fmt, const void *fci, size_t fciLen)
{
    if (!fci) {
        fciLen = 0;
    }

    auto realSize = sizeof(RtcpFB) + fciLen;
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpFB *)new char[bytes];
    if (fci && fciLen) {
        auto ret = memcpy_s((char *)ptr + sizeof(RtcpFB), fciLen, fci, fciLen);
        if (ret != EOK) {
            return nullptr;
        }
    }
    SetupHeader(ptr, type, fmt, bytes);
    SetupPadding(ptr, bytes - realSize);
    return std::shared_ptr<RtcpFB>((RtcpFB *)ptr, [](RtcpFB *ptr) { delete[] (char *)ptr; });
}

//------------------------------ RtcpBye ------------------------------//

std::shared_ptr<RtcpBye> RtcpBye::Create(const std::vector<uint32_t> &ssrcs, const std::string &reason)
{
    auto realSize = sizeof(RtcpHeader) + sizeof(uint32_t) * ssrcs.size() + 1 + reason.size();
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpBye *)new char[bytes];
    if (ptr == nullptr) {
        return nullptr;
    }
    SetupHeader(ptr, RtcpType::RTCP_BYE, ssrcs.size(), bytes);
    SetupPadding(ptr, bytes - realSize);

    int32_t i = 0;
    for (auto ssrc : ssrcs) {
        ((RtcpBye *)ptr)->ssrc_[i++] = htonl(ssrc);
    }

    if (!reason.empty()) {
        uint8_t *reasonLenPtr = (uint8_t *)ptr + sizeof(RtcpHeader) + sizeof(uint32_t) * ssrcs.size();
        *reasonLenPtr = reason.size() & 0xFF;
        auto ret = memcpy_s(reasonLenPtr + 1, *reasonLenPtr, reason.data(), *reasonLenPtr);
        if (ret != EOK) {
            return nullptr;
        }
    }

    return std::shared_ptr<RtcpBye>(ptr, [](RtcpBye *ptr) { delete[] (char *)ptr; });
}

std::vector<uint32_t> RtcpBye::GetSSRC() const
{
    std::vector<uint32_t> ret;
    for (size_t i = 0; i < reportCount_; ++i) {
        ret.emplace_back(ntohl(ssrc_[i]));
    }

    return ret;
}

std::string RtcpBye::GetReason() const
{
    auto *reasonLenPtr = (uint8_t *)ssrc_ + sizeof(uint32_t) * reportCount_;
    if (reasonLenPtr + 1 >= (uint8_t *)this + GetSize()) {
        return "";
    }

    return std::string((char *)reasonLenPtr + 1, *reasonLenPtr);
}

//-----------------------------------------------------------------------------

std::shared_ptr<RtcpXRDLRR> RtcpXRDLRR::Create(size_t itemCount)
{
    auto realSize = sizeof(RtcpXRDLRR) - sizeof(RtcpXRDLRRReportItem) + itemCount * sizeof(RtcpXRDLRRReportItem);
    auto bytes = AlignSize(realSize);
    if (bytes == 0 || bytes < 0) {
        return nullptr;
    }
    auto ptr = (RtcpXRDLRR *)new char[bytes];
    if (ptr == nullptr) {
        return nullptr;
    }
    SetupHeader(ptr, RtcpType::RTCP_XR, 0, bytes);
    SetupPadding(ptr, bytes - realSize);
    return std::shared_ptr<RtcpXRDLRR>(ptr, [](RtcpXRDLRR *ptr) { delete[] (char *)ptr; });
}

std::vector<RtcpXRDLRRReportItem *> RtcpXRDLRR::GetItemList()
{
    auto count = blockLength_ / 3;
    RtcpXRDLRRReportItem *ptr = &items_;
    std::vector<RtcpXRDLRRReportItem *> list;
    for (int32_t i = 0; i < (int32_t)count; ++i) {
        list.emplace_back(ptr);
        ++ptr;
    }

    return list;
}

} // namespace Sharing
} // namespace OHOS