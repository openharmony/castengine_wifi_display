#ifndef OHOS_SHARING_WFD_CALLBACK_NAPI_H
#define OHOS_SHARING_WFD_CALLBACK_NAPI_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "services/impl/scene/wfd/wfd_def.h"
#include "uv.h"
#include "wfd.h"
#include "wfd_js_result.h"
#include "services/common/sharing_hisysevent.h"

namespace OHOS {
namespace Sharing {

enum class AsyncWorkType {
    ASYNC_WORK_INVALID = 0,
    ASYNC_WORK_START,
    ASYNC_WORK_STOP,
    ASYNC_WORK_START_DISCOVERY,
    ASYNC_WORK_STOP_DISCOVERY
};

class WfdSinkNapi;

struct WfdSinkAsyncContext {
    explicit WfdSinkAsyncContext(napi_env env1) : env(env1) {}

    void SignError(int32_t code, std::string message)
    {
        SHARING_LOGD("trace.");
        errFlag = true;
        errCode = code;
        errMessage = message;
    }

    bool errFlag = false;

    int32_t errCode;
    int32_t videoFormat;

    float volume;
    
    std::string deviceId;
    std::string surfaceId;
    std::string errMessage;

    std::unique_ptr<WfdJsResult> JsResult;

    napi_env env;
    napi_async_work work = nullptr;
    napi_ref callbackRef = nullptr;
    napi_deferred deferred = nullptr;

    SceneType sceneType;
    CodecAttr videoAttr;
    CodecAttr audioAttr;

    WfdSinkNapi *napi = nullptr;
    AsyncWorkType asyncWorkType = AsyncWorkType::ASYNC_WORK_INVALID;
};

struct AutoRef {
    AutoRef(napi_env env, napi_ref cb) : env_(env), cb_(cb) {}
    ~AutoRef()
    {
        SHARING_LOGD("trace.");
        if (env_ != nullptr && cb_ != nullptr) {
            SHARING_LOGD("~Wfd AutoRef dtor.");
            (void)napi_delete_reference(env_, cb_);
        }
    }

    napi_env env_;

    napi_ref cb_;
};

class WfdSinkCallbackNapi : public IWfdSinkListener {
public:
    WfdSinkCallbackNapi(napi_env env) : env_(env) {}

    void SetWfdSinkNapi(WfdSinkNapi *napi)
    {
        SHARING_LOGD("trace.");
        napi_ = napi;
    }

public:
    void OnInfo(std::shared_ptr<BaseMsg> &info) override;

    void OnAccelerationDone(std::string surfaceId);
    void OnDeviceStateChanged(const ConnectionInfo &info);
    void OnError(std::string deviceId, int32_t errorCode, std::string msg);

    void ClearCallbackReference();
    void SaveCallbackReference(const std::string &name, std::weak_ptr<AutoRef> ref);

protected:
    struct WfdJsCallback {
        std::string callbackName = "unknown";

        std::weak_ptr<AutoRef> callback;
        std::unique_ptr<WfdJsResult> jsResult;
    };

    void OnJsCallback(WfdJsCallback *jsCb);

protected:
    std::unordered_map<std::string, std::weak_ptr<AutoRef>> refMap_;

private:
    std::mutex mutex_;
    napi_env env_ = nullptr;
    WfdSinkNapi *napi_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif