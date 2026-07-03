## Context

Step1 已完成 Sink/Source 专属 codec/session/scene/agent/mediaplayer 的物理搬迁。当前公共模块中仍有以下混居问题：

| 模块 | 文件 | 角色 | 问题 |
|------|------|------|------|
| codec | audio_aac_codec.h/.cpp | Sink Decoder + Source Encoder | 同文件混居 |
| codec | audio_g711_codec.h/.cpp | Sink Decoder + Source Encoder + G711_TYPE | 同文件混居 |
| codec | codec_factory.h/.cpp | Sink 工厂方法 + Source 工厂方法 | 同类混居 |
| common | sharing_hisysevent.h/.cpp | Source-only | 在 common/ |
| common | sharing_sink_hisysevent.h/.cpp | Sink-only | 在 common/ |
| rtp | rtp_codec.h | RtpDecoder(Sink) + RtpEncoder(Source) | 同文件混居 |
| rtp | rtp_factory.h/.cpp | CreateRtpUnpack(Sink) + CreateRtpPack(Source) | 同类混居 |
| rtp | rtp_codec_h264/ts/aac/g711 | Decoder(Sink) + Encoder(Source) | 4 组同文件混居 |
| rtp | rtp_unpack.h + impl | Sink-only | 在公共 protocol/rtp/ |
| rtp | rtp_pack.h + impl | Source-only | 在公共 protocol/rtp/ |
| rtp | rtp_queue | Sink-only | 在公共 protocol/rtp/ |
| rtp | rtp_maker | Source-only | 在公共 protocol/rtp/ |
| rtp | adts | Sink-oriented | 在公共 protocol/rtp/ |
| wfd | wfd_def.h | SinkConfig/DevSurfaceItem/WfdParamsInfo(Sink) + SourceConfig(Source) + 共享 | 同文件混居 |
| wfd | wfd_session_def.h | WfdSink/SourceSessionEventMsg + 共享 | 同文件混居 |
| wfd | wfd_media_def.h | WfdConsumer(Sink) + WfdProducer(Source) | 同文件混居 |

## Goals / Non-Goals

**Goals:**
- 将 codec、hisysevent、RTP 全模块、wfd_def/session_def/media_def 中的 Sink-only / Source-only 定义拆分到对应角色目录
- 更新所有消费方的 #include 路径和类名引用
- 更新 BUILD.gn 将新位置文件编入条件编译体系
- 修复 WfdConsumerEventMsg::Ptr 别名 bug

**Non-Goals:**
- 不拆 wfd_msg.h（留后续 Step）
- 不改变类的行为逻辑
- 不处理共享定义（ConnectionState、WfdSceneEventMsg、rtp_def.h、rtp_packet.h 等留公共不动）

## Decisions

### D1: CodecFactory → SinkCodecFactory + SourceCodecFactory（方案 A 独立类）

**理由**: 彻底角色隔离，类名表达归属。调用方零歧义。

**替代**: 方案 B 仅拆 .cpp → CodecFactory 类声明仍在公共层，未真正隔离

### D2: G711_TYPE 两端各复制一份 vs 新建公共头文件

**选择**: 两端各复制一份

**理由**: 2 值枚举复制成本极低，比新建碎片化头文件更简洁

### D3: #ifdef 保护策略

**选择**: SinkCodecFactory/SourceCodecFactory 中角色专属 #include 和创建逻辑加 #ifdef 保护

**理由**: 与 Step1 codec_factory.cpp 已有模式一致

### D4: AudioAACDecoder 保留（死代码）

**理由**: AAC 解码 ffmpeg 实现保留备用

### D5: WfdParamsInfo 放 sink_def.h

**理由**: Sink-only 使用模式，遵循隔离原则

### D6: hisysevent 移到角色目录 vs 留 common/ 加 ifdef

**选择**: 移到角色目录（sink/common/ + source/common/）

### D7: rtp_codec.h → rtp_decoder.h + rtp_encoder.h

**理由**: RtpDecoder 和 RtpEncoder 零交叉依赖，各自只服务一侧

### D8: rtp_factory → RtpSinkFactory + RtpSourceFactory，原文件删除

**理由**: 纯静态工厂，Sink/Source 方法无共享实现。.cpp 需 include 两端 impl，无法留在公共

**替代 A**: 留 rtp_factory.h 公共只拆 .cpp — 不可行（.cpp 需两端 include）
**替代 B**: facade 模式 — 不必要，消费方已是角色特化的

### D9: 4 个 rtp codec 混合文件 → 8 个独立 Decoder/Encoder

**理由**: 与 codec 步骤 AAC/G711 拆分模式完全一致。Decoder 只被 UnpackImpl(Sink) 用，Encoder 只被 PackImpl(Source) 用

**注意**: FuFlags struct(H264) 两端各复制一份，与 G711_TYPE 原则一致

### D10: rtp_unpack.h/rtp_pack.h 移到角色目录 vs 留公共

**选择**: 移到角色目录（删除 rtp_factory 后无共享消费者）

### D11: rtp_queue/rtp_maker/adts 移到角色目录

**理由**: 消费方都是角色特化的

### D12: BUILD.gn 编进策略 — 条件块编进现有 .z.so

**理由**: 不新建碎片化共享库。Sink 文件 wifi_display_support_sink 条件，Source 文件 wifi_display_support_source 条件

### D13: wfd_media_def.h 拆完后删除

**理由**: 拆分后无共享定义

### D14: SourceConfig / RtpEncoderAAC / AudioAACDecoder 保留

**理由**: 死代码保留备用原则

## Risks / Trade-offs

- [变更规模大] → 涉及 codec + hisysevent + RTP + wfd_def 四大模块，~25 新建文件 + ~11 删除 + ~18 移动。按依赖序分步执行降低风险。
- [FuFlags + G711_TYPE 枚举复制] → 两端需同步修改。当前极小（FuFlags ~4 值，G711_TYPE ~2 值），风险可接受。
- [sharing_codec/sharing_rtp 内容大幅缩减] → 验证条件块正确覆盖所有源文件。
- [内部 API BREAKING] → CodecFactory 和 RtpFactory 消失，调用方需改类名和 include。影响可控（生产代码各 ~2 文件 + 测试 ~5 文件）。
