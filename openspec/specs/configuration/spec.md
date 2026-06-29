## ADDED Requirements

### Requirement: Config 单例管理运行时配置
`Config` SHALL 为单例，管理从 `sharing_config.json` 加载的运行时配置，提供按模块和标签分组的 Get/Set 操作。

#### Scenario: 初始化配置
- **WHEN** Sharing SA 启动
- **THEN** `Config::Init()` SHALL 加载 `sharing_config.json` 并解析为按模块和标签分组的层次化 `SharingData`

#### Scenario: 获取配置值
- **WHEN** `Config::GetConfig(module, tag, key)` 被调用
- **THEN** 系统 SHALL 返回指定 module/tag/key 路径的配置值

#### Scenario: 设置配置值
- **WHEN** `Config::SetConfig(module, tag, key, value)` 被调用
- **THEN** 系统 SHALL 更新运行时配置并发射配置变更事件

#### Scenario: 配置未初始化
- **WHEN** `Config::GetConfig()` 在 `Init()` 完成前调用
- **THEN** 系统 SHALL 返回默认值或指示配置未就绪状态

### Requirement: SharingData 层次化数据模型
`SharingData` SHALL 按层次组织配置值：模块 → 标签 → 键 → 值，`SharingValue` 支持类型化值。

#### Scenario: 模块级配置访问
- **WHEN** 按模块名访问配置
- **THEN** `SharingDataGroupByModule` SHALL 返回该模块的所有标签和值

#### Scenario: 标签级配置访问
- **WHEN** 按模块和标签访问配置
- **THEN** `SharingDataGroupByTag` SHALL 返回模块内该标签的所有键值对

### Requirement: 通过 config.gni 的特性开关
构建系统 SHALL 支持通过 `config.gni` 中 `declare_args()` 的特性开关，可在产品配置层级覆盖。

#### Scenario: WFD 支持开关
- **WHEN** `sharing_framework_support_wfd` 为 true（默认）
- **THEN** WFD 实现源 SHALL 包含在构建中；为 false 时 WFD 源 SHALL 排除

#### Scenario: 端支持开关
- **WHEN** `wifi_display_support_sink` 为 true（默认）
- **THEN** SinkAgent 和端场景源 SHALL 包含在构建中；为 false 时端源 SHALL 排除

#### Scenario: 源支持开关
- **WHEN** `wifi_display_support_source` 为 true（默认）
- **THEN** SrcAgent 和源场景源 SHALL 包含在构建中；为 false 时源源 SHALL 排除

#### Scenario: 覆盖率开关
- **WHEN** `sharing_framework_feature_coverage` 为 true
- **THEN** 构建 SHALL 为编译器和链接器添加 `--coverage` 标志；为 false（默认）时不添加覆盖率标志

### Requirement: sharing_config.json 运行时默认值
`sharing_config.json` SHALL 定义所有模块的默认运行时配置值。

#### Scenario: 编解码配置默认值
- **WHEN** 未覆盖时访问编解码配置
- **THEN** 默认值 SHALL 包含 `forceSWDecoder.isEnable = false`

#### Scenario: 媒体通道配置默认值
- **WHEN** 访问媒体通道配置
- **THEN** 默认值 SHALL 包含 videoFormat（1920x1080@30fps）、audioFormat（2ch@48kHz）、rtcpLimit timeout=3、bufferDispatcher max=800 increment=50

#### Scenario: 上下文限制默认值
- **WHEN** 访问上下文配置
- **THEN** 默认值 SHALL 包含 maxContext=20、maxSinkAgent=20、maxSrcAgent=20

#### Scenario: 网络端口默认值
- **WHEN** 访问网络配置
- **THEN** 默认值 SHALL 包含 UDP 端口范围 6700-7000

#### Scenario: WFD 应用默认值
- **WHEN** 访问 WFD 应用配置
- **THEN** 默认值 SHALL 包含 accessDevMaximum=4、surfaceMaximum=5、foregroundMaximum=2、defaultWfdCtrlport=7236

### Requirement: 配置变更事件
Config SHALL 在配置值变更时发射事件，允许模块动态响应。

#### Scenario: 配置变更通知
- **WHEN** `Config::SetConfig()` 更新值
- **THEN** Config SHALL 发射 `EVENT_CONFIGURE` 事件，以便依赖模块用新设置重新初始化