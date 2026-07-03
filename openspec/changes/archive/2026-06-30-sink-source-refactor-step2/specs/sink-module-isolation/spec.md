## MODIFIED Requirements

### Requirement: Sink-only code physical separation
Sink-only 文件/目录 SHALL 从当前位置移动或拆分到 `services/sink/` 下，Step1 原有清单基础上追加以下整文件/类拆分：

| 原路径 | 目标路径 | 类型 |
|--------|----------|------|
| `codec/include/audio_aac_codec.h/.cpp` 中 AudioAACDecoder | `sink/codec/include/audio_aac_decoder.h` + `sink/codec/src/audio_aac_decoder.cpp` | 类拆分 |
| `codec/include/audio_g711_codec.h/.cpp` 中 AudioG711Decoder+G711_TYPE | `sink/codec/include/audio_g711_decoder.h` + `sink/codec/src/audio_g711_decoder.cpp` | 类拆分 |
| `codec/include/codec_factory.h/.cpp` 中 Sink 部分 | `sink/codec/include/sink_codec_factory.h` + `sink/codec/src/sink_codec_factory.cpp` | 类拆分（SinkCodecFactory 新类） |
| `common/sharing_sink_hisysevent.h/.cpp` | `sink/common/include/` + `sink/common/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec.h` 中 RtpDecoder | `sink/protocol/rtp/include/rtp_decoder.h` | 类拆分 |
| `protocol/rtp/include/rtp_factory.h/.cpp` 中 CreateRtpUnpack | `sink/protocol/rtp/include/rtp_sink_factory.h` + `sink/protocol/rtp/src/rtp_sink_factory.cpp` | 类拆分（RtpSinkFactory） |
| `protocol/rtp/include/rtp_unpack.h` | `sink/protocol/rtp/include/rtp_unpack.h` | 整文件移动 |
| `protocol/rtp/include/rtp_unpack_impl.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_queue.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/adts.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec_h264.h/.cpp` 中 RtpDecoderH264 | `sink/protocol/rtp/include/rtp_decoder_h264.h` + `sink/protocol/rtp/src/rtp_decoder_h264.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_ts.h/.cpp` 中 RtpDecoderTs | `sink/protocol/rtp/include/rtp_decoder_ts.h` + `sink/protocol/rtp/src/rtp_decoder_ts.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_aac.h/.cpp` 中 RtpDecoderAAC | `sink/protocol/rtp/include/rtp_decoder_aac.h` + `sink/protocol/rtp/src/rtp_decoder_aac.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_g711.h/.cpp` 中 RtpDecoderG711 | `sink/protocol/rtp/include/rtp_decoder_g711.h` + `sink/protocol/rtp/src/rtp_decoder_g711.cpp` | 类拆分 |
| `impl/scene/wfd/wfd_def.h` 中 SinkConfig,DevSurfaceItem,WfdParamsInfo | `sink/impl/scene/wfd/include/sink_def.h` | 类拆分 |
| `impl/wfd/wfd_session_def.h` 中 WfdSinkSessionEventMsg | `sink/impl/wfd/include/sink_session_def.h` | 类拆分 |
| `impl/wfd/wfd_media_def.h` 中 WfdConsumerEventMsg | `sink/impl/wfd/include/sink_media_def.h` | 类拆分（修复 Ptr bug） |

#### Scenario: Sink codec files split to sink/codec/
- **WHEN** codec 拆分完成
- **THEN** `sink/codec/include/audio_aac_decoder.h/.cpp`、`audio_g711_decoder.h/.cpp`、`sink_codec_factory.h/.cpp` SHALL 存在，且 `services/codec/include/audio_aac_codec.h/.cpp`、`audio_g711_codec.h/.cpp`、`codec_factory.h/.cpp` SHALL 不再存在

#### Scenario: Sink hisysevent moved to sink/common/
- **WHEN** hisysevent 拆分完成
- **THEN** `sharing_sink_hisysevent.h/.cpp` SHALL 存在于 `services/sink/common/include/` 和 `services/sink/common/src/`

#### Scenario: RTP Sink module fully isolated
- **WHEN** RTP 拆分完成
- **THEN** `sink/protocol/rtp/` SHALL 包含 rtp_decoder.h、rtp_sink_factory.h/.cpp、rtp_unpack.h、rtp_unpack_impl.h/.cpp、rtp_queue.h/.cpp、adts.h/.cpp、rtp_decoder_h264/ts/aac/g711.h/.cpp，且 `services/protocol/rtp/` SHALL 只剩 rtp_def.h、rtp_packet.h/.cpp

#### Scenario: SinkConfig/DevSurfaceItem/WfdParamsInfo in sink_def.h
- **WHEN** wfd_def.h 拆分完成
- **THEN** `sink/impl/scene/wfd/include/sink_def.h` SHALL 包含这三个 struct，且 `wfd_def.h` SHALL 不再包含它们

#### Scenario: WfdConsumerEventMsg::Ptr bug fixed
- **WHEN** wfd_media_def.h 拆分完成
- **THEN** `sink_media_def.h` 中 WfdConsumerEventMsg::Ptr SHALL 为 `std::shared_ptr<WfdConsumerEventMsg>`（而非 `std::shared_ptr<WfdProducerEventMsg>`）

### Requirement: Sink-only BUILD.gn targets created

| 变更 | 内容 |
|------|------|
| `sink/common/BUILD.gn` (新建) | `sharing_sink_common_srcs` source_set |
| `sink/protocol/rtp/BUILD.gn` (新建) | `sharing_sink_rtp_srcs` source_set，包含所有 Sink RTP 源文件 |
| `services/codec/BUILD.gn` (修改) | sink 条件块添加 sink/codec 角色专属 .cpp |
| `services/common/BUILD.gn` (修改) | sink 条件 deps 增加 sink/common |
| `services/protocol/rtp/BUILD.gn` (修改) | sink 条件 deps 增加 sink/protocol/rtp |

#### Scenario: Sink codec compiled via sharing_codec.z.so
- **WHEN** `wifi_display_support_sink=true`
- **THEN** sharing_codec sink 条件块 SHALL 包含 sink/codec 角色专属 .cpp

#### Scenario: Sink RTP compiled via sharing_rtp.z.so
- **WHEN** `wifi_display_support_sink=true`
- **THEN** sharing_rtp sink 条件块 SHALL 包含 sink/protocol/rtp 所有 Sink RTP 源文件
