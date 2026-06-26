## Why

当前 services/ 目录下 Sink-only 和 Source-only 的代码与公共代码混在同一目录和 BUILD.gn target 中，无法通过 `wifi_display_support_sink` 和 `wifi_display_support_source` 编译开关实现独立裁剪。需要将明确归属 Sink 或 Source 的整文件/整目录物理分离到 `services/sink/` 和 `services/source/`，公共代码保持原位不变，为后续更深层的代码拆分（Step 2/3）奠定基础。

## What Changes

- 在 `services/` 下新建 `sink/` 和 `source/` 两个顶层目录
- 将整个文件/目录归属 Sink-only 的代码移到 `services/sink/`，保持与原代码相对路径一致
- 将整个文件/目录归属 Source-only 的代码移到 `services/source/`，保持与原代码相对路径一致
- 所有移走的 Sink-only 编译产物仅在 `wifi_display_support_sink=true` 时编译
- 所有移走的 Source-only 编译产物仅在 `wifi_display_support_source=true` 时编译
- 统一定义 `WFD_SINK` 和 `WFD_SOURCE` 编译宏，用于 `codec_factory.cpp` 等公共工厂类的条件编译保护
- **BREAKING**: `mediaplayer` 从无条件依赖变为 `wifi_display_support_sink` 条件依赖
- **BREAKING**: `screen_capture` 从 `sharing_framework_support_wfd` 条件变为 `wifi_display_support_source` 条件
- `sharing_sink_hisysevent.h/cpp` 和 `sharing_hisysevent.h/cpp` 暂不移动（公共代码交叉引用问题留 Step 2/3 处理）
- 混合头文件（`wfd_def.h`、`wfd_msg.h`、`wfd_session_def.h`、`wfd_media_def.h`、`audio_aac_codec.h`、`audio_g711_codec.h`）暂不拆分（留 Step 2 处理）

## Capabilities

### New Capabilities
- `sink-module-isolation`: Sink-only 代码从公共目录物理分离到 `services/sink/`，通过 `wifi_display_support_sink` 编译开关独立裁剪
- `source-module-isolation`: Source-only 代码从公共目录物理分离到 `services/source/`，通过 `wifi_display_support_source` 编译开关独立裁剪
- `wfd-feature-macros`: 统一 `WFD_SINK` / `WFD_SOURCE` 编译宏定义机制，替代当前散落的 `#ifdef WFD_SINK` 未定义宏问题

### Modified Capabilities

## Impact

- **编译产物结构**: `libsharing_service.z.so` 和 `libsharing_codec.z.so` 的组成内容不变，但 Sink/Source 部分 feature flag 条件化
- **BUILD.gn**: 约 12 个 BUILD.gn 文件需要修改（services/ 主入口、agent/、impl/wfd/、impl/scene/wfd/、codec/、新建的 sink/ 和 source/ 下约 8 个 BUILD.gn）
- **#include 路径**: 移走文件内部的 `#include` 路径需要调整指向新位置；引用移走文件的公共代码路径不变（通过 BUILD.gn include_dirs 解决）
- **依赖链**: `codec/BUILD.gn` 对 `mediaplayer` 的依赖从无条件改为 `wifi_display_support_sink` 条件；`codec_factory.cpp` 需条件 `#include` 和条件 `case` 分支
- **宏定义**: 需新建 `config("wifi_display_feature_flags")` 将 feature flag 转化为 `WFD_SINK`/`WFD_SOURCE` 编译宏
- **`sharing_common` so**: `sharing_sink_hisysevent.cpp` 和 `sharing_hisysevent.cpp` 仍无条件编译在其中（Step 2/3 再处理）
- **`sharing_rtp` so**: `rtp_unpack_impl.cpp` 仍引用 `sharing_sink_hisysevent.h`（设计缺陷，Step 2/3 处理）