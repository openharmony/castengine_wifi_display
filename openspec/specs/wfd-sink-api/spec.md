## ADDED Requirements

### Requirement: WfdSinkFactory 创建端实例
系统 SHALL 提供 `WfdSinkFactory::CreateSink(type, key)` 方法，返回 `std::shared_ptr<WfdSink>` 实例。`type` 参数决定端的类实现。

#### Scenario: 使用有效类型创建端
- **WHEN** `WfdSinkFactory::CreateSink(type, key)` 使用有效类型字符串调用
- **THEN** SHALL 返回非空的 `WfdSink` 实例

#### Scenario: 使用无效类型创建端
- **WHEN** `WfdSinkFactory::CreateSink(type, key)` 使用无法识别的类型字符串调用
- **THEN** SHALL 返回空的 `WfdSink` 实例

### Requirement: WfdSink 启动和停止
系统 SHALL 允许 WfdSink 通过 `Start()` 和 `Stop()` 方法启动和停止。启动 SHALL 启用 Wi-Fi P2P 监听模式以接受源设备连接。

#### Scenario: 启动端
- **WHEN** `WfdSink::Start()` 被调用
- **THEN** 系统 SHALL 启用 Wi-Fi P2P 监听模式并接受 WFD 源设备连接

#### Scenario: 停止端
- **WHEN** `WfdSink::Stop()` 被调用
- **THEN** 系统 SHALL 禁用 Wi-Fi P2P 监听模式、断开所有活动会话并释放资源

### Requirement: WfdSink 播放控制
系统 SHALL 允许 WfdSink 通过 `Play()`、`Pause()` 和 `Close()` 方法控制播放。

#### Scenario: 播放投屏流
- **WHEN** `WfdSink::Play()` 在投屏流暂停时调用
- **THEN** 系统 SHALL 在所有已追加的 Surface 上恢复媒体解码和渲染

#### Scenario: 暂停投屏流
- **WHEN** `WfdSink::Pause()` 在投屏流播放时调用
- **THEN** 系统 SHALL 暂停媒体解码并停止渲染，不断开连接

#### Scenario: 关闭投屏流
- **WHEN** `WfdSink::Close()` 在有活动投屏流时调用
- **THEN** 系统 SHALL 拆除 WFD 会话并断开与源的连接

### Requirement: WfdSink 静音和取消静音
系统 SHALL 允许 WfdSink 通过 `Mute()` 和 `UnMute()` 方法静音和取消静音音频。

#### Scenario: 静音音频
- **WHEN** `WfdSink::Mute()` 在音频播放时调用
- **THEN** 系统 SHALL 停止音频渲染，不影响视频渲染

#### Scenario: 取消静音音频
- **WHEN** `WfdSink::UnMute()` 在音频静音时调用
- **THEN** 系统 SHALL 恢复音频渲染

### Requirement: WfdSink Surface 管理
系统 SHALL 允许 WfdSink 通过 `AppendSurface(surfaceId, sceneType)` 和 `RemoveSurface(surfaceId)` 方法追加和移除显示 Surface。一个端 SHALL 支持最多5个 Surface，其中最多2个可同时处于前台模式。

#### Scenario: 追加 Surface
- **WHEN** `WfdSink::AppendSurface(surfaceId, sceneType)` 使用有效 Surface ID 调用
- **THEN** 系统 SHALL 注册 Surface 用于视频渲染，为其创建 `VideoPlayController`

#### Scenario: 追加 Surface 超过上限
- **WHEN** `WfdSink::AppendSurface(surfaceId, sceneType)` 在已达最大 Surface 数（5）时调用
- **THEN** 系统 SHALL 拒绝请求并通知监听器 Surface 失败错误

#### Scenario: 追加前台 Surface 超过限制
- **WHEN** `WfdSink::AppendSurface(surfaceId, FOREGROUND)` 在已有2个前台 Surface 活动时调用
- **THEN** 系统 SHALL 拒绝请求

#### Scenario: 移除 Surface
- **WHEN** `WfdSink::RemoveSurface(surfaceId)` 使用已追加的 Surface ID 调用
- **THEN** 系统 SHALL 停止在该 Surface 上渲染并释放关联的 `VideoPlayController`

### Requirement: WfdSink 场景类型和媒体格式
系统 SHALL 允许 WfdSink 通过 `SetSceneType(sceneType)` 设置场景类型，通过 `SetMediaFormat(videoFormat, audioFormat)` 协商媒体格式。

#### Scenario: 设置场景类型
- **WHEN** `WfdSink::SetSceneType(sceneType)` 使用 FOREGROUND 或 BACKGROUND 调用
- **THEN** 系统 SHALL 将场景类型应用到媒体渲染管道，影响优先级和资源分配

#### Scenario: 设置媒体格式
- **WHEN** `WfdSink::SetMediaFormat(videoFormat, audioFormat)` 被调用
- **THEN** 系统 SHALL 在 WFD 会话建立期间与源协商指定的视频/音频编解码和分辨率

### Requirement: WfdSink 配置和绑定设备
系统 SHALL 允许 WfdSink 通过 `GetSinkConfig()` 查询配置，通过 `GetBoundDevicesList()` 和 `DeleteBoundDevice(deviceId)` 管理绑定设备。

#### Scenario: 获取端配置
- **WHEN** `WfdSink::GetSinkConfig()` 被调用
- **THEN** 系统 SHALL 返回 `SinkConfig`，包含 `surfaceMaximum`（5）、`accessDevMaximum`（4）和 `foregroundMaximum`（2）

#### Scenario: 获取绑定设备列表
- **WHEN** `WfdSink::GetBoundDevicesList()` 被调用
- **THEN** 系统 SHALL 返回所有已建立连接设备的 `BoundDeviceInfo` 列表

#### Scenario: 删除绑定设备
- **WHEN** `WfdSink::DeleteBoundDevice(deviceId)` 被调用
- **THEN** 系统 SHALL 从绑定设备列表移除该设备并断开与该设备的任何活动会话

### Requirement: WfdSink 监听器注册
系统 SHALL 允许 WfdSink 通过 `SetListener(listener)` 注册事件监听器，其中 `listener` 实现 `IWfdEventListener`。

#### Scenario: 注册监听器
- **WHEN** `WfdSink::SetListener(listener)` 使用有效的 `IWfdEventListener` 调用
- **THEN** 后续事件（连接变更、解码器加速、错误、Surface 失败） SHALL 通过 `OnInfo(BaseMsg)` 传递给该监听器

### Requirement: WfdSink NAPI 绑定
系统 SHALL 通过 `WfdSinkNapi` 提供 WfdSink 的 JS/NAPI 包装，暴露 `CreateSink`、`Stop`、`Play`、`On`、`Release`、`SetDiscoverable` 及所有端方法的异步操作。

#### Scenario: JS 端创建
- **WHEN** 从 JavaScript 调用 `@ohos.wfd.createSink()`
- **THEN** SHALL 返回 WfdSinkNapi 对象，代理到原生 WfdSink 实现

#### Scenario: JS 端操作
- **WHEN** 从 JavaScript 调用端方法（play、pause、close、mute、unmute、appendSurface、removeSurface）
- **THEN** 对应的原生方法 SHALL 通过 NAPI 异步工作机制异步调用，结果通过 Promise 返回

### Requirement: WfdSink 多源接收
一个 WfdSink SHALL 能同时从多个源设备接收投屏流，最多支持 `accessDevMaximum`（4）个。

#### Scenario: 从多个源接收
- **WHEN** 多个源设备连接到端
- **THEN** 系统 SHALL 为每个连接创建独立的端代理，各自有独立的媒体通道和播放控制