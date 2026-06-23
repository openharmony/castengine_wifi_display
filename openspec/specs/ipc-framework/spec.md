## ADDED Requirements

### Requirement: InterIpcService 承载 Sharing 系统能
`InterIpcService` SHALL 作为系统能（SA）承载 Sharing 服务存根，接受客户端连接。

#### Scenario: SA 启动
- **WHEN** Sharing SA 由系统启动
- **THEN** `InterIpcService::OnAddSystemAbility` SHALL 初始化服务并通过 `IInterIpc` 接口接受客户端连接

#### Scenario: SA 关闭
- **WHEN** Sharing SA 由系统停止
- **THEN** `InterIpcService::OnRemoveSystemAbility` SHALL 清理所有活动交互并释放资源

### Requirement: InterIpcClient 连接应用进程到 SA
`InterIpcClient` SHALL 从应用进程通过 OHOS IPC 连接到 Sharing SA，建立双向通信通道。

#### Scenario: 客户端连接到 SA
- **WHEN** 从应用进程调用 `InterIpcClient::Connect(key)`
- **THEN** 系统 SHALL 通过 `SystemAbilityManagerClient` 获取 SA 代理、创建 `InterIpcProxy` 并通过 `IpcMsgAdapter` 设置双向 IPC

#### Scenario: 客户端断开与 SA 的连接
- **WHEN** `InterIpcClient::Disconnect()` 被调用
- **THEN** 系统 SHALL 释放代理连接并清理 IPC 通道

### Requirement: IpcMsgAdapter 桥接 MsgAdapter 到 OHOS IPC
`IpcMsgAdapter` SHALL 将 `MsgAdapter` 接口适配到 OHOS IPC `IRemoteObject`，使 `BaseMsg` 跨进程边界序列化。

#### Scenario: 向 SA 发送请求
- **WHEN** 从客户端调用 `IpcMsgAdapter::SendRequest(BaseMsg)`
- **THEN** 系统 SHALL 通过 `IpcMsgEncoder` 将 `BaseMsg` 编码到 `MessageParcel`、调用 `IInterIpc::DoIpcCommand` 并通过 `IpcMsgDecoder` 解码响应

#### Scenario: 从 SA 接收通知
- **WHEN** SA 通过 `SetListenerObject` 反向通道发送通知
- **THEN** 系统 SHALL 通过 `InterIpcClientStub::OnRemoteRequest` 接收 `MessageParcel`、通过 `IpcMsgDecoder` 解码并传递给 `MsgAdapterListener::OnRequest`

### Requirement: BaseMsg 序列化框架
`BaseMsg` SHALL 使用 `IPC_BIND_ATTR` 宏声明可序列化字段，通过 `IpcMsgEncoder` 和 `IpcMsgDecoder` 实现自动编码/解码。

#### Scenario: 编码 IPC 消息
- **WHEN** `BaseMsg` 子类实例需要跨 IPC 发送
- **THEN** `IpcMsgEncoder` SHALL 将消息 ID 和所有 `IPC_BIND_ATTR` 声明的字段按声明顺序写入 `MessageParcel`

#### Scenario: 解码 IPC 消息
- **WHEN** 通过 IPC 收到 `MessageParcel`
- **THEN** `IpcMsgDecoder` SHALL 读取消息 ID、实例化正确的 `BaseMsg` 子类并从 Parcel 填充所有 `IPC_BIND_ATTR` 声明的字段

#### Scenario: 未知的消息 ID
- **WHEN** `MessageParcel` 包含无法识别的消息 ID
- **THEN** `IpcMsgDecoder` SHALL 返回错误且消息 SHALL 被丢弃

### Requirement: WfdMsgId 枚举
系统 SHALL 定义 WfdMsgId 枚举，含30+消息 ID，覆盖端操作、源操作和通知。

#### Scenario: 端操作消息
- **WHEN** 发送端 IPC 消息（start、stop、play、pause、close、mute、unmute、appendSurface、removeSurface、setSceneType、setMediaFormat、getSinkConfig、getBoundDevices、deleteBoundDevice）
- **THEN** 消息 SHALL 携带对应的 WfdMsgId 并路由到相应的端场景处理器

#### Scenario: 源操作消息
- **WHEN** 发送源 IPC 消息（startDiscovery、stopDiscovery、addDevice、removeDevice、destroyScreenCapture）
- **THEN** 消息 SHALL 携带对应的 WfdMsgId 并路由到相应的源场景处理器

#### Scenario: 通知消息
- **WHEN** 从 SA 向客户端发送通知消息（connectionChanged、decoderAccelerationDone、errorMsg、surfaceFailure、isPcSource）
- **THEN** 消息 SHALL 携带对应的通知 WfdMsgId 并传递到客户端的 `IWfdEventListener::OnInfo`

### Requirement: 双向 IPC 通知
IPC 框架 SHALL 支持双向通信，SA 可向客户端发送主动通知。

#### Scenario: SA 到客户端通知
- **WHEN** SA 中发生需要通知客户端的事件
- **THEN** SA SHALL 创建 `BaseMsg` 通知、编码并通过 `SetListenerObject` 反向 IPC 通道发送到客户端的 `InterIpcClientStub`

### Requirement: IPC 编解码处理复杂类型
`IpcCodec` SHALL 支持编码/解码复杂类型，包括字符串、向量、映射和嵌套的 `BaseMsg` 子类。

#### Scenario: 编码 CodecAttr 向量
- **WHEN** 声明为 `std::vector<CodecAttr>` 的 `BaseMsg` 字段需要序列化
- **THEN** `IpcMsgEncoder` SHALL 写入向量大小和每个元素的字段；`IpcMsgDecoder` SHALL 重构包含所有元素的向量