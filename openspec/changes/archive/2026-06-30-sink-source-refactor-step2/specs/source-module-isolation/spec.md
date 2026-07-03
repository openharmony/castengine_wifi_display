## MODIFIED Requirements

### Requirement: Source-only code physical separation
Source-only 文件/目录 SHALL 从当前位置移动或拆分到 `services/source/` 下，Step1 原有清单基础上追加以下整文件/类拆分：

| 原路径 | 目标路径 | 类型 |
|--------|----------|------|
| `codec/include/audio_aac_codec.h/.cpp` 中 AudioAACEncoder | `source/codec/include/audio_aac_encoder.h` + `source/codec/src/audio_aac_encoder.cpp` | 类拆分 |
| `codec/include/audio_g711_codec.h/.cpp` 中 AudioG711Encoder+G711_TYPE | `source/codec/include/audio_g711_encoder.h` + `source/codec/src/audio_g711_encoder.cpp` | 类拆分 |
| `codec/include/codec_factory.h/.cpp` 中 Source 部分 | `source/codec/include/source_codec_factory.h` + `source/codec/src/source_codec_factory.cpp` | 类拆分（SourceCodecFactory 新类） |
| `common/sharing_hisysevent.h/.cpp` | `source/common/include/` + `source/common/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec.h` 中 RtpEncoder | `source/protocol/rtp/include/rtp_encoder.h` | 类拆分 |
| `protocol/rtp/include/rtp_factory.h/.cpp` 中 CreateRtpPack | `source/protocol/rtp/include/rtp_source_factory.h` + `source/protocol/rtp/src/rtp_source_factory.cpp` | 类拆分（RtpSourceFactory） |
| `protocol/rtp/include/rtp_pack.h` | `source/protocol/rtp/include/rtp_pack.h` | 整文件移动 |
| `protocol/rtp/include/rtp_pack_impl.h/.cpp` | `source/protocol/rtp/include/` + `source/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_maker.h/.cpp` | `source/protocol/rtp/include/` + `source/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec_h264.h/.cpp` 中 RtpEncoderH264 | `source/protocol/rtp/include/rtp_encoder_h264.h` + `source/protocol/rtp/src/rtp_encoder_h264.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_ts.h/.cpp` 中 RtpEncoderTs | `source/protocol/rtp/include/rtp_encoder_ts.h` + `source/protocol/rtp/src/rtp_encoder_ts.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_aac.h/.cpp` 中 RtpEncoderAAC | `source/protocol/rtp/include/rtp_encoder_aac.h` + `source/protocol/rtp/src/rtp_encoder_aac.cpp` | 类拆分（死代码保留） |
| `protocol/rtp/include/rtp_codec_g711.h/.cpp` 中 RtpEncoderG711 | `source/protocol/rtp/include/rtp_encoder_g711.h` + `source/protocol/rtp/src/rtp_encoder_g711.cpp` | 类拆分 |
| `impl/scene/wfd/wfd_def.h` 中 SourceConfig | `source/impl/scene/wfd/include/source_def.h` | 类拆分 |
| `impl/wfd/wfd_session_def.h` 中 WfdSourceSessionEventMsg | `source/impl/wfd/include/source_session_def.h` | 类拆分 |
| `impl/wfd/wfd_media_def.h` 中 WfdProducerEventMsg | `source/impl/wfd/include/source_media_def.h` | 类拆分 |

#### Scenario: Source codec files split to source/codec/
- **WHEN** codec 拆分完成
- **THEN** `source/codec/include/audio_aac_encoder.h/.cpp`、`audio_g711_encoder.h/.cpp`、`source_codec_factory.h/.cpp` SHALL 存在

#### Scenario: Source hisysevent moved to source/common/
- **WHEN** hisysevent 拆分完成
- **THEN** `sharing_hisysevent.h/.cpp` SHALL 存在于 `services/source/common/include/` 和 `services/source/common/src/`

#### Scenario: RTP Source module fully isolated
- **WHEN** RTP 拆分完成
- **THEN** `source/protocol/rtp/` SHALL 包含 rtp_encoder.h、rtp_source_factory.h/.cpp、rtp_pack.h、rtp_pack_impl.h/.cpp、rtp_maker.h/.cpp、rtp_encoder_h264/ts/aac/g711.h/.cpp

#### Scenario: SourceConfig in source_def.h
- **WHEN** wfd_def.h 拆分完成
- **THEN** `source/impl/scene/wfd/include/source_def.h` SHALL 包含 SourceConfig

#### Scenario: wfd_media_def.h deleted
- **WHEN** wfd_media_def.h 拆分完成
- **THEN** `services/impl/wfd/wfd_media_def.h` SHALL 被删除

### Requirement: Source-only BUILD.gn targets created

| 变更 | 内容 |
|------|------|
| `source/common/BUILD.gn` (新建) | `sharing_source_common_srcs` source_set |
| `source/protocol/rtp/BUILD.gn` (新建) | `sharing_source_rtp_srcs` source_set，包含所有 Source RTP 源文件 |
| `services/codec/BUILD.gn` (修改) | source 条件块添加 source/codec 角色专属 .cpp |
| `services/common/BUILD.gn` (修改) | source 条件 deps 增加 source/common |
| `services/protocol/rtp/BUILD.gn` (修改) | source 条件 deps 增加 source/protocol/rtp |

#### Scenario: Source codec compiled via sharing_codec.z.so
- **WHEN** `wifi_display_support_source=true`
- **THEN** sharing_codec source 条件块 SHALL 包含 source/codec 角色专属 .cpp

#### Scenario: Source RTP compiled via sharing_rtp.z.so
- **WHEN** `wifi_display_support_source=true`
- **THEN** sharing_rtp source 条件块 SHALL 包含 source/protocol/rtp 所有 Source RTP 源文件
