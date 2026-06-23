## ADDED Requirements

### Requirement: EventEmitter 向 Scheduler 发送事件
`EventEmitter` SHALL 将 `SharingEvent` 实例发送到 `Scheduler` 路由到注册的监听器。

#### Scenario: 发送同步事件
- **WHEN** `EventEmitter::SendSyncEvent(SharingEvent)` 被调用
- **THEN** Scheduler SHALL 路由事件到指定 `ClassType` 注册的 `EventListener`，阻塞直到处理完成

#### Scenario: 发送异步事件
- **WHEN** `EventEmitter::SendEvent(SharingEvent)` 被调用
- **THEN** Scheduler SHALL 通过 `TaskPool` 异步路由事件到注册的 `EventListener`

### Requirement: Scheduler 按 ClassType 和 EventType 路由事件
`Scheduler` SHALL 为单例，接收事件并基于 `ClassType` 和 `EventType` 分发到相应注册的 `EventListener` 实例。

#### Scenario: 注册事件监听器
- **WHEN** `EventListener` 使用特定 `ClassType`（INTERACTION、SCHEDULER、CONTEXT、AGENT、SESSION、CONSUMER、PRODUCER）创建
- **THEN** 监听器 SHALL 注册到 Scheduler 接收匹配其 `ClassType` 的事件

#### Scenario: 路由事件到正确监听器
- **WHEN** `ClassType = AGENT` 和 `EventType = EVENT_AGENT_START` 的 `SharingEvent` 到达 Scheduler
- **THEN** Scheduler SHALL 分发事件到所有注册了 `ClassType = AGENT` 的 `EventListener` 实例

#### Scenario: 事件无注册监听器
- **WHEN** 事件到达 Scheduler 但其 `ClassType` 无注册监听器
- **THEN** 事件 SHALL 被静默丢弃

### Requirement: EventListener 按 EventType 过滤
`EventListener` SHALL 向 Scheduler 注册特定 `ClassType` 并接受匹配其声明 `EventType` 集合的事件。

#### Scenario: 接受匹配事件
- **WHEN** Scheduler 分发匹配 `ClassType` 和 `EventType` 的事件
- **THEN** `EventListener::OnEvent` SHALL 在注册的处理对象上调用 `HandleEventBase::HandleEvent`

#### Scenario: 忽略不匹配事件
- **WHEN** Scheduler 分发匹配 `ClassType` 但不匹配 `EventType` 的事件
- **THEN** `EventListener` SHALL 忽略该事件

### Requirement: SharingEvent 携带类型化 EventMsg
`SharingEvent` SHALL 携带 `EventMsg::Ptr`，含模块特定事件数据（ContextEventMsg、AgentEventMsg、SessionEventMsg、InteractionEventMsg 等）。

#### Scenario: 代理事件消息
- **WHEN** 代理相关事件被发射
- **THEN** `SharingEvent` SHALL 包含 `AgentEventMsg`，含代理 ID、运行步骤和状态信息

#### Scenario: 交互事件消息
- **WHEN** 交互相关事件被发射
- **THEN** `SharingEvent` SHALL 包含 `InteractionEventMsg`，含交互 ID 和错误或通知类型

### Requirement: TaskPool 用于异步事件处理
`TaskPool` SHALL 为 Scheduler 提供线程池用于异步事件处理。

#### Scenario: 异步事件分发到线程
- **WHEN** 通过 `SendEvent` 发送异步事件
- **THEN** Scheduler SHALL 将事件处理任务提交到 `TaskPool` 在工作线程上执行

### Requirement: 事件类型枚举
系统 SHALL 定义按模块组织的 `EventType` 枚举：EVENT_CONTEXT、EVENT_AGENT、EVENT_SESSION、EVENT_MEDIA、EVENT_SCHEDULER、EVENT_INTERACTION、EVENT_CONFIGURE、EVENT_WFD、EVENT_SCREEN_CAPTURE。

#### Scenario: WFD 特定事件类型
- **WHEN** WFD 会话状态变更发生
- **THEN** 事件 SHALL 使用 `EVENT_WFD` EventType 及特定子事件（媒体初始化、RTSP 播放、RTSP 拆除、请求 IDR、是否 PC 源、TCP 成功）

#### Scenario: 媒体通道事件
- **WHEN** 媒体通道生命周期事件发生（创建/销毁/启动/停止/暂停/恢复）
- **THEN** 事件 SHALL 使用 `EVENT_MEDIA` EventType 及消费者或生产者操作的特定子事件