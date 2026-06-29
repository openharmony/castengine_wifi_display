## ADDED Requirements

### Requirement: Interaction 按客户端会话编排
`Interaction` SHALL 作为按客户端的会话编排器，桥接客户端的 IPC 通道到服务的场景逻辑。

#### Scenario: 为客户端创建 Interaction
- **WHEN** 客户端连接到 Sharing SA
- **THEN** `InteractionManager::CreateInteraction(className)` SHALL 创建带有指定场景类（如 WfdSinkScene、WfdSourceScene）的 `Interaction` 实例

#### Scenario: Interaction 路由 IPC 到场景
- **WHEN** 通过 `IpcMsgAdapter::OnRequest` 到达 IPC 请求
- **THEN** Interaction SHALL 将请求转发到 `BaseScene::OnRequest` 进行场景特定处理

#### Scenario: Interaction 路由场景响应到客户端
- **WHEN** 场景产生响应或通知
- **THEN** Interaction SHALL 通过 `IpcMsgAdapter::SendRequest` 将响应转发回客户端

### Requirement: InteractionManager 管理所有 Interaction
`InteractionManager` SHALL 为单例，管理所有活动的 Interaction 实例，将会话密钥映射到交互 ID。

#### Scenario: 创建 Interaction
- **WHEN** 新客户端会话建立
- **THEN** InteractionManager SHALL 创建带有指定场景类的 Interaction 并按密钥注册

#### Scenario: 销毁 Interaction
- **WHEN** 客户端断开或会话结束
- **THEN** InteractionManager SHALL 销毁关联的 Interaction，释放其场景和上下文

#### Scenario: 域消息路由
- **WHEN** 从远程设备到达域消息
- **THEN** InteractionManager SHALL 基于目标 RPC 密钥路由消息到相应的 Interaction

### Requirement: ISharingAdapter 接口
`ISharingAdapter` SHALL 定义场景与交互层之间的完整适配器接口，含30+方法用于上下文/代理生命周期、媒体控制和窗口管理。

#### Scenario: 场景调用适配器创建上下文
- **WHEN** 场景需要为代理创建上下文
- **THEN** 场景 SHALL 通过其适配器引用调用 `ISharingAdapter::CreateContext(className)`

#### Scenario: 场景调用适配器创建代理
- **WHEN** 场景需要在上下文中创建代理
- **THEN** 场景 SHALL 通过其适配器引用调用 `ISharingAdapter::CreateAgent(contextId, className)`

### Requirement: Context 分组代理
`Context` SHALL 作为分组代理的容器，路由事件到代理并转发代理通知向上。

#### Scenario: 创建上下文
- **WHEN** `ISharingAdapter::CreateContext(className)` 被调用
- **THEN** 系统 SHALL 创建 Context 实例并注册到 `ContextManager`

#### Scenario: 在上下文中创建代理
- **WHEN** `Context::HandleCreateAgent(className)` 被调用
- **THEN** 系统 SHALL 创建带有指定会话类的 Agent（SinkAgent 或 SrcAgent）并注册到上下文的代理映射表

#### Scenario: 分发事件到代理
- **WHEN** 事件到达 Context
- **THEN** Context SHALL 通过 `DistributeEvent` 分发事件到所有活动代理

#### Scenario: 代理通知向上传递
- **WHEN** 代理发送状态通知
- **THEN** `Context::OnAgentNotify` SHALL 将通知转发到 Interaction

### Requirement: ContextManager 限制资源使用
`ContextManager` SHALL 执行最大限制：maxContext=20、maxSrcAgent=20、maxSinkAgent=20。

#### Scenario: 上下文限制达到
- **WHEN** `HandleContextCreate` 在已有20个上下文时调用
- **THEN** 系统 SHALL 拒绝创建请求

#### Scenario: 代理限制达到
- **WHEN** 代理创建超过配置的源或端代理最大数
- **THEN** 系统 SHALL 拒绝创建请求

### Requirement: Agent 生命周期状态机
`Agent` SHALL 遵循生命周期状态机：IDLE → CREATE → START → PAUSE → RESUME → PLAY → DESTROY。

#### Scenario: 代理创建步骤
- **WHEN** 代理被创建
- **THEN** 代理 SHALL 转换到 CREATE 步骤，初始化其 BaseSession

#### Scenario: 代理启动步骤
- **WHEN** 收到启动命令
- **THEN** 代理 SHALL 转换到 START 步骤，启动其会话并创建媒体通道

#### Scenario: 代理销毁步骤
- **WHEN** 收到销毁命令或会话错误发生
- **THEN** 代理 SHALL 转换到 DESTROY 步骤，释放所有资源并通知上下文

#### Scenario: 步骤权重优先级
- **WHEN** 同一代理收到多个并发操作请求
- **THEN** 步骤权重系统 SHALL 优先处理操作（如 DESTROY 权重最高，阻塞较低权重操作）

### Requirement: SinkAgent 处理端特定操作
`SinkAgent` SHALL 管理端端会话和媒体通道生命周期。

#### Scenario: SinkAgent 接收消费者通知
- **WHEN** WfdRtpConsumer 报告状态变更
- **THEN** `SinkAgent::OnSessionNotify` SHALL 将通知转发到上下文和交互

### Requirement: SrcAgent 处理源特定操作
`SrcAgent` SHALL 管理源端会话和媒体通道生命周期，持有端代理 ID 的引用。

#### Scenario: SrcAgent 接收生产者通知
- **WHEN** WfdRtpProducer 或 ScreenCaptureConsumer 报告状态变更
- **THEN** `SrcAgent::OnSessionNotify` SHALL 将通知转发到上下文和交互