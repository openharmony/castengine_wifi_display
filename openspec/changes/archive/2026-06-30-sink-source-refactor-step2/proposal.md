## Why

Step1 完成了 Sink/Source 专属文件的物理搬迁和 BUILD.gn 条件编译配置，但公共模块中仍有大量 Sink-only / Source-only 代码混居：codec 混合类（AudioAACCodec/AudioG711Codec/CodecFactory）、RTP 模块全量混合（编解码器/工厂/工具）、hisysevent 打点事件、WFD 定义文件（wfd_def/wfd_session_def/wfd_media_def）。Step2 需要将这些单端专用代码拆分到各自角色目录，完成 Sink/Source 代码角色隔离。

## What Changes

- **codec 混合类拆分**：AudioAACCodec → AudioAACDecoder(Sink) + AudioAACEncoder(Source)；AudioG711Codec → AudioG711Decoder(Sink) + AudioG711Encoder(Source)；CodecFactory → SinkCodecFactory + SourceCodecFactory；G711_TYPE 枚举两端各复制一份；原公共文件删除
- **hisysevent 拆分**：sharing_hisysevent(Source-only) → source/common/；sharing_sink_hisysevent(Sink-only) → sink/common/；条件块编进 sharing_common.z.so
- **RTP 模块全面拆分**：
  - rtp_codec.h → rtp_decoder.h(Sink) + rtp_encoder.h(Source)
  - rtp_factory → RtpSinkFactory(Sink) + RtpSourceFactory(Source)
  - 4 个 codec 混合文件 → 4 Decoder(Sink) + 4 Encoder(Source)（含 FuFlags 复制）
  - rtp_unpack.h + rtp_unpack_impl → sink/protocol/rtp/
  - rtp_pack.h + rtp_pack_impl → source/protocol/rtp/
  - rtp_queue → sink/protocol/rtp/；rtp_maker → source/protocol/rtp/；adts → sink/protocol/rtp/
  - 共享 protocol/rtp/ 只留 rtp_def.h + rtp_packet.h/.cpp
- **wfd_def.h 拆分**：SinkConfig/SourceConfig 不拆分（留 wfd_def.h 公共）；DevSurfaceItem/WfdParamsInfo → sink_def.h；共享定义留公共
- **wfd_session_def.h 拆分**：WfdSinkSessionEventMsg → sink_session_def.h；WfdSourceSessionEventMsg → source_session_def.h；共享定义留公共
- **wfd_media_def.h 拆分**：WfdConsumerEventMsg → sink_media_def.h（修复 Ptr bug）；WfdProducerEventMsg → source_media_def.h；原文件删除
- **BUILD.gn 全面重组**：sharing_codec/sharing_common/sharing_rtp 的 sources 和条件块均需更新

## Capabilities

### New Capabilities

- `codec-class-split`: Sink/Source codec 模块内同类文件的物理拆分——将同文件中的 Sink 类和 Source 类分别移到 sink/codec/ 和 source/codec/，CodecFactory 拆成 SinkCodecFactory 和 SourceCodecFactory

### Modified Capabilities

- `sink-module-isolation`: 新增 codec(AAC/G711 Decoder + SinkCodecFactory)、hisysevent_sink、RTP Sink 全模块（rtp_decoder/rtp_sink_factory/4 Decoder/rtp_queue/adts/rtp_unpack/rtp_unpack_impl）、DevSurfaceItem/WfdParamsInfo、WfdSinkSessionEventMsg、WfdConsumerEventMsg（SinkConfig 仍留 wfd_def.h 公共）
- `source-module-isolation`: 新增 codec(AAC/G711 Encoder + SourceCodecFactory)、sharing_hisysevent、RTP Source 全模块（rtp_encoder/rtp_source_factory/4 Encoder/rtp_maker/rtp_pack/rtp_pack_impl）、WfdSourceSessionEventMsg、WfdProducerEventMsg（SourceConfig 仍留 wfd_def.h 公共）

## Impact

- **文件变更**：删除 11 个原混合文件（3 codec + rtp_factory + rtp_codec + 4 rtp_codec + wfd_media_def），新建 ~25 个角色专属文件，移动 ~18 个整文件
- **API 变更**：CodecFactory → SinkCodecFactory/SourceCodecFactory；RtpFactory → RtpSinkFactory/RtpSourceFactory（**BREAKING** for internal API）
- **BUILD.gn**：sharing_codec、sharing_common、sharing_rtp 三大共享库全面重组
- **bug 修复**：WfdConsumerEventMsg::Ptr 别名 bug
- **枚举复制**：G711_TYPE(Sink+Source 各一份)、FuFlags(Sink+Source 各一份)
