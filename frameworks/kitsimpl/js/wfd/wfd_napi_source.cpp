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

#include "wfd_source_napi.h"
#include "ability_manager/include/ability_manager_client.h"
#include "common/sharing_log.h"
#include "surface_utils.h"
#include "utils.h"
#include "wfd.h"

namespace OHOS {
namespace Sharing {

napi_ref WfdSourceNapi::constructor_ = nullptr;
const std::string BUNDLE_NAME = "com.example.player";
const std::string ABILITY_NAME = "MainAbility";
const std::string CLASS_NAME = "WfdSourceImpl";
const int32_t ARGS_ONE = 1;
const int32_t ARGS_TWO = 2;
const int32_t STRING_MAX_SIZE = 255;

WfdSourceNapi::WfdSourceNapi() {
    SHARING_LOGI("ctor %{public}p.", this);
}

WfdSourceNapi::~WfdSourceNapi() {
    SHARING_LOGI("dtor %{public}p.", this);
    CancelCallbackReference();
    nativeWfdSource_.reset();
}

napi_value WfdSourceNapi::Init(napi_env env, napi_value exports)
{
    SHARING_LOGD("trace.");
    napi_property_descriptor staticProperty[] = {DECLARE_NAPI_STATIC_FUNCTION("createSource", CreateSource)};

    napi_property_descriptor properties[] = {DECLARE_NAPI_FUNCTION("startDiscovery", StartDiscovery),
                                             DECLARE_NAPI_FUNCTION("stopDiscovery", StopDiscovery)};

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);

    SHARING_CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to define WfdSourceClient class.");

    status = napi_create_reference(env, constructor, 1, &constructor_);
    SHARING_CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to create reference of constructor.");

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    SHARING_CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to set constructor.");

    status = napi_define_properties(env, exports, sizeof(staticProperty) / sizeof(staticProperty[0]), staticProperty);
    SHARING_CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to define static function.");

    SHARING_LOGD("success.");
    return exports;
}

void WfdSourceNapi::CancelCallbackReference() 
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(refMutex_);
    if (jsCallback_ != nullptr) {
        jsCallback_->ClearCallbackReference();
        jsCallback_.reset();
    }

    refMap_.clear();
}

napi_value WfdSourceNapi::Constructor(napi_env env, napi_callback_info info)
{
    SHARING_LOGD("trace.");
    napi_status status;
    napi_value result = nullptr;
    napi_value jsThis = nullptr;

    auto finalize = [](napi_env env, void *data, void *hint) {
        SHARING_LOGD("Destructor in.");
        auto *wfdSourceNapi = reinterpret_cast<WfdSourceNapi *>(data);
        wfdSourceNapi->CancelCallbackReference();

        delete wfdSourceNapi;
        SHARING_LOGD("Destructor out.");
    };

    status = napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        napi_get_undefined(env, &result);
        SHARING_LOGE("Failed to retrieve details about the call.");
        return result;
    }

    WfdSourceNapi *jsCast = new (std::nothrow) WfdSourceNapi();
    SHARING_CHECK_AND_RETURN_RET_LOG(jsCast != nullptr, result, "No memory.");

    jsCast->env_ = env;

    std::vector<AAFwk::AbilityRunningInfo> outInfo;
    AAFwk::AbilityManagerClient::GetInstance()->GetAbilityRunningInfos(outInfo);

    if (outInfo.size() != 1) {
        SHARING_LOGE("get ability size error: %{public}zu.", outInfo.size());
        for (auto &item : outInfo) {
            SHARING_LOGW("get app, bundle: %{public}s, ability: %{public}s.", item.ability.GetBundleName().c_str(),
                         item.ability.GetAbilityName().c_str());
        }

        jsCast->bundleName_ = BUNDLE_NAME;
        jsCast->abilityName_ = ABILITY_NAME;
    } else {
        jsCast->bundleName_ = outInfo[0].ability.GetBundleName();
        jsCast->abilityName_ = outInfo[0].ability.GetAbilityName();
        SHARING_LOGI("get app, bundle: %{public}s, ability: %{public}s.", jsCast->bundleName_.c_str(),
                     jsCast->abilityName_.c_str());
    }

    DmKit::InitDeviceManager();
    if (DmKit::GetTrustedDevicesInfo().size() > 0) {
        for (auto &item : DmKit::GetTrustedDevicesInfo()) {
            SHARING_LOGI("remote trusted device: %{public}s.", item.deviceId);
        }
    }

    RpcKeyParser parser;
    jsCast->localKey_ =
        parser.GetRpcKey(jsCast->bundleName_, jsCast->abilityName_, DmKit::GetLocalDevicesInfo().deviceId, CLASS_NAME);

    jsCast->nativeWfdSource_ = WfdSourceFactory::CreateSource(0, jsCast->localKey_);
    SHARING_CHECK_AND_RETURN_RET_LOG(jsCast->nativeWfdSource_ != nullptr, result, "failed to WfdSourceImpl.");

    jsCast->jsCallback_ = std::make_shared<WfdSourceCallbackNapi>(env);
    jsCast->jsCallback_->SetWfdSourceNapi(jsCast);
    jsCast->nativeWfdSource_->SetListener(jsCast->jsCallback_);

    status = napi_wrap(env, jsThis, reinterpret_cast<void *>(jsCast), finalize, nullptr, nullptr);
    if (status != napi_ok) {
        napi_get_undefined(env, &result);
        delete jsCast;
        SHARING_LOGE("Fail to wrapping js to native napi.");
        return result;
    }

    SHARING_LOGD("success.");
    return jsThis;
}

napi_value WfdSourceNapi::CreateSource(napi_env env, napi_callback_info info)
{
    SHARING_LOGD("trace.");
    napi_value result = nullptr;
    napi_value constructor = nullptr;

    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        SHARING_LOGE("Fail to get the representation of constructor object.");
        napi_get_undefined(env, &result);
        return result;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        SHARING_LOGE("Fail to instantiate js cast instance.");
        napi_get_undefined(env, &result);
        return result;
    }

    SHARING_LOGD("success.");
    return result;
}

napi_value WfdSourceNapi::StartDiscovery(napi_env env, napi_callback_info info)
{
    SHARING_LOGD("trace.");
    napi_value result = nullptr;
    napi_value jsThis = nullptr;
    napi_value args[ARGS_ONE] = {nullptr};
    size_t argCount = ARGS_ONE;

    napi_get_undefined(env, &result);
    auto asyncContext = std::make_unique<WfdSourceAsyncContext>(env);
    asyncContext->asyncWorkType = AsyncWorkType::ASYNC_WORK_START_DISCOVERY;

    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SHARING_LOGE("Failed to retrieve details about the call.");
        asyncContext->SignError(SharingErrorCode::ERR_BAD_PARAMETER, "Failed to retrieve details about the call");
    }

    napi_valuetype valueType = napi_undefined;
    if (args[0] == nullptr && napi_typeof(env, args[0], &valueType) != napi_ok && valueType != napi_function) {
        SHARING_LOGW("no callback here.");
    }

    napi_create_reference(env, args[0], 1, &(asyncContext->callbackRef));

    if (asyncContext->callbackRef == nullptr) {
        SHARING_LOGD("napi_create_promise.");
        napi_create_promise(env, &(asyncContext->deferred), &result);
    }

    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncContext->napi));
    SHARING_CHECK_AND_RETURN_RET_LOG(asyncContext->napi != nullptr, result, "failed to GetJsInstance.");

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "StartDiscovery", NAPI_AUTO_LENGTH, &resource);

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, WfdSourceNapi::AsyncWork,
                                          WfdSourceNapi::CompleteCallback, static_cast<void *>(asyncContext.get()),
                                          &asyncContext->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncContext->work));

    asyncContext.release();
    SHARING_LOGD("success.");
    return result;
}

napi_value WfdSourceNapi::StopDiscovery(napi_env env, napi_callback_info info)
{
    SHARING_LOGD("trace.");
    napi_value result = nullptr;
    napi_value jsThis = nullptr;
    napi_value args[ARGS_ONE] = {nullptr};
    size_t argCount = ARGS_ONE;

    napi_get_undefined(env, &result);
    auto asyncContext = std::make_unique<WfdSourceAsyncContext>(env);
    asyncContext->asyncWorkType = AsyncWorkType::ASYNC_WORK_STOP_DISCOVERY;

    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SHARING_LOGE("Failed to retrieve details about the call.");
        asyncContext->SignError(SharingErrorCode::ERR_BAD_PARAMETER, "Failed to retrieve details about the call");
    }

    napi_valuetype valueType = napi_undefined;
    if (args[0] == nullptr && napi_typeof(env, args[0], &valueType) != napi_ok && valueType != napi_function) {
        SHARING_LOGW("no callback here.");
    }

    napi_create_reference(env, args[0], 1, &(asyncContext->callbackRef));

    if (asyncContext->callbackRef == nullptr) {
        SHARING_LOGD("napi_create_promise.");
        napi_create_promise(env, &(asyncContext->deferred), &result);
    }

    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncContext->napi));
    SHARING_CHECK_AND_RETURN_RET_LOG(asyncContext->napi != nullptr, result, "failed to GetJsInstance.");

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "StopDiscovery", NAPI_AUTO_LENGTH, &resource);

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, WfdSourceNapi::AsyncWork, 
                                          WfdSourceNapi::CompleteCallback,static_cast<void *>(asyncContext.get()), 
                                          &asyncContext->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncContext->work));

    asyncContext.release();
    SHARING_LOGD("success.");
    return result;
}

void WfdSourceNapi::AsyncWork(napi_env env, void *data)
{
    SHARING_LOGD("trace.");
    auto asyncContext = reinterpret_cast<WfdSourceAsyncContext *>(data);
    if (asyncContext == nullptr) {
        SHARING_LOGE("WfdSourceAsyncContext is nullptr.");
        return;
    }

    SHARING_LOGD("workType: %{public}d.", asyncContext->asyncWorkType);
    if (asyncContext->napi == nullptr || asyncContext->napi->nativeWfdSource_ == nullptr) {
        if (asyncContext->napi == nullptr) {
            SHARING_LOGE("WfdSourceAsyncContext->napi is nullptr.");
        } else {
            SHARING_LOGE("WfdSourceAsyncContext->nativeWfdSource_ is nullptr.");
        }

        asyncContext->SignError(CommonErrorCode, "native instance is null");
        return;
    }

    auto jsCast = asyncContext->napi;
    auto nativeWfdSource = jsCast->nativeWfdSource_;
    int32_t ret = 0;

    switch (asyncContext->asyncWorkType) {
        case AsyncWorkType::ASYNC_WORK_START_DISCOVERY: {
            ret = nativeWfdSource->StartDiscover();
            if (ret < 0) {
                SHARING_LOGE("nativeWfdSource->StartDiscovery error, %{public}d.", ret);
                asyncContext->SignError(CommonErrorCode, "native instance StartDiscovery error");
                return;
            }
            asyncContext->JsResult = std::make_unique<WfdJsResult>(ret);
            break;
        }
        case AsyncWorkType::ASYNC_WORK_STOP_DISCOVERY: {
            ret = nativeWfdSource->StopDiscover();
            if (ret < 0) {
                SHARING_LOGE("nativeWfdSource->StopDiscovery error, %{public}d.", ret);
                asyncContext->SignError(CommonErrorCode, "native instance StopDiscovery error");
                return;
            }
            asyncContext->JsResult = std::make_unique<WfdJsResult>(ret);
            break;
        }
        default:
            SHARING_LOGE("error unknown operation (%{public}d).", (int32_t)asyncContext->asyncWorkType);
            break;
    }
}

void WfdSourceNapi::CompleteCallback(napi_env env, napi_status status, void *data)
{
    SHARING_LOGD("trace.");
    auto asyncContext = reinterpret_cast<WfdSourceAsyncContext *>(data);
    SHARING_CHECK_AND_RETURN_LOG(asyncContext != nullptr, "asyncContext is nullptr!");

    if (status != napi_ok) {
        asyncContext->SignError(CommonErrorCode, "napi_create_async_work status != napi_ok");
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_value args[2] = {nullptr};
    napi_get_undefined(env, &args[0]);
    napi_get_undefined(env, &args[1]);

    if (asyncContext->errFlag) {
        SHARING_LOGE("async callback failed.");
        CreateError(env, asyncContext->errCode, asyncContext->errMessage, result);
        args[0] = result;
    } else {
        SHARING_LOGD("async callback out.");
        if (asyncContext->JsResult != nullptr) {
            auto res = asyncContext->JsResult->GetJsResult(env, result);
            if (res == napi_ok) {
                args[1] = result;
            } else {
                SHARING_LOGE("asyncContext->JsResult->GetJsResult error, %{public}d.", (int32_t)res);
                asyncContext->SignError(CommonErrorCode, "failed to get return data");
                CreateError(env, asyncContext->errCode, asyncContext->errMessage, result);
                args[0] = result;
            }
        }
    }

    if (asyncContext->deferred) {
        if (asyncContext->errFlag) {
            SHARING_LOGD("napi_reject_deferred.");
            napi_reject_deferred(env, asyncContext->deferred, args[0]);
        } else {
            SHARING_LOGD("napi_resolve_deferred.");
            napi_resolve_deferred(env, asyncContext->deferred, args[1]);
        }
    } else {
        SHARING_LOGD("napi_call_function callback.");
        napi_value callback = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        SHARING_CHECK_AND_RETURN_LOG(callback != nullptr, "callbackRef is nullptr!");
        constexpr size_t argCount = 2;
        napi_value retVal;
        napi_get_undefined(env, &retVal);
        napi_call_function(env, nullptr, callback, argCount, args, &retVal);
        napi_delete_reference(env, asyncContext->callbackRef);
    }
    napi_delete_async_work(env, asyncContext->work);
    if (asyncContext) {
        delete asyncContext;
        asyncContext = nullptr;
    }
    SHARING_LOGD("success.");
}

napi_status WfdSourceNapi::CreateError(napi_env env, int32_t errCode, const std::string &errMsg, napi_value &errVal)
{
    SHARING_LOGD("trace.");
    napi_get_undefined(env, &errVal);

    napi_value msgValStr = nullptr;
    napi_status nstatus = napi_create_string_utf8(env, errMsg.c_str(), NAPI_AUTO_LENGTH, &msgValStr);
    if (nstatus != napi_ok || msgValStr == nullptr) {
        SHARING_LOGE("create error message str fail.");
        return napi_invalid_arg;
    }

    nstatus = napi_create_error(env, nullptr, msgValStr, &errVal);
    if (nstatus != napi_ok || errVal == nullptr) {
        SHARING_LOGE("create error fail.");
        return napi_invalid_arg;
    }

    napi_value codeStr = nullptr;
    nstatus = napi_create_string_utf8(env, "code", NAPI_AUTO_LENGTH, &codeStr);
    if (nstatus != napi_ok || codeStr == nullptr) {
        SHARING_LOGE("create code str fail.");
        return napi_invalid_arg;
    }

    napi_value errCodeVal = nullptr;
    nstatus = napi_create_int32(env, errCode, &errCodeVal);
    if (nstatus != napi_ok || errCodeVal == nullptr) {
        SHARING_LOGE("create error code number val fail.");
        return napi_invalid_arg;
    }

    nstatus = napi_set_property(env, errVal, codeStr, errCodeVal);
    if (nstatus != napi_ok) {
        SHARING_LOGE("set error code property fail.");
        return napi_invalid_arg;
    }

    napi_value msgStr = nullptr;
    nstatus = napi_create_string_utf8(env, "msg", NAPI_AUTO_LENGTH, &msgStr);
    if (nstatus != napi_ok || msgStr == nullptr) {
        SHARING_LOGE("create msg str fail.");
        return napi_invalid_arg;
    }

    nstatus = napi_set_property(env, errVal, msgStr, msgValStr);
    if (nstatus != napi_ok) {
        SHARING_LOGE("set error msg property fail.");
        return napi_invalid_arg;
    }

    napi_value nameStr = nullptr;
    nstatus = napi_create_string_utf8(env, "name", NAPI_AUTO_LENGTH, &nameStr);
    if (nstatus != napi_ok || nameStr == nullptr) {
        SHARING_LOGE("create name str fail.");
        return napi_invalid_arg;
    }

    napi_value errNameVal = nullptr;
    nstatus = napi_create_string_utf8(env, "BusinessError", NAPI_AUTO_LENGTH, &errNameVal);
    if (nstatus != napi_ok || errNameVal == nullptr) {
        SHARING_LOGE("create BusinessError str fail.");
        return napi_invalid_arg;
    }

    nstatus = napi_set_property(env, errVal, nameStr, errNameVal);
    if (nstatus != napi_ok) {
        SHARING_LOGE("set error name property fail.");
        return napi_invalid_arg;
    }
    return napi_ok;
}

} // namespace Sharing
} // namespace OHOS