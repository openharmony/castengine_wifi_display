#ifndef OHOS_SHARING_WFD_NAPI_SOURCE_H
#define OHOS_SHARING_WFD_NAPI_SOURCE_H

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "interaction/device_kit/dm_kit.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "wfd_callback_napi.h"
#include "wfd_enum_napi.h"
#include "wfd_impl.h"
#include "wfd_js_result.h"

namespace OHOS {
namespace Sharing {

class WfdSourceNapi {
public:
    WfdSourceNapi();
    ~WfdSourceNapi();

    static napi_value Init(napi_env env, napi_value exports);

    void CancelCallbackReference();
    static napi_value StartDiscovery(napi_env env, napi_callback_info info);
    static napi_value StopDiscovery(napi_env env, napi_callback_info info);

private:
    static napi_value CreateSource(napi_env env, napi_callback_info info);
    static napi_value Constructor(napi_env env, napi_callback_info info);

    static void AsyncWork(napi_env env, void *data);
    static void CompleteCallback(napi_env env, napi_status status, void *data);

    static napi_status CreateError(napi_env env, int32_t errCode, const std::string &errMsg, napi_value &errVal);

private:
    static napi_ref constructor_;

    std::string localKey_;
    std::string bundleName_;
    std::string abilityName_;

    std::mutex refMutex_;
    std::mutex devicesMutex_;

    std::shared_ptr<WfdSource> nativeWfdSource_ = nullptr;
    std::shared_ptr<WfdSourceCallbackNapi> jsCallback_ = nullptr;

    std::unordered_set<std::string> deviceIds_;
    std::unordered_map<std::string, std::shared_ptr<AutoRef>> refMap_;

    napi_env env_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif