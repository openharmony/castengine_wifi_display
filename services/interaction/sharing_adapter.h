/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_ADPATER_H
#define OHOS_SHARING_ADPATER_H

#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {

class ISharingAdapter {
public:
    virtual ~ISharingAdapter() = default;

    /**
     * @brief Notify framework to release the scene.
     *
     * @param sceneId Indicates the scene ID.
     */
    virtual void ReleaseScene(uint32_t sceneId) = 0;
    virtual void OnSceneNotifyDestroyed(uint32_t sceneId) = 0;

    /**
     * @brief Forward events synchronously or asynchronously.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param event Event struct.
     * @param isSync Indicates whether synchronous mode is used.
     * @return Returns 0 if forward the event successfully; returns -1 otherwise.
     */
    virtual int32_t ForwardEvent(uint32_t contextId, uint32_t agentId, SharingEvent &event, bool isSync) = 0;

    /**
     * @brief Forward the domain msg to the interaction manager.
     *
     * @param msg msg to forward.
     */
    virtual void ForwardDomainMsg(std::shared_ptr<BaseDomainMsg> &msg) = 0;

    /**
     * @brief Create a context to contain agents.
     *
     * @param contextId Indicates the context ID. The value <b>0</b> indicates an invalid ID and the creation fails.
     * @return Returns 0 if the context is created; returns -1 otherwise.
     */
    virtual int32_t CreateContext(uint32_t &contextId) = 0;

    /**
     * @brief Create an agent in the context by specifying a business implementation class.
     *
     * @param contextId Indicates the context ID. If contextId is 0, a new context will be created with an agent.
     * @param agentId Indicates the created agent ID. The value <b>0</b> indicates an invalid ID and the creation fails.
     * @param agentType Indicates the agent type {@link AgentType} defined in {@link agent_def.h}.
     * @param sessionName Indicates the business implementation class's name.
     * @return Returns 0 if the agent is created; returns -1 otherwise.
     */
    virtual int32_t CreateAgent(uint32_t &contextId, uint32_t &agentId, AgentType agentType,
                                std::string sessionName) = 0;

    /**
     * @brief Delete all agents in this context, then delete the context.
     *
     * @param contextId Indicates context ID.
     * @return Returns 0 if the context is destroyed; returns -1 otherwise.
     */
    virtual int32_t DestroyContext(uint32_t contextId) = 0;

    /**
     * @brief Delete the agent.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @return Returns 0 if the agent is destroyed; returns -1 otherwise.
     */
    virtual int32_t DestroyAgent(uint32_t contextId, uint32_t agentId) = 0;

    /**
     * @brief Stop operating business agent.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @return Returns 0 if stop sending or receiving streams; returns -1 otherwise.
     */
    virtual int32_t Stop(uint32_t contextId, uint32_t agentId) = 0;

    /**
     * @brief Start operating business agent.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @return Returns 0 if the interaction starts; returns -1 otherwise.
     */
    virtual int32_t Start(uint32_t contextId, uint32_t agentId) = 0;

    /**
     * @brief Pause operating business agent.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param mediaType the media type {@link MediaType} defined in {@link const_def.h}.
     * @return Returns 0 if the stream is paused; returns -1 otherwise.
     */
    virtual int32_t Pause(uint32_t contextId, uint32_t agentId, MediaType mediaType) = 0;

    /**
     * @brief Resume operating business agent.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param mediaType the media type {@link MediaType} defined in {@link const_def.h}.
     * @return Returns 0 if the stream is resumed; returns -1 otherwise.
     */
    virtual int32_t Resume(uint32_t contextId, uint32_t agentId, MediaType mediaType) = 0;

    /**
     * @brief Preview Media Resources.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @return Returns 0 if starts playing; returns -1 otherwise.
     */
    virtual int32_t Play(uint32_t contextId, uint32_t agentId) = 0;

    /**
     * @brief Close the window for previewing media sources.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @return Returns 0 if stops playing; returns -1 otherwise.
     */
    virtual int32_t Close(uint32_t contextId, uint32_t agentId) = 0;

    /**
     * @brief Set the keyframe playback mode.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param surfaceId Indicates the surface ID.
     * @param keyFrame Indicates whether it is keyframe playback mode.
     * @return Returns 0 if keyframe mode is set; returns -1 otherwise.
     */
    virtual int32_t SetKeyPlay(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyFrame) = 0;

    /**
     * @brief Set the keyframe playback mode.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param surfaceId Indicates the surface ID.
     * @param keyRedirect Indicates whether it is keyframe rapid redirect.
     * @return Returns 0 if keyframe mode is set; returns -1 otherwise.
     */
    virtual int32_t SetKeyRedirect(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyRedirect) = 0;

    /**
     * @brief Set the playback volume of audio resources.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param volume Indicates the target volume of the audio to set, ranging from 0 to 1.
     * @return Returns 0 if the volume is set; returns -1 otherwise.
     */
    virtual int32_t SetVolume(uint32_t contextId, uint32_t agentId, float volume) = 0;

    /**
     * @brief Append the surface for the preview window.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param surface Pointer of the surface.
     * @return Returns 0 if the surface is set; returns -1 otherwise.
     */
    virtual int32_t AppendSurface(uint32_t contextId, uint32_t agentId, sptr<Surface> surface,
                                  SceneType sceneType = FOREGROUND) = 0;

    /**
     * @brief del the surface for the preview window.
     *
     * @param contextId Indicates the context ID.
     * @param agentId Indicates the agent ID.
     * @param surfaceId Indicates the surface ID.
     * @return Returns 0 if the surface is del; returns -1 otherwise.
     */
    virtual int32_t RemoveSurface(uint32_t contextId, uint32_t agentId, uint64_t surfaceId) = 0;

    /**
     * @brief Destroy the window.
     *
     * @param windowId Indicates the window ID.
     * @return Returns 0 if the window is destroyed; returns -1 otherwise.
     */
    virtual int32_t DestroyWindow(int32_t windowId) = 0;

    /**
     * @brief Create a preview window inside the framework.
     *
     * @param windowId Indicates the window ID. The value <b>-1</b> indicates an invalid ID and the creation fails.
     * @param windowProperty Indicated initialization for window. For details, see {@link windowProperty} defined in
     * {@link event_comm.h}.
     * @return Returns 0 if the window is created; returns -1 otherwise.
     */
    virtual int32_t CreateWindow(int32_t &windowId, WindowProperty &windowProperty) = 0;

    /**
     * @brief Hide the window.
     *
     * @param windowId Indicates the window ID.
     * @return Returns 0 if the window is hidden; returns -1 otherwise.
     */
    virtual int32_t Hide(int32_t windowId) = 0;

    /**
     * @brief Show the window.
     *
     * @param windowId Indicates the window ID.
     * @return Returns 0 if the window is shown; returns -1 otherwise.
     */
    virtual int32_t Show(int32_t windowId) = 0;

    /**
     * @brief Set the full-screen property of the window.
     *
     * @param windowId Indicates the window ID.
     * @return Returns 0 if the window is to be full screen; returns -1 otherwise.
     */
    virtual int32_t SetFullScreen(int32_t windowId, bool isFull) = 0;

    /**
     * @brief Move the window to the specified coordinates.
     *
     * @param windowId Indicates the window ID.
     * @param x The starting x-axis position of the window
     * @param y The starting y-axis position of the window
     * @return Returns 0 if the window is moved successfully; returns -1 otherwise.
     */
    virtual int32_t MoveTo(int32_t windowId, int32_t x, int32_t y) = 0;

    /**
     * @brief Get surface of the window.
     *
     * @param windowId Indicates the window ID.
     * @param surface Pointer of surface.
     * @return Returns 0 if gets the surface successfully; returns -1 otherwise.
     */
    virtual int32_t GetSurface(int32_t windowId, sptr<Surface> &surface) = 0;

    /**
     * @brief Resize the window.
     *
     * @param windowId Indicates the window ID.
     * @param width Indicates the windows'width you set.
     * @param height Indicates the window's height you set.
     * @return Returns 0 if the window is resized; returns -1 otherwise.
     */
    virtual int32_t ReSize(int32_t windowId, int32_t width, int32_t height) = 0;
};

} // namespace Sharing
} // namespace OHOS
#endif