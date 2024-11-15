#include "native_module_ohos_wfd.h"

namespace OHOS {
namespace Sharing {
/*
 * Function registering all props and functions of ohos.camera module
 */
static napi_value Export(napi_env env, napi_value exports)
{
    SHARING_LOGI("called() WfdSinkNapi::Init().");
    InitEnums(env, exports);
    WfdSinkNapi::Init(env, exports);
    return exports;
}

/*
 * module define
 */
static napi_module g_module = {.nm_version = 1,
                               .nm_flags = 0,
                               .nm_filename = nullptr,
                               .nm_register_func = Export,
                               .nm_modname = "multimedia.SharingWfd",
                               .nm_priv = ((void *)0),
                               .reserved = {0}};

/*
 * module register
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    SHARING_LOGI("called() multimedia.SharingWfd.");
    napi_module_register(&g_module);
}

} // namespace Sharing
} // namespace OHOS