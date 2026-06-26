## 1. 目录结构创建

- [x] 1.1 创建 `services/sink/common/include/` + `services/sink/common/src/`
- [x] 1.2 创建 `services/source/common/include/` + `services/source/common/src/`
- [x] 1.3 创建 `services/sink/protocol/rtp/include/` + `services/sink/protocol/rtp/src/`
- [x] 1.4 创建 `services/source/protocol/rtp/include/` + `services/source/protocol/rtp/src/`
- [x] 1.5 创建 `services/sink/impl/scene/wfd/include/`
- [x] 1.6 创建 `services/source/impl/scene/wfd/include/`
- [x] 1.7 创建 `services/sink/impl/wfd/include/`
- [x] 1.8 创建 `services/source/impl/wfd/include/`

## 2. Codec 混合类拆分

### 2a. AudioAACDecoder → sink/codec

- [x] 2a.1 创建 `services/sink/codec/include/audio_aac_decoder.h`：提取 AudioAACDecoder 类定义
- [x] 2a.2 创建 `services/sink/codec/src/audio_aac_decoder.cpp`：提取 AudioAACDecoder 全部实现

### 2b. AudioAACEncoder → source/codec

- [x] 2b.1 创建 `services/source/codec/include/audio_aac_encoder.h`：提取 AudioAACEncoder 类定义
- [x] 2b.2 创建 `services/source/codec/src/audio_aac_encoder.cpp`：提取 AudioAACEncoder 全部实现
- [x] 2b.3 删除 `services/codec/include/audio_aac_codec.h` + `services/codec/src/audio_aac_codec.cpp`

### 2c. AudioG711Decoder → sink/codec

- [x] 2c.1 创建 `services/sink/codec/include/audio_g711_decoder.h`：G711_TYPE 枚举 + AudioG711Decoder
- [x] 2c.2 创建 `services/sink/codec/src/audio_g711_decoder.cpp`：AudioG711Decoder 全部实现

### 2d. AudioG711Encoder → source/codec

- [x] 2d.1 创建 `services/source/codec/include/audio_g711_encoder.h`：G711_TYPE 枚举 + AudioG711Encoder
- [x] 2d.2 创建 `services/source/codec/src/audio_g711_encoder.cpp`：AudioG711Encoder 全部实现
- [x] 2d.3 删除 `services/codec/include/audio_g711_codec.h` + `services/codec/src/audio_g711_codec.cpp`

### 2e. SinkCodecFactory → sink/codec

- [x] 2e.1 创建 `services/sink/codec/include/sink_codec_factory.h`：SinkCodecFactory 类定义
- [x] 2e.2 创建 `services/sink/codec/src/sink_codec_factory.cpp`：实现 CreateAudioDecoder/CreateVideoDecoder，#ifdef 保护

### 2f. SourceCodecFactory → source/codec

- [x] 2f.1 创建 `services/source/codec/include/source_codec_factory.h`：SourceCodecFactory 类定义
- [x] 2f.2 创建 `services/source/codec/src/source_codec_factory.cpp`：实现 CreateAudioEncoder/CreateVideoEncoder，#ifdef 保护
- [x] 2f.3 删除 `services/codec/include/codec_factory.h` + `services/codec/src/codec_factory.cpp`

### 2g. Codec 生产代码调用方更新

- [x] 2g.1 `services/sink/mediaplayer/src/audio_player.cpp`：CodecFactory → SinkCodecFactory
- [x] 2g.2 `services/source/impl/screen_capture/screen_capture_consumer.cpp`：CodecFactory → SourceCodecFactory

## 3. hisysevent 拆分

- [x] 3.1 移 sharing_sink_hisysevent.h → sink/common/include/
- [x] 3.2 移 sharing_sink_hisysevent.cpp → sink/common/src/
- [x] 3.3 移 sharing_hisysevent.h → source/common/include/
- [x] 3.4 移 sharing_hisysevent.cpp → source/common/src/
- [x] 3.5 删除原位置 hisysevent 文件
- [x] 3.6 更新 Sink 侧 hisysevent 消费方 include（wfd_sink_scene.h、wfd_rtp_consumer.cpp、wfd_sink_session.cpp、video_sink_decoder.cpp、audio_avcodec_decoder.cpp、video_play_controller.cpp、agent.cpp）

## 4. RTP 接口拆分（rtp_codec.h → rtp_decoder.h + rtp_encoder.h）

- [x] 4.1 创建 `services/sink/protocol/rtp/include/rtp_decoder.h`：RtpDecoder 抽象基类 + OnFrame
- [x] 4.2 创建 `services/source/protocol/rtp/include/rtp_encoder.h`：RtpEncoder 抽象基类 + OnRtpPack
- [x] 4.3 删除 `services/protocol/rtp/include/rtp_codec.h`

## 5. RTP 工厂拆分（rtp_factory → RtpSinkFactory + RtpSourceFactory）

- [x] 5.1 创建 `services/sink/protocol/rtp/include/rtp_sink_factory.h` + `src/rtp_sink_factory.cpp`
- [x] 5.2 创建 `services/source/protocol/rtp/include/rtp_source_factory.h` + `src/rtp_source_factory.cpp`
- [x] 5.3 删除 `services/protocol/rtp/include/rtp_factory.h` + `src/rtp_factory.cpp`
- [x] 5.4 更新 wfd_rtp_consumer：RtpFactory → RtpSinkFactory
- [x] 5.5 更新 wfd_rtp_producer：RtpFactory → RtpSourceFactory

## 6. RTP 接口+实现+工具整文件移动

- [x] 6.1 移 rtp_unpack.h → sink/protocol/rtp/include/
- [x] 6.2 移 rtp_unpack_impl.h/.cpp → sink/protocol/rtp/（cpp 更新 hisysevent include）
- [x] 6.3 移 rtp_pack.h → source/protocol/rtp/include/
- [x] 6.4 移 rtp_pack_impl.h/.cpp → source/protocol/rtp/
- [x] 6.5 移 rtp_queue.h/.cpp → sink/protocol/rtp/
- [x] 6.6 移 rtp_maker.h/.cpp → source/protocol/rtp/
- [x] 6.7 移 adts.h/.cpp → sink/protocol/rtp/
- [x] 6.8 删除原位置所有已移文件

## 7. RTP codec 混合文件拆分（4→8）

### 7a. rtp_codec_h264 → rtp_decoder_h264 + rtp_encoder_h264

- [x] 7a.1 创建 `sink/protocol/rtp/include/rtp_decoder_h264.h` + `src/rtp_decoder_h264.cpp`（含 FuFlags 复制）
- [x] 7a.2 创建 `source/protocol/rtp/include/rtp_encoder_h264.h` + `src/rtp_encoder_h264.cpp`（含 FuFlags 复制）
- [x] 7a.3 删除 `services/protocol/rtp/include/rtp_codec_h264.h` + `src/rtp_codec_h264.cpp`

### 7b. rtp_codec_ts → rtp_decoder_ts + rtp_encoder_ts

- [x] 7b.1 创建 `sink/protocol/rtp/include/rtp_decoder_ts.h` + `src/rtp_decoder_ts.cpp`
- [x] 7b.2 创建 `source/protocol/rtp/include/rtp_encoder_ts.h` + `src/rtp_encoder_ts.cpp`
- [x] 7b.3 删除 `services/protocol/rtp/include/rtp_codec_ts.h` + `src/rtp_codec_ts.cpp`

### 7c. rtp_codec_aac → rtp_decoder_aac + rtp_encoder_aac

- [x] 7c.1 创建 `sink/protocol/rtp/include/rtp_decoder_aac.h` + `src/rtp_decoder_aac.cpp`
- [x] 7c.2 创建 `source/protocol/rtp/include/rtp_encoder_aac.h` + `src/rtp_encoder_aac.cpp`（死代码保留）
- [x] 7c.3 删除 `services/protocol/rtp/include/rtp_codec_aac.h` + `src/rtp_codec_aac.cpp`

### 7d. rtp_codec_g711 → rtp_decoder_g711 + rtp_encoder_g711

- [x] 7d.1 创建 `sink/protocol/rtp/include/rtp_decoder_g711.h` + `src/rtp_decoder_g711.cpp`
- [x] 7d.2 创建 `source/protocol/rtp/include/rtp_encoder_g711.h` + `src/rtp_encoder_g711.cpp`
- [x] 7d.3 删除 `services/protocol/rtp/include/rtp_codec_g711.h` + `src/rtp_codec_g711.cpp`

### 7e. RTP 内部 include 更新

- [x] 7e.1 rtp_unpack_impl.cpp：codec includes → decoder includes
- [x] 7e.2 rtp_pack_impl.cpp：codec includes → encoder includes

## 8. wfd_def.h 拆分（SinkConfig/SourceConfig 不拆，留 wfd_def.h）

- [x] 8.1 创建 sink_def.h：DevSurfaceItem + WfdParamsInfo（SinkConfig 留 wfd_def.h）
- [x] 8.2 ~~创建 source_def.h~~：SourceConfig 留 wfd_def.h，source_def.h 已删除
- [x] 8.3 从 wfd_def.h 删除 DevSurfaceItem、WfdParamsInfo（SinkConfig/SourceConfig 保留）
- [x] 8.4 更新 Sink 侧消费方 include（wfd_sink_scene.h、wfd_trust_list_manager.h 仍需 #include "sink_def.h" for DevSurfaceItem/WfdParamsInfo）
- [x] 8.5 更新 Source 侧消费方 include（wfd_source_scene.h：移除 #include "source_def.h"，改为 #include "wfd_def.h"）

## 9. wfd_session_def.h 拆分

- [x] 9.1 创建 sink_session_def.h：WfdSinkSessionEventMsg
- [x] 9.2 创建 source_session_def.h：WfdSourceSessionEventMsg
- [x] 9.3 从 wfd_session_def.h 删除两个角色 struct
- [x] 9.4 更新 Sink 侧消费方 include
- [x] 9.5 更新 Source 侧消费方 include

## 10. wfd_media_def.h 拆分

- [x] 10.1 创建 sink_media_def.h：WfdConsumerEventMsg（修复 Ptr bug）
- [x] 10.2 创建 source_media_def.h：WfdProducerEventMsg
- [x] 10.3 删除 wfd_media_def.h
- [x] 10.4 更新 Sink/Source 侧消费方 include

## 11. BUILD.gn 全面重组

- [x] 11.1 创建 `services/sink/common/BUILD.gn`：sharing_sink_common_srcs
- [x] 11.2 创建 `services/source/common/BUILD.gn`：sharing_source_common_srcs
- [x] 11.3 创建 `services/sink/protocol/rtp/BUILD.gn`：sharing_sink_rtp_srcs（8 个 Sink RTP .cpp）
- [x] 11.4 创建 `services/source/protocol/rtp/BUILD.gn`：sharing_source_rtp_srcs（7 个 Source RTP .cpp）
- [x] 11.5 更新 `services/codec/BUILD.gn`：删除 3 公共 .cpp，sink/source 条件块添加角色 .cpp
- [x] 11.6 更新 `services/common/BUILD.gn`：移除 hisysevent .cpp，添加 Sink/Source 条件 deps
- [x] 11.7 更新 `services/protocol/rtp/BUILD.gn`：全面重组（移除所有角色 .cpp，只留 rtp_packet.cpp；添加 Sink/Source 条件 deps + include_dirs）
- [x] 11.8 更新 sink/impl/scene/wfd、sink/impl/wfd、source/impl/scene/wfd、source/impl/wfd BUILD.gn include_dirs
- [x] 11.9 更新 sink/BUILD.gn、source/BUILD.gn deps
- [x] 11.10 更新 `services/impl/wfd/BUILD.gn`：移除 wfd_media_def.h（无需改动，未引用旧文件）
- [x] 11.11 处理 agent BUILD.gn 条件 include_dirs（无需改动，agent 不引用已拆分头文件）

## 12. 测试/Demo 代码更新

- [x] 12.1 Codec 测试：CodecFactory → SinkCodecFactory/SourceCodecFactory，include 更新
- [x] 12.2 RTP 测试：RtpFactory → RtpSinkFactory/RtpSourceFactory，include 更新
- [x] 12.3 WFD 测试：session_def/media_def include 更新
- [x] 12.4 测试 .cpp 旧 include 路径修复：rtp_unit_test.cpp (adts/rtp_maker/rtp_pack/rtp_pack_impl/rtp_queue/rtp_unpack_impl)、mock_rtp_pack.h、mock_rtp_unpack.h、wfd_sink_rtsp_fuzzer.cpp
- [x] 12.5 测试 BUILD.gn include_dirs 修复：rtp_unit_test、impl/rtp (consumer+producer)、impl/session (sink+source)、wfdsinkrtsp_fuzzer、demo (rtp/codec/player/rtsp)

## 13. 编译验证（每个阶段完成后执行）

编译命令：`cd /home/sunhy/openharmony/code && ./build.sh --product-name rk3568 --build-target sharing_framework`

- [x] 13.0 编译验证基线：确认当前代码（已完成 §2 codec + §3 hisysevent + §8-10 wfd_def 文件搬迁但 BUILD.gn 未更新）能编译通过（先修复 BUILD.gn，再验证）
- [x] 13.1 编译验证 §4-5：RTP 接口拆分 + 工厂拆分完成后编译通过
- [x] 13.2 编译验证 §6：RTP 接口+实现+工具移动完成后编译通过
- [x] 13.3 编译验证 §7：RTP codec 混合文件拆分完成后编译通过
- [x] 13.4 编译验证 §11：BUILD.gn 全面重组完成后编译通过
- [x] 13.5 编译验证 §12：测试/Demo 代码更新完成后编译通过
- [ ] 13.6 最终编译验证：sharing_framework (non-NAPI) ✅；sharing_packages 仅 NAPI Step1 bug；测试目标 tests:test ✅、wfd_unit_test ✅、wfd_sink_fuzz_test ✅

## 14. 代码验证

- [x] 14.1 确认 services/codec/ 只剩公共基类头文件 + media_frame_pipeline.cpp ✅（5 公共头 + 1 公共 .cpp）
- [x] 14.2 确认 services/common/ 不再有 hisysevent 文件 ✅（已移到 sink/common + source/common）
- [x] 14.3 确认 services/protocol/rtp/ 只剩 rtp_def.h + rtp_packet.h/.cpp ✅
- [x] 14.4 确认无 CodecFactory/RtpFactory/rtp_codec.h/4 codec 混合文件/wfd_media_def.h 引用 ✅
- [x] 14.5 确认所有 Sink/Source 消费方 include 和类名已更新 ✅
- [x] 14.6 确认 WfdConsumerEventMsg::Ptr 已修正 ✅（shared_ptr<WfdConsumerEventMsg>）
- [x] 14.7 确认 FuFlags + G711_TYPE 各两端独立存在 ✅
