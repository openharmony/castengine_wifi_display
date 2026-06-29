## ADDED Requirements

### Requirement: 端场景启用 P2P 监听模式
`WfdSinkScene` SHALL 管理 Wi-Fi P2P 监听模式，在端启动时接受源设备的入站连接。

#### Scenario: 启动端监听模式
- **WHEN** 收到 start IPC 消息
- **THEN** 系统 SHALL 启用 Wi-Fi P2P 监听模式并开始接受源设备的入站 P2P 连接请求

#### Scenario: 停止端监听模式
- **WHEN** 收到 stop IPC 消息
- **THEN** 系统 SHALL 禁用 Wi-Fi P2P 监听模式、断开所有活动的源连接并销毁所有端代理

### Requirement: 端场景为连接创建端代理
`WfdSinkScene` SHALL 为每个入站 P2P 连接创建带有 WfdSinkSession 的端代理。

#### Scenario: 入站 P2P 连接
- **WHEN** 收到来自源设备的 Wi-Fi P2P 连接请求
- **THEN** 系统 SHALL 接受连接，创建带有 WfdSinkSession 的 SinkAgent，并初始化 RTSP 客户端会话协商 WFD 参数

### Requirement: 端场景管理信任列表
`WfdSinkScene` SHALL 通过 `WfdTrustListManager` 维护受信任设备列表，限制可连接端的设备。

#### Scenario: 受信任设备连接
- **WHEN** 来自信任列表中设备的 P2P 连接请求
- **THEN** 系统 SHALL 接受连接并继续 WFD 会话建立

#### Scenario: 不受信任设备连接
- **WHEN** 来自不在信任列表中设备的 P2P 连接请求
- **THEN** 系统 SHALL 拒绝 P2P 连接

#### Scenario: 获取绑定设备
- **WHEN** 收到 getBoundDevices IPC 消息
- **THEN** 系统 SHALL 从活动端代理返回当前连接设备信息列表

#### Scenario: 删除绑定设备
- **WHEN** 收到包含设备 ID 的 deleteBoundDevice IPC 消息
- **THEN** 系统 SHALL 断开与该设备的连接、销毁其端代理并从活动连接中移除

### Requirement: 端场景处理播放控制 IPC
`WfdSinkScene` SHALL 处理 play、pause、close、mute、unmute 的 IPC 请求并路由到相应的端代理。

#### Scenario: 播放命令
- **WHEN** 收到特定连接的 play IPC 消息
- **THEN** 系统 SHALL 在相应端代理中恢复媒体解码和渲染

#### Scenario: 暂停命令
- **WHEN** 收到特定连接的 pause IPC 消息
- **THEN** 系统 SHALL 暂停媒体渲染，不断开连接

#### Scenario: 关闭命令
- **WHEN** 收到特定连接的 close IPC 消息
- **THEN** 系统 SHALL 拆除 WFD 会话并断开与源的连接

#### Scenario: 静音命令
- **WHEN** 收到 mute IPC 消息
- **THEN** 系统 SHALL 在相应端代理中停止音频渲染

### Requirement: 端场景管理 Surface
`WfdSinkScene` SHALL 处理 appendSurface 和 removeSurface IPC 消息，路由到相应的媒体通道。

#### Scenario: 追加 Surface
- **WHEN** 收到包含 Surface ID 和场景类型的 appendSurface IPC 消息
- **THEN** 系统 SHALL 将 Surface 添加到相应端代理的媒体通道，创建 VideoPlayController 进行解码和渲染

#### Scenario: 移除 Surface
- **WHEN** 收到包含 Surface ID 的 removeSurface IPC 消息
- **THEN** 系统 SHALL 从媒体通道移除 Surface，释放关联的 VideoPlayController

#### Scenario: Surface 失败通知
- **WHEN** Surface 操作失败（如 Surface 创建错误、解码器初始化失败）
- **THEN** 系统 SHALL 向客户端发送 surfaceFailure 通知

### Requirement: 端场景处理场景类型和媒体格式
`WfdSinkScene` SHALL 处理 setSceneType 和 setMediaFormat IPC 消息。

#### Scenario: 设置场景类型
- **WHEN** 收到包含 FOREGROUND 或 BACKGROUND 的 setSceneType IPC 消息
- **THEN** 系统 SHALL 更新相应媒体通道的场景类型，影响资源优先级

#### Scenario: 设置媒体格式
- **WHEN** 收到包含视频和音频格式参数的 setMediaFormat IPC 消息
- **THEN** 系统 SHALL 更新相应端代理媒体通道的编解码配置

### Requirement: 端场景报告配置
`WfdSinkScene` SHALL 在查询时返回端配置。

#### Scenario: 获取端配置
- **WHEN** 收到 getSinkConfig IPC 消息
- **THEN** 系统 SHALL 返回包含 surfaceMaximum（5）、accessDevMaximum（4）和 foregroundMaximum（2）的 SinkConfig

### Requirement: 端场景监控 SA 生命周期
`WfdSinkScene` SHALL 监控 Sharing Service 系统能生命周期并处理服务停止事件。

#### Scenario: 服务停止
- **WHEN** Sharing Service 系统能正在停止
- **THEN** 系统 SHALL 断开所有活动会话、销毁所有代理并清理 P2P 资源