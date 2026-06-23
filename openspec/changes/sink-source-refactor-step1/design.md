## Context

OpenHarmony castengine_wifi_display 项目的 `services/` 目录下，Sink-only（接收端）和 Source-only（发送端）代码与公共代码混在一起。当前无法通过 `wifi_display_support_sink` 和 `wifi_display_support_source` 编译开关实现独立裁剪，因为所有代码编译在同一个 BUILD.gn target 中，没有按角色物理分离。

当前编译结构问题：
- `mediaplayer/` 100% Sink-only 但无条件编译和依赖
- `impl/screen_capture/` 100% Source-only 但只在 `sharing_framework_support_wfd` 下编译（未细分到 `wifi_display_support_source`）
- `agent/sinkagent/` 和 `agent/srcagent/` 混在同一个 `sharing_agent_srcs` source_set
- Sink/Source codec 实现混在 `sharing_codec` shared_library
- `common/sharing_sink_hisysevent.h/cpp` 被 2 个公共文件（`agent.cpp`、`rtp_unpack_impl.cpp`）交叉引用
- `agent.cpp` 使用未定义的 `#ifdef WFD_SINK` 宏

## Goals / Non-Goals

**Goals:**
- 在 `services/` 下创建 `sink/` 和 `source/` 目录，物理分离整文件归属 Sink/Source 的代码
- 所有移走的文件保持与原代码相对路径一致
- 所有 Sink-only 编译产物仅在 `wifi_display_support_sink=true` 时编译
- 所有 Source-only 编译产物仅在 `wifi_display_support_source=true` 时编译
- 统一 `WFD_SINK`/`WFD_SOURCE` 编译宏定义机制
- 修改所有受影响的 BUILD.gn，用 feature flag 条件化 Sink/Source deps
- 编译产物（so）数量和名称不变

**Non-Goals:**
- 不拆分混合头文件（`wfd_def.h`、`wfd_msg.h`、`wfd_session_def.h`、`wfd_media_def.h`、`audio_aac_codec.h`、`audio_g711_codec.h`、`.inc files`）—— 留 Step 2
- 不移动 `sharing_sink_hisysevent.h/cpp` 和 `sharing_hisysevent.h/cpp` —— 公共代码交叉引用问题留 Step 2/3
- 不处理 `rtp_unpack_impl.cpp` 中 Sink hisysevent 的不当引用 —— 留 Step 2/3
- 不重构 `frameworks/` 和 `interfaces/` 层 —— 不在 services/ 范围内
- 不拆分 `codec_factory` 为 SinkCodecFactory + SourceCodecFactory —— 留 Step 2
- 不拆分 `mediachannel/` 的 BaseConsumer/BaseProducer —— 框架层保持公共
- 不拆分 `agent/agent.h/cpp` 基类 —— 保持公共

## Decisions

### D1: hisysevent 文件暂不移动（思路 B）

**决策**: `sharing_sink_hisysevent.h/cpp` 和 `sharing_hisysevent.h/cpp` 留在 `common/`，Step 1 不移动。

**理由**: 如果移动 `sharing_sink_hisysevent`，公共文件 `agent.cpp` 和 `rtp_unpack_impl.cpp` 的 `#include` 路径必须改为条件引用，违背"Step 1 不引入公共代码的角色条件编译"原则。且 `sharing_common` 是 shared_library，移动后 hisysevent 编译条件化需要改产物结构。

**替代方案**（思路 A/C）: 立即移走并用 `#ifdef` 保护公共代码引用。被否决，因为会增加公共代码的条件编译片段。

### D2: 编译宏命名统一为 WFD_SINK / WFD_SOURCE

**决策**: 使用 `WFD_SINK` 和 `WFD_SOURCE` 作为编译宏名，在 BUILD.gn config 中通过 cflags 定义。

**理由**: 与现有代码风格一致（`agent.cpp:239` 已使用 `#ifdef WFD_SINK`）。宏名简短，比 `WIFI_DISPLAY_SUPPORT_SINK` 更可读。

**实现**: 在 services 顶层 BUILD.gn（或新建独立 config 文件）中定义 `config("wifi_display_feature_flags")`：
```gn
config("wifi_display_feature_flags") {
  if (wifi_display_support_sink) {
    cflags = [ "-DWFD_SINK" ]
  }
  if (wifi_display_support_source) {
    cflags = [ "-DWFD_SOURCE" ]
  }
}
```
需要条件编译的 source_set/shared_library 在 configs 中引入此 config。

### D3: codec_factory.cpp 用 WFD_SINK/WFD_SOURCE 条件编译保护

**决策**: `codec_factory.cpp` 保留在公共 `codec/`，用 `#ifdef WFD_SINK`/`#ifdef WFD_SOURCE` 保护 Sink/Source codec 的 `#include` 和 `switch case` 分支。

**理由**: CodecFactory 是工厂模式，角色分发是其固有职责，不是"公共代码不当引入角色依赖"。且 `CreateVideoEncoder/Decoder` 都是空壳方法，`VideoSourceEncoder`/`VideoSinkDecoder` 不经工厂创建（由各自 Session/Consumer 直接构造）。

**替代方案**: 拆分 CodecFactory 为 SinkCodecFactory + SourceCodecFactory。被否决，Step 1 改动量过大，属于 Step 2 范围。

### D4: 移走的 codec 文件保持 include/src 子目录结构

**决策**: `sink/codec/` 和 `source/codec/` 保持 `include/` 和 `src/` 子目录结构，与原 `codec/` 目录风格一致。

**理由**: 保持代码风格一致性，便于后续 Step 2 拆分混合 codec 文件时保持同样的目录模式。

### D5: mediaplayer 从无条件依赖改为 wifi_display_support_sink 条件依赖

**决策**: `sharing_service` 对 `mediaplayer` 的依赖从 unconditional 改为 `if (wifi_display_support_sink)` 条件。

**理由**: mediaplayer 100% Sink-only（音视频解码渲染），当前无条件依赖是遗留问题。同理 `sharing_codec` 对 mediaplayer 的依赖也条件化。

### D6: screen_capture 从 sharing_framework_support_wfd 条件改为 wifi_display_support_source 条件

**决策**: `sharing_service` 对 `impl/screen_capture` 的依赖从 `if (sharing_framework_support_wfd)` 改为 `if (wifi_display_support_source)`（仍在 wfd support 大条件内）。

**理由**: screen_capture 100% Source-only，当前笼统放在 wfd support 下未细分角色。

### D7: 编译产物结构不变

**决策**: `libsharing_service.z.so`、`libsharing_codec.z.so`、`libsharing_common.z.so`、`libsharing_network.z.so`、`libsharing_rtp.z.so` 等产物名称和数量不变。

**理由**: 产品形态不变，只是 so 的编译内容 composition 通过 feature flag 条件化。避免引入新的 so 产物导致 SA 加载逻辑变化。

## Risks / Trade-offs

- [RISK: mediaplayer 条件化后 sink=false 编译可能缺少符号] → 缓解：mediaplayer 只被 Sink 代码调用，sink=false 时无调用者，链接不受影响。但需验证 `sharing_codec` 对 mediaplayer 的条件 dep 在 sink=false 时是否导致 codec_factory 中 Sink 相关代码的链接失败 → 通过 `#ifdef WFD_SINK` 保护确保 sink=false 时 Sink codec 相关代码不参与编译。

- [RISK: WFD_SINK/WFD_SOURCE 宏未在所有需要的 BUILD.gn target 中定义] → 缓解：通过统一的 config 引入机制，每个需要条件编译的 target 必须在 configs 中加入 `wifi_display_feature_flags`。

- [RISK: 移走文件后 #include 路径不匹配导致编译失败] → 缓解：移走文件的内部 #include 通过 BUILD.gn include_dirs 解决（`$SHARING_ROOT_DIR/services` 已覆盖公共模块路径）；交叉引用的路径通过 include_dirs 指向新位置。

- [RISK: agent.cpp 的 `#ifdef WFD_SINK` 宏当前未定义，重构前需确保宏定义到位] → 缓解：在 `wifi_display_feature_flags` config 中定义 `WFD_SINK`，agent 的 BUILD.gn 引入此 config。

- [TRADE-OFF: sharing_sink_hisysevent.cpp 仍在 sharing_common so 中无条件编译] → 当 sink=false 时该代码编译了但无调用者，增加少量无用代码体积。Step 2/3 再彻底处理。

- [TRADE-OFF: codec_factory.cpp 出现条件编译片段] → 工厂模式的固有特性，不属于公共代码不当角色依赖。Step 2 可拆分消除。