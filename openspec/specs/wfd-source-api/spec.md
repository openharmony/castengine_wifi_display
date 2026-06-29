## ADDED Requirements

### Requirement: WfdSourceFactory 创建源实例
系统 SHALL 提供 `WfdSourceFactory::CreateSource(type, key)` 方法，返回 `std::shared_ptr<WfdSource>` 实例。`type` 参数决定源的类实现，`key` 参数标识会话。

#### Scenario: 使用有效类型创建源
- **WHEN** `WfdSourceFactory::CreateSource(type, key)` 使用有效类型字符串调用
- **THEN** SHALL 返回非空的 `WfdSource` 实例

#### Scenario: 使用无效类型创建源
- **WHEN** `WfdSourceFactory::CreateSource(type, key)` 使用无法识别的类型字符串调用
- **THEN** SHALL 返回空的 `WfdSource` 实例

### Requirement: WfdSource 发现生命周期
系统 SHALL 允许 WfdSource 通过 `StartDiscover()` 和 `StopDiscover()` 方法启动和停止设备发现。发现 SHALL 使用 Wi-Fi P2P 查找可用的端设备。

#### Scenario: 启动发现
- **WHEN** `WfdSource::StartDiscover()` 被调用
- **THEN** 系统 SHALL 启动 Wi-Fi P2P 设备发现，并通过 `IWfdEventListener::OnInfo` 以设备发现消息通知监听器发现的设备

#### Scenario: 停止发现
- **WHEN** `WfdSource::StopDiscover()` 被调用
- **THEN** 系统 SHALL 终止 Wi-Fi P2P 设备发现并停止发送设备发现通知

#### Scenario: 发现已进行时重复启动
- **WHEN** `WfdSource::StartDiscover()` 在发现已激活时被调用
- **THEN** 系统 SHALL 无错误地处理重复请求

### Requirement: WfdSource 设备管理
系统 SHALL 允许 WfdSource 通过 `AddDevice(deviceInfo)` 和 `RemoveDevice(deviceId)` 方法添加和移除端设备。一个源 SHALL 同时只能向一个设备投屏。

#### Scenario: 添加设备进行投屏
- **WHEN** `WfdSource::AddDevice(deviceInfo)` 使用已发现的设备调用
- **THEN** 系统 SHALL 发起 P2P 连接和与该设备的 WFD 会话建立，并通知监听器连接状态变更

#### Scenario: 已在投屏时添加设备
- **WHEN** `WfdSource::AddDevice(deviceInfo)` 在已向另一设备投屏时调用
- **THEN** 系统 SHALL 拒绝请求或断开现有设备后再连接新设备

#### Scenario: 移除设备
- **WHEN** `WfdSource::RemoveDevice(deviceId)` 使用已连接设备的 ID 调用
- **THEN** 系统 SHALL 断开与设备的连接、拆除 WFD 会话并停止屏幕采集

### Requirement: WfdSource 监听器注册
系统 SHALL 允许 WfdSource 通过 `SetListener(listener)` 注册事件监听器，其中 `listener` 实现 `IWfdEventListener`。

#### Scenario: 注册监听器
- **WHEN** `WfdSource::SetListener(listener)` 使用有效的 `IWfdEventListener` 调用
- **THEN** 后续事件（设备发现、连接变更、错误） SHALL 通过 `OnInfo(BaseMsg)` 传递给该监听器

#### Scenario: 未注册监听器时的事件
- **WHEN** 在注册监听器之前发生事件
- **THEN** 事件 SHALL 被静默丢弃，不产生错误

### Requirement: WfdSource NAPI 绑定
系统 SHALL 通过 `WfdSourceNapi` 提供 WfdSource 的 JS/NAPI 包装，暴露 `CreateSource`、`StartDiscovery`、`StopDiscovery`、`On` 和 `Release` 方法。

#### Scenario: JS 源创建
- **WHEN** 从 JavaScript 调用 `@ohos.wfd.createSource()`
- **THEN** SHALL 返回 WfdSourceNapi 对象，代理到原生 WfdSource 实现

#### Scenario: JS 发现控制
- **WHEN** 从 JavaScript 调用 `source.startDiscovery()` 或 `source.stopDiscovery()`
- **THEN** 对应的原生 `StartDiscover()` 或 `StopDiscover()` SHALL 通过 NAPI 异步工作机制异步调用

### Requirement: WfdSource 在会话结束时销毁屏幕采集
系统 SHALL 在源会话结束时调用 `DestroyScreenCapture` IPC 命令。

#### Scenario: 会话拆除触发采集清理
- **WHEN** WFD 源会话被拆除（M8 TEARDOWN 完成或错误）
- **THEN** 系统 SHALL 发送 `DestroyScreenCapture` IPC 消息清理屏幕采集资源