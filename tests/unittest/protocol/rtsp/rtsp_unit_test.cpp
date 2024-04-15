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

#include "rtsp_unit_test.h"
#include <iostream>
#include "protocol/rtsp/include/rtsp_common.h"
#include "protocol/rtsp/include/rtsp_request.h"
#include "protocol/rtsp/include/rtsp_response.h"
#include "protocol/rtsp/include/rtsp_sdp.h"
#include "protocol/rtsp/include/rtsp_url.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void RtspUnitTest::SetUpTestCase() {}
void RtspUnitTest::TearDownTestCase() {}
void RtspUnitTest::SetUp() {}
void RtspUnitTest::TearDown() {}

namespace {

HWTEST_F(RtspUnitTest, RtspUnitTest_001, Function | SmallTest | Level2)
{
    auto ret = RtspCommon::GetRtspDate();
    ASSERT_TRUE(ret != "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_002, Function | SmallTest | Level2)
{
    const std::string str = "123";
    auto ret = RtspCommon::Trim(str);
    EXPECT_EQ(ret, str);
    const std::string str1 = " 123";
    auto ret1 = RtspCommon::Trim(str1);
    EXPECT_EQ(ret1, str);
    const std::string str2 = "123 ";
    auto ret2 = RtspCommon::Trim(str2);
    EXPECT_EQ(ret2, str);
    const std::string str3 = " 123 ";
    auto ret3 = RtspCommon::Trim(str3);
    EXPECT_EQ(ret3, str);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_003, Function | SmallTest | Level2)
{
    const std::string str = "12 3";
    auto ret = RtspCommon::Trim(str);
    EXPECT_EQ(ret, str);
    const std::string str1 = " 12 3";
    auto ret1 = RtspCommon::Trim(str1);
    EXPECT_EQ(ret1, str);
    const std::string str2 = "12 3 ";
    auto ret2 = RtspCommon::Trim(str2);
    EXPECT_EQ(ret2, str);
    const std::string str3 = " 12 3 ";
    auto ret3 = RtspCommon::Trim(str3);
    EXPECT_EQ(ret3, str);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_004, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_OPTIONS;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_005, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_DESCRIBE;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_006, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_ANNOUNCE;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_007, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_SETUP;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_008, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_PLAY;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_009, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_PAUSE;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_010, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_TEARDOWN;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_011, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_GET_PARAMETER;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_012, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_SET_PARAMETER;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_013, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_REDIRECT;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_014, Function | SmallTest | Level2)
{
    const std::string method = RTSP_METHOD_RECORD;
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_015, Function | SmallTest | Level2)
{
    const std::string method = "unknown";
    auto ret = RtspCommon::VerifyMethod(method);
    EXPECT_NE(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_016, Function | SmallTest | Level2)
{
    const std::string str = "str";
    const std::string delimiter = "s";
    auto ret = RtspCommon::Split(str, delimiter);
    ASSERT_TRUE(ret.size() > 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_017, Function | SmallTest | Level2)
{
    std::list<std::string> lines;
    std::list<std::pair<std::string, std::string>> params;
    lines.push_front("key:12");
    lines.push_front("key2:212");
    lines.push_front("key3:3212");
    RtspCommon::SplitParameter(lines, params);
    ASSERT_TRUE(params.size() == 3);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_018, Function | SmallTest | Level2)
{
    const std::string message =
        "Request: GET_PARAMETER rtsp://localhost/wfd1.0 RTSP/1.0\r\nCSeq: 2\r\nContent-type: text/parameters";
    std::vector<std::string> firstLine;
    std::unordered_map<std::string, std::string> header;
    std::list<std::string> body;
    auto ret = RtspCommon::ParseMessage(message, firstLine, header, body);
    ASSERT_TRUE(ret.code == RtspErrorType::OK);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_019, Function | SmallTest | Level2)
{
    const std::string &username = "username";
    const std::string &realm = "realm";
    const std::string &password = "password";
    const std::string &nonce = "nonce";
    const std::string &method = "method";
    const std::string &url = "url";
    auto ret = RtspCommon::GenerateAuthorization(username, realm, password, nonce, method, url);
    EXPECT_EQ(ret,
              "Digest username=\"username\", realm=\"realm\", nonce=\"nonce\", uri=\"url\", response=\"00000000\"");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_020, Function | SmallTest | Level2)
{
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_021, Function | SmallTest | Level2)
{
    const std::string &request =
        "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\nUser-Agent: KaihongOS\r\nRequire: org.wfa.wfd1.0\r\n";
    auto rtspRequest = std::make_shared<RtspRequest>(request);
    EXPECT_NE(rtspRequest, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_022, Function | SmallTest | Level2)
{
    const std::string &method = RTSP_METHOD_OPTIONS;
    int32_t cseq = 1;
    const std::string &url = "rtsp://192.168.49.1/wfd1.0/streamid=0 RTSP/1.0\r\n";
    auto request = std::make_shared<RtspRequest>(method, cseq, url);
    EXPECT_NE(request, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_023, Function | SmallTest | Level2)
{
    const std::string &method = RTSP_METHOD_OPTIONS;
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetMethod(method);
    auto ret = request->GetMethod();
    EXPECT_EQ(ret, method);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_024, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetCSeq(cseq);
    auto ret = request->GetCSeq();
    EXPECT_EQ(ret, cseq);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_025, Function | SmallTest | Level2)
{
    const std::string &url = "rtsp://192.168.49.1/wfd1.0/streamid=0 RTSP/1.0\r\n";
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetUrl(url);
    auto ret = request->GetUrl();
    EXPECT_EQ(ret, url);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_026, Function | SmallTest | Level2)
{
    const std::string &authorization = "authorization";
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetAuthorization(authorization);
    auto ret = request->GetAuthorization();
    EXPECT_EQ(ret, authorization);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_027, Function | SmallTest | Level2)
{
    const std::string &userAgent = "userAgent";
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetUserAgent(userAgent);
    auto ret = request->GetUserAgent();
    EXPECT_EQ(ret, userAgent);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_028, Function | SmallTest | Level2)
{
    const std::string &session = "session";
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetSession(session);
    auto ret = request->GetSession();
    EXPECT_EQ(ret, session);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_029, Function | SmallTest | Level2)
{
    int32_t timeout = 10;
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->SetTimeout(timeout);
    EXPECT_EQ(timeout, request->timeout_);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_030, Function | SmallTest | Level2)
{
    const std::string &header = "header";
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    request->AddCustomHeader(header);
    EXPECT_EQ(header, request->customHeaders_);
    request->ClearCustomHeader();
    EXPECT_EQ(request->customHeaders_, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_031, Function | SmallTest | Level2)
{
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    auto ret = request->GetBody();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_032, Function | SmallTest | Level2)
{
    const std::string &method = RTSP_METHOD_OPTIONS;
    int32_t cseq = 1;
    const std::string &url = "rtsp://192.168.49.1/wfd1.0/streamid=0 RTSP/1.0\r\n";
    auto request = std::make_shared<RtspRequest>(method, cseq, url);
    EXPECT_NE(request, nullptr);
    auto ret = request->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_033, Function | SmallTest | Level2)
{
    auto rtspRequest = std::make_shared<RtspRequest>();
    EXPECT_NE(rtspRequest, nullptr);
    const std::string &request =
        "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\nUser-Agent: KaihongOS\r\nRequire: org.wfa.wfd1.0\r\n";
    auto ret = rtspRequest->Parse(request);
    EXPECT_EQ(ret.code, RtspErrorType::OK);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_034, Function | SmallTest | Level2)
{
    auto request = std::make_shared<RtspRequest>();
    EXPECT_NE(request, nullptr);
    const std::string token = "token";
    const std::string value = "value";
    request->tokens_[token] = value;
    auto ret = request->GetToken(token);
    EXPECT_EQ(ret, value);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_035, Function | SmallTest | Level2)
{
    auto response = std::make_shared<RtspResponse>();
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_036, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_037, Function | SmallTest | Level2)
{
    auto response = std::make_shared<RtspResponse>();
    EXPECT_NE(response, nullptr);
    const std::string &header = "header";
    response->AddCustomHeader(header);
    EXPECT_EQ(header, response->customHeaders_);
    response->ClearCustomHeader();
    EXPECT_EQ(response->customHeaders_, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_038, Function | SmallTest | Level2)
{
    int32_t timeout = 10;
    auto response = std::make_shared<RtspResponse>();
    EXPECT_NE(response, nullptr);
    response->SetTimeout(timeout);
    auto ret = response->GetTimeout();
    EXPECT_EQ(ret, timeout);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_039, Function | SmallTest | Level2)
{
    const std::string &session = "session";
    auto response = std::make_shared<RtspResponse>();
    EXPECT_NE(response, nullptr);
    response->SetSession(session);
    auto ret = response->GetSession();
    EXPECT_EQ(ret, session);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_040, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->GetCSeq();
    EXPECT_EQ(ret, cseq);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_041, Function | SmallTest | Level2)
{
    const std::string &responseMsg =
        "RTSP/1.0 200 OK\r\nCSeq: 2\r\nDate: Thu, Nov 09 2023 08:43:36 GMT\r\nContent-Type: text/parameters\r\n"
        "Content-Length: 961\r\n\r\n"
        "wfd_video_formats : 38 00 02 10 00000080 00000000 00000000 00 0000 0000 00 none none\r\n"
        "wfd_audio_codecs: AAC 00000001 00\r\nwfd_client_rtp_ports: RTP/AVP/UDP;unicast 6700 0 mode=play"
        "\r\nwfd_display_edid: none\r\nwfd_connector_type: none\r\nwfd_uibc_capability: none\r\n"
        "wfd2_rotation_capability: none\r\nwfd2_video_formats: none\r\n"
        "wfd2_audio_codecs: none\r\nwfd2_video_stream_control: none\r\n"
        "wfd_content_protection: none\r\nwfd_idr_request_capability: none\r\n"
        "intel_friendly_name: none\r\nintel_sink_manufacturer_name: none\r\n"
        "intel_sink_model_name: none\r\nintel_sink_version: none\r\n"
        "intel_sink_device_URL: none\r\nmicrosoft_latency_management_capability: none\r\n"
        "microsoft_format_change_capability: none\r\n"
        "microsoft_diagnostics_capability: none\r\nmicrosoft_cursor: none\r\n"
        "microsoft_rtcp_capability: none\r\n"
        "microsoft_video_formats: none\r\n"
        "microsoft_max_bitrate: none\r\nmicrosoft_multiscreen_projection: none\r\n"
        "microsoft_audio_mute: none\r\nmicrosoft_color_space_conversion: none\r\nwfd_connector_type: 5\r\n";
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Parse(responseMsg);
    ASSERT_TRUE(ret.code == RtspErrorType::OK);
    auto date = response->GetDate();
    EXPECT_NE(date, "");
    auto statusRet = response->GetStatus();
    EXPECT_EQ(statusRet, status);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_042, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &msg1 = "RTSP/1.0 401 Unauthorized\r\nCSeq: 3\r\n";
    const std::string &msg2 = "WWW-Authenticate: Digest realm=\"IP Camera(23435)\",";
    const std::string &msg3 = "nonce=\"8fd7c44874480bd643d970149224da11\",";
    const std::string &msg4 = "stale=\"FALSE\"\r\nDate:  Wed, Jun 03 2020 10:54:30 GMT\r\n";
    response->Parse(msg1 + msg2 + msg3 + msg4);
    auto ret = response->GetDigestRealm();
    EXPECT_EQ(ret, "IP Camera(23435)\",nonce=\"8fd7c44874480bd643d970149224da11\",stale=\"FALSE");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_043, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &msg1 = "RTSP/1.0 401 Unauthorized\r\nCSeq: 3\r\n";
    const std::string &msg2 = "WWW-Authenticate: Digest realm=\"IP Camera(23435)\",";
    const std::string &msg3 = "nonce=\"8fd7c44874480bd643d970149224da11\",";
    const std::string &msg4 = "stale=\"FALSE\"\r\nDate:  Wed, Jun 03 2020 10:54:30 GMT\r\n";
    response->Parse(msg1 + msg2 + msg3 + msg4);
    auto ret = response->GetNonce();
    EXPECT_NE(ret, "\"8fd7c44874480bd643d970149224da11\"");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_044, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->GetBody();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_045, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_046, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponse>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string token = "token";
    const std::string value = "value";
    response->tokens_[token] = value;
    auto ret = response->GetToken(token);
    EXPECT_EQ(ret, value);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_047, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseOptions>(cseq, status);
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_048, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseOptions>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &lists = "lists";
    response->SetPublicItems(lists);
    EXPECT_EQ(response->publicItems_, lists);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_049, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseOptions>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_050, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseDescribe>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_051, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseDescribe>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &line = "line";
    response->AddBodyItem(line);
    EXPECT_NE(response->body_.size(), 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_052, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseDescribe>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &url = "rtsp://192.168.2.4/wfd1.0/streamid=0";
    response->SetUrl(url);
    EXPECT_EQ(response->url_, url);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_053, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_054, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &destination = "destination";
    response->SetDestination(destination);
    EXPECT_EQ(response->destination_, destination);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_055, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &source = "source";
    response->SetSource(source);
    EXPECT_EQ(response->source_, source);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_056, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    int32_t minClientPort = 6700;
    int32_t maxClientPort = 0;
    response->SetClientPort(minClientPort, maxClientPort);
    EXPECT_EQ(response->minClientPort_, minClientPort);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_057, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    int32_t minClientPort = 6700;
    int32_t maxClientPort = 6702;
    response->SetClientPort(minClientPort, maxClientPort);
    EXPECT_EQ(response->minClientPort_, minClientPort);
    EXPECT_EQ(response->maxServerPort_, maxClientPort);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_058, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetup>(cseq, status);
    EXPECT_NE(response, nullptr);
    int32_t minServerPort = 67000;
    int32_t maxServerPort = 67002;
    response->SetServerPort(minServerPort, maxServerPort);
    EXPECT_EQ(response->minServerPort_, minServerPort);
    EXPECT_EQ(response->maxServerPort_, maxServerPort);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_059, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponsePlay>(cseq, status);
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_060, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseTeardown>(cseq, status);
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_061, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseGetParameter>(cseq, status);
    EXPECT_NE(response, nullptr);
    auto ret = response->Stringify();
    EXPECT_NE(ret, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_062, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseGetParameter>(cseq, status);
    EXPECT_NE(response, nullptr);
    const std::string &line = "line";
    response->AddBodyItem(line);
    EXPECT_NE(response->body_.size(), 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_063, Function | SmallTest | Level2)
{
    int32_t cseq = 1;
    int32_t status = 200;
    auto response = std::make_shared<RtspResponseSetParameter>(cseq, status);
    EXPECT_NE(response, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_064, Function | SmallTest | Level2)
{
    auto session = std::make_shared<SessionOrigin>();
    EXPECT_NE(session, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_065, Function | SmallTest | Level2)
{
    auto session = std::make_shared<SessionOrigin>();
    EXPECT_NE(session, nullptr);
    uint32_t sessionVersion = 1;
    const std::string netType = "IN";
    const std::string username = "StreamingServer";
    const std::string addrType = "IP4";
    const std::string sessionId = "3677033027";
    const std::string unicastAddr = "192.168.1.44";
    const std::string origin = "StreamingServer 3677033027 1 IN IP4 192.168.1.44";
    auto ret = session->Parse(origin);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(session->sessionVersion, sessionVersion);
    EXPECT_EQ(session->netType, netType);
    EXPECT_EQ(session->username, username);
    EXPECT_EQ(session->addrType, addrType);
    EXPECT_EQ(session->sessionId, sessionId);
    EXPECT_EQ(session->unicastAddr, unicastAddr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_066, Function | SmallTest | Level2)
{
    auto media = std::make_shared<MediaLine>();
    EXPECT_NE(media, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_067, Function | SmallTest | Level2)
{
    auto media = std::make_shared<MediaLine>();
    EXPECT_NE(media, nullptr);
    uint16_t port = 49170;
    int32_t fmt = 0;
    const std::string mediaType = "audio";
    const std::string protoType = "RTP/AVP";
    const std::string mediaLine = "audio 49170 RTP/AVP 0";
    auto ret = media->Parse(mediaLine);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(media->port, port);
    EXPECT_EQ(media->fmt, fmt);
    EXPECT_EQ(media->mediaType, mediaType);
    EXPECT_EQ(media->protoType, protoType);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_068, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "audio 49170 RTP/AVP 0";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    auto ret2 = mediaDesc->GetMediaType();
    EXPECT_EQ(ret2, "audio");
    auto ret3 = mediaDesc->GetPayloadType();
    EXPECT_EQ(ret3, 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_069, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "video 49170/2 RTP/AVP 31";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    auto ret2 = mediaDesc->GetMediaType();
    EXPECT_EQ(ret2, "video");
    auto ret3 = mediaDesc->GetPayloadType();
    EXPECT_EQ(ret3, 31);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_070, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    mediaDesc->attributes_.push_back(
        "fmtp:96 profile-level-id=4D4015;"
        "sprop-parameter-sets=Z01AFZZWCwSbCEiAAAH0AAAw1DBgAHP2AOg1cABQ,aO88gA==;packetization-mode=1");
    mediaDesc->attributes_.push_back("cliprect:0,0,576,352");
    mediaDesc->attributes_.push_back("rtpmap:99 X-GSMLPC/8000");
    mediaDesc->attributes_.push_back("control:trackID=101");
    auto ret1 = mediaDesc->GetRtpMap();
    EXPECT_EQ(ret1, "99 X-GSMLPC/8000");
    auto ret2 = mediaDesc->GetTrackId();
    EXPECT_EQ(ret2, "trackID=101");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_071, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "video 49170/2 RTP/AVP 31";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    mediaDesc->attributes_.push_back(
        "fmtp:96 profile-level-id=4D4015;"
        "sprop-parameter-sets=Z01AFZZWCwSbCEiAAAH0AAAw1DBgAHP2AOg1cABQ,aO88gA==;packetization-mode=1");
    auto ret2 = mediaDesc->GetVideoSps();
    EXPECT_EQ(ret2.size(), 30);
    auto ret3 = mediaDesc->GetVideoPps();
    EXPECT_EQ(ret3.size(), 0);
    auto ret4 = mediaDesc->GetVideoSize();
    EXPECT_EQ(ret4.first, 352);
    EXPECT_EQ(ret4.second, 288);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_072, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "audio 49170 RTP/AVP 0";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    mediaDesc->attributes_.push_back(
        "fmtp:96 profile-level-id=4D4015;"
        "sprop-parameter-sets=Z01AFZZWCwSbCEiAAAH0AAAw1DBgAHP2AOg1cABQ,aO88gA==;packetization-mode=1");
    auto ret2 = mediaDesc->GetVideoSps();
    EXPECT_EQ(ret2.size(), 0);
    auto ret3 = mediaDesc->GetVideoPps();
    EXPECT_EQ(ret3.size(), 0);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_073, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "audio 49170 RTP/AVP 0";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    mediaDesc->attributes_.push_back("rtpmap:98 L16/11025/2");
    mediaDesc->attributes_.push_back("fmtp:97 streamtype=5;profile-level-id=15; mode=AAC-hbr;"
                                     " config=1308; SizeLength=13; IndexLength=3;IndexDeltaLength=3; Profile=1;");
    auto ret2 = mediaDesc->GetAudioChannels();
    EXPECT_EQ(ret2, 2);
    auto ret3 = mediaDesc->GetAudioSamplingRate();
    EXPECT_EQ(ret3, 11025);
    auto ret4 = mediaDesc->GetAudioConfig();
    EXPECT_EQ(ret4, "1308");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_074, Function | SmallTest | Level2)
{
    auto mediaDesc = std::make_shared<MediaDescription>();
    EXPECT_NE(mediaDesc, nullptr);
    const std::string mediaLine = "video 49170/2 RTP/AVP 31";
    auto ret1 = mediaDesc->media_.Parse(mediaLine);
    ASSERT_TRUE(ret1);
    mediaDesc->attributes_.push_back("rtpmap:98 L16/11025/2");
    mediaDesc->attributes_.push_back("fmtp:97 streamtype=5;profile-level-id=15; mode=AAC-hbr;"
                                     " config=1308; SizeLength=13; IndexLength=3;IndexDeltaLength=3; Profile=1;");
    auto ret2 = mediaDesc->GetAudioChannels();
    EXPECT_EQ(ret2, 0);
    auto ret3 = mediaDesc->GetAudioSamplingRate();
    EXPECT_EQ(ret3, 0);
    auto ret4 = mediaDesc->GetAudioConfig();
    EXPECT_EQ(ret4, "");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_075, Function | SmallTest | Level2)
{
    auto rtspSdp = std::make_shared<RtspSdp>();
    EXPECT_NE(rtspSdp, nullptr);
    const std::string mediaLine = "m=video 49170/2 RTP/AVP 31 \r\n";
    auto ret = rtspSdp->Parse(mediaLine);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_076, Function | SmallTest | Level2)
{
    auto rtspSdp = std::make_shared<RtspSdp>();
    EXPECT_NE(rtspSdp, nullptr);
    const std::string mediaLine = "v=0\r\nb=X-YZ:128\r\no=HWPSS 3427743244 1084119141 IN IP4 127.0.0.1\r\n"
                                  "s=test1.mp4\r\ni=test\r\nu=uri\r\ne=email\r\np=123456\r\nt=121212 232322\r\n"
                                  "r=121212\r\nz=zone;\r\na=attr\r\nc=connection\r\n";
    auto ret = rtspSdp->Parse(mediaLine);
    EXPECT_EQ(ret, true);
    auto ret1 = rtspSdp->GetOrigin();
    EXPECT_EQ(ret1.addrType, "IP4");
    EXPECT_EQ(ret1.netType, "IN");
    EXPECT_EQ(ret1.sessionId, "3427743244");
    EXPECT_EQ(ret1.sessionVersion, 1084119141);
    EXPECT_EQ(ret1.unicastAddr, "127.0.0.1");
    EXPECT_EQ(ret1.username, "HWPSS");

    auto ret2 = rtspSdp->GetName();
    EXPECT_EQ(ret2, "test1.mp4");
    auto ret3 = rtspSdp->GetInfo();
    EXPECT_EQ(ret3, "test");
    auto ret4 = rtspSdp->GetUri();
    EXPECT_EQ(ret4, "uri");
    auto ret5 = rtspSdp->GetEmail();
    EXPECT_EQ(ret5, "email");
    auto ret6 = rtspSdp->GetPhone();
    EXPECT_EQ(ret6, "123456");
    auto ret7 = rtspSdp->GetConnection();
    EXPECT_EQ(ret7, "connection");
    auto ret8 = rtspSdp->GetBandwidth();
    EXPECT_EQ(ret8[0], "X-YZ:128");
    auto ret9 = rtspSdp->GetTime();
    EXPECT_EQ(ret9.first, 121212);
    EXPECT_EQ(ret9.second, 232322);
    auto ret10 = rtspSdp->getAttributes();
    EXPECT_EQ(ret10[0], "attr");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_077, Function | SmallTest | Level2)
{
    auto rtspUrl = std::make_shared<RtspUrl>();
    EXPECT_NE(rtspUrl, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_078, Function | SmallTest | Level2)
{
    const std::string url = "rtsp://admin:12345@172.6.22.106:554/h264/ch33/main/av_stream";
    auto rtspUrl = std::make_shared<RtspUrl>(url);
    EXPECT_NE(rtspUrl, nullptr);
}

HWTEST_F(RtspUnitTest, RtspUnitTest_079, Function | SmallTest | Level2)
{
    auto rtspUrl = std::make_shared<RtspUrl>();
    EXPECT_NE(rtspUrl, nullptr);
    const std::string url = "rtsp://admin:12345@172.6.22.106:554/h264/ch33/main/av_stream";
    auto ret = rtspUrl->Parse(url);
    EXPECT_NE(ret, false);
    auto host = rtspUrl->GetHost();
    EXPECT_EQ(host, "172.6.22.106");
    auto path = rtspUrl->GetPath();
    EXPECT_EQ(path, "/h264/ch33/main/av_stream");
    auto port = rtspUrl->GetPort();
    EXPECT_EQ(port, 554);
    auto userName = rtspUrl->GetUsername();
    EXPECT_EQ(userName, "admin");
    auto password = rtspUrl->GetPassword();
    EXPECT_EQ(password, "12345");
}

HWTEST_F(RtspUnitTest, RtspUnitTest_080, Function | SmallTest | Level2)
{
    auto rtspUrl = std::make_shared<RtspUrl>();
    EXPECT_NE(rtspUrl, nullptr);
    const std::string url = "";
    auto ret = rtspUrl->Parse(url);
    EXPECT_NE(ret, true);
}

} // namespace
} // namespace Sharing
} // namespace OHOS