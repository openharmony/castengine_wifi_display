/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "wfd_message_test.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdMessageTest::SetUpTestCase(void)
{
}

void WfdMessageTest::TearDownTestCase(void)
{
}

void WfdMessageTest::SetUp(void)
{
}

void WfdMessageTest::TearDown(void)
{
}

HWTEST_F(WfdMessageTest, WfdRtspM1Response_001, TestSize.Level1)
{
    WfdRtspM1Response response(1, RTSP_STATUS_OK);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseParse_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    std::string rspStr = response.Stringify();
    WfdRtspM3Response m3Response(1, RTSP_STATUS_OK);
    RtspError result = m3Response.Parse(rspStr);
    EXPECT_EQ(result.code, RtspErrorType::OK);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetVideoFormats_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_1920X1080_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetVideoFormats_002, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_1920X1080_25);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetVideoFormats_003, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_1280X720_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetVideoFormats_004, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_1280X720_25);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetVideoFormats_005, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_640X480_60);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetAudioCodecs_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetAudioCodecs(AUDIO_44100_8_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetAudioCodecs_002, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetAudioCodecs(AUDIO_44100_16_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetAudioCodecs_003, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetAudioCodecs(AUDIO_48000_16_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetAudioCodecs_004, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetAudioCodecs(AUDIO_44100_8_1);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetClientRtpPorts_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetClientRtpPorts(1);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetContentProtection_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetContentProtection("protect");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetCoupledSink_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetCoupledSink("coupled_sink");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetUibcCapability_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetUibcCapability("uibc_capab");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetStandbyResumeCapability_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetStandbyResumeCapability("standby_resume_capab");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseSetCustomParam_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetCustomParam("key", "value");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetClientRtpPorts_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetClientRtpPorts(1);
    int32_t ret = response.GetClientRtpPorts();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetUibcCapability_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetUibcCapability("enable");
    std::string capability = response.GetUibcCapability();
    EXPECT_EQ(capability, "enable");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetAudioCodecs_001, TestSize.Level1)
{
    WfdRtspM3Response m3Response(1, RTSP_STATUS_OK);
    m3Response.SetAudioCodecs(AUDIO_48000_16_2);
    auto m3RspStr = m3Response.Stringify();
    WfdRtspM3Response response;
    response.Parse(m3RspStr);
    AudioFormat aFormat = response.GetAudioCodecs();
    EXPECT_EQ(aFormat, AUDIO_48000_16_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetAudioCodecs_002, TestSize.Level1)
{
    WfdRtspM3Response m3Response(1, RTSP_STATUS_OK);
    m3Response.SetAudioCodecs(AUDIO_44100_8_2);
    auto m3RspStr = m3Response.Stringify();
    WfdRtspM3Response response;
    response.Parse(m3RspStr);
    AudioFormat aFormat = response.GetAudioCodecs();
    EXPECT_EQ(aFormat, AUDIO_48000_8_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetAudioCodecs_003, TestSize.Level1)
{
    WfdRtspM3Response m3Response(1, RTSP_STATUS_OK);
    m3Response.SetAudioCodecs(AUDIO_44100_8_2);
    auto m3RspStr = m3Response.Stringify();
    WfdRtspM3Response response;
    response.Parse(m3RspStr);
    AudioFormat aFormat = response.GetAudioCodecs();
    EXPECT_EQ(aFormat, AUDIO_48000_8_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetCoupledSink_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetCoupledSink("enable");
    std::string capability = response.GetCoupledSink();
    EXPECT_EQ(capability, "enable");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetContentProtection_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetContentProtection("enable");
    std::string capability = response.GetContentProtection();
    EXPECT_EQ(capability, "enable");
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_640_480_P60);
    EXPECT_EQ(vFormat, VIDEO_640X480_60);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_002, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_1280_720_P30);
    EXPECT_EQ(vFormat, VIDEO_1280X720_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_003, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_1280_720_P60);
    EXPECT_EQ(vFormat, VIDEO_1280X720_60);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_004, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_1920_1080_P25);
    EXPECT_EQ(vFormat, VIDEO_1920X1080_25);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_005, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_1920_1080_P30);
    EXPECT_EQ(vFormat, VIDEO_1920X1080_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormatsByCea_006, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    VideoFormat vFormat = response.GetVideoFormatsByCea(CEA_1920_1080_P60);
    EXPECT_EQ(vFormat, VIDEO_1920X1080_60);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetVideoFormats_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetVideoFormats(VIDEO_1920X1080_30);
    VideoFormat vFormat = response.GetVideoFormats();
    EXPECT_EQ(vFormat, VIDEO_1920X1080_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM3ResponseGetStandbyResumeCapability_001, TestSize.Level1)
{
    WfdRtspM3Response response(1, RTSP_STATUS_OK);
    response.SetStandbyResumeCapability("enable");
    std::string capablity = response.GetStandbyResumeCapability();
    EXPECT_EQ(capablity, "enable");
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetClientRtpPorts_001, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    request.SetClientRtpPorts(1);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetAudioCodecs_001, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    request.SetAudioCodecs(AUDIO_44100_8_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetAudioCodecs_002, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    request.SetAudioCodecs(AUDIO_44100_16_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetAudioCodecs_003, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    request.SetAudioCodecs(AUDIO_48000_16_2);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetPresentationUrl_001, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    request.SetPresentationUrl("1.1.1.1");
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetVideoFormats_001, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    WfdVideoFormatsInfo wfdVideoFormatsInfo;
    request.SetVideoFormats(wfdVideoFormatsInfo, VIDEO_1920X1080_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetVideoFormats_002, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    WfdVideoFormatsInfo wfdVideoFormatsInfo;
    request.SetVideoFormats(wfdVideoFormatsInfo, VIDEO_1920X1080_25);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetVideoFormats_003, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    WfdVideoFormatsInfo wfdVideoFormatsInfo;
    request.SetVideoFormats(wfdVideoFormatsInfo, VIDEO_1280X720_30);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetVideoFormats_004, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    WfdVideoFormatsInfo wfdVideoFormatsInfo;
    request.SetVideoFormats(wfdVideoFormatsInfo, VIDEO_1280X720_25);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestSetVideoFormats_005, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    WfdVideoFormatsInfo wfdVideoFormatsInfo;
    request.SetVideoFormats(wfdVideoFormatsInfo, VIDEO_640X480_60);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestParse_001, TestSize.Level1)
{
    WfdRtspM4Request request(1, "url");
    std::string reqStr(request.Stringify());
    RtspError result = request.Parse(reqStr);
    EXPECT_EQ(result.code, RtspErrorType::OK);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestGetPresentationUrl_001, TestSize.Level1)
{
    WfdRtspM4Request m4Request(1, "url");
    m4Request.SetPresentationUrl("1.1.1.1");
    auto m4ReqStr = m4Request.Stringify();
    WfdRtspM4Request request;
    request.Parse(m4ReqStr);
    std::string ret = request.GetPresentationUrl();
    EXPECT_NE(ret.find("1.1.1.1"), std::string::npos);
}

HWTEST_F(WfdMessageTest, WfdRtspM4RequestGetRtpPort_001, TestSize.Level1)
{
    WfdRtspM4Request m4Request(1, "url");
    m4Request.SetClientRtpPorts(1);
    auto m4ReqStr = m4Request.Stringify();
    WfdRtspM4Request request;
    request.Parse(m4ReqStr);
    int32_t ret = request.GetRtpPort();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(WfdMessageTest, WfdRtspM5RequestSetTriggerMethod_001, TestSize.Level1)
{
    WfdRtspM5Request request(1);
    request.SetTriggerMethod("trigger");
}

HWTEST_F(WfdMessageTest, WfdRtspM5RequestSetTriggerMethod_002, TestSize.Level1)
{
    WfdRtspM5Request request(1);
    request.SetTriggerMethod("trigger");
    std::string ret = request.GetTriggerMethod();
    EXPECT_EQ(ret, "trigger");
}

HWTEST_F(WfdMessageTest, WfdRtspM6ResponseSetClientPort_001, TestSize.Level1)
{
    WfdRtspM6Response response(1, 1, "sessionID", 30);
    response.SetClientPort(1);
}

HWTEST_F(WfdMessageTest, WfdRtspM6ResponseSetServerPort_001, TestSize.Level1)
{
    WfdRtspM6Response response;
    response.SetServerPort(1);
}

HWTEST_F(WfdMessageTest, WfdRtspM6ResponseGetClientPort_001, TestSize.Level1)
{
    WfdRtspM6Response m6Response(1, 1, "sessionID", 30);
    m6Response.SetClientPort(1);
    auto m6RspStr = m6Response.StringifyEx();
    WfdRtspM6Response response;
    response.Parse(m6RspStr);
    int32_t port = response.GetClientPort();
    EXPECT_EQ(port, 1);
}

HWTEST_F(WfdMessageTest, WfdRtspM6ResponseGetServerPort_001, TestSize.Level1)
{
    WfdRtspM6Response m6Response(1, 1, "sessionID", 30);
    m6Response.SetServerPort(1);
    auto m6RspStr = m6Response.StringifyEx();
    WfdRtspM6Response response;
    response.Parse(m6RspStr);
    int32_t port = response.GetServerPort();
    EXPECT_EQ(port, 1);
}

HWTEST_F(WfdMessageTest, WfdRtspM6ResponseStringifyEx_001, TestSize.Level1)
{
    WfdRtspM6Response m6Response(1, 1, "sessionID", 30);
    auto m6RspStr = m6Response.StringifyEx();
    WfdRtspM6Response response;
    RtspError ret = response.Parse(m6RspStr);
    EXPECT_EQ(ret.code, RtspErrorType::OK);
}

HWTEST_F(WfdMessageTest, WfdRtspM7ResponseStringifyEx_001, TestSize.Level1)
{
    WfdRtspM6Response m7Response(1, 1, "sessionID", 30);
    auto m7RspStr = m7Response.StringifyEx();
    WfdRtspM6Response response;
    RtspError ret = response.Parse(m7RspStr);
    EXPECT_EQ(ret.code, RtspErrorType::OK);
}

} // namespace Sharing
} // namespace OHOS
