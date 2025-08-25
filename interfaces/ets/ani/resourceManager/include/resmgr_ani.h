/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESMGR_ANI_H
#define RESMGR_ANI_H

#include "ani.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResMgrAni {
public:
    static ani_string GetStringSyncById(ani_env* env, ani_object object, ani_long resId);

    static ani_string GetFormatStringSyncById(ani_env *env, ani_object object, ani_long resId, ani_object args);

    static ani_string GetStringByNameSync(ani_env* env, ani_object object, ani_string aniName);

    static ani_string GetFormatStringByNameSync(ani_env *env, ani_object object, ani_string aniName, ani_object args);

    static ani_boolean GetBooleanById(ani_env* env, ani_object object, ani_long resId);

    static ani_boolean GetBooleanByName(ani_env* env, ani_object object, ani_string aniName);

    static ani_int GetIntById(ani_env* env, ani_object object, ani_long resId);

    static ani_int GetIntByName(ani_env* env, ani_object object, ani_string aniName);

    static ani_double GetDoubleById(ani_env* env, ani_object object, ani_long resId);

    static ani_double GetDoubleByName(ani_env* env, ani_object object, ani_string aniName);

    static ani_string GetIntPluralStringValueSyncById(ani_env* env, ani_object object,
        ani_long resId, ani_int num, ani_object args);

    static ani_string GetIntPluralStringByNameSync(ani_env* env, ani_object object,
        ani_string aniName, ani_int num, ani_object args);

    static ani_string GetDoublePluralStringValueSyncById(ani_env* env, ani_object object,
        ani_long resId, ani_double num, ani_object args);

    static ani_string GetDoublePluralStringByNameSync(ani_env* env, ani_object object,
        ani_string aniName, ani_double num, ani_object args);

    static ani_object GetDrawableDescriptorById(ani_env* env, ani_object object,
        ani_long resId, ani_object density, ani_object type);

    static ani_object GetDrawableDescriptorByName(ani_env* env, ani_object object,
        ani_string aniName, ani_object density, ani_object type);

    static ani_long GetColorSyncById(ani_env* env, ani_object object, ani_long resId);

    static ani_long GetColorByNameSync(ani_env* env, ani_object object, ani_string aniName);

    static void AddResource(ani_env* env, ani_object object, ani_string path);

    static void RemoveResource(ani_env* env, ani_object object, ani_string path);

    static ani_object GetRawFdSync(ani_env* env, ani_object object, ani_string path);

    static void CloseRawFdSync(ani_env* env, ani_object object, ani_string path);

    static ani_boolean IsRawDir(ani_env* env, ani_object object, ani_string path);

    static ani_object GetRawFileListSync(ani_env* env, ani_object object, ani_string path);

    static ani_object GetRawFileContentSync(ani_env* env, ani_object object, ani_string path);

    static ani_object GetMediaContentSyncById(ani_env* env, ani_object object,
        ani_long resId, ani_object density);

    static ani_string GetMediaContentBase64SyncById(ani_env* env, ani_object object,
        ani_long resId, ani_object density);

    static ani_object GetStringArrayValueSyncById(ani_env* env, ani_object object, ani_long resId);

    static ani_object GetStringArrayByNameSync(ani_env* env, ani_object object, ani_string aniName);

    static ani_object GetMediaByNameSync(ani_env* env, ani_object object, ani_string aniName, ani_object density);

    static ani_string GetMediaBase64ByNameSync(ani_env* env, ani_object object, ani_string aniName, ani_object density);

    static ani_object GetConfigurationSync(ani_env* env, ani_object object);

    static ani_object GetDeviceCapabilitySync(ani_env* env, ani_object object);

    static ani_object GetLocales(ani_env* env, ani_object object, ani_object includeSystem);

    static ani_long GetSymbolById(ani_env* env, ani_object object, ani_long resId);

    static ani_long GetSymbolByName(ani_env* env, ani_object object, ani_string aniName);

    static ani_object GetOverrideResourceManager(ani_env* env, ani_object object, ani_object configuration);

    static ani_object GetOverrideConfiguration(ani_env* env, ani_object object);

    static void UpdateOverrideConfiguration(ani_env* env, ani_object object, ani_object configuration);

    static ani_object GetSysResourceManager(ani_env* env);

    static ani_ref TransferToDynamicResource(ani_env* env, ani_object input);

    static ani_object TransferToStaticResource(ani_env* env, ani_object input);

    static void DestoryResMgr(ani_env* env, ani_object nativePtr);

    static ani_status BindResMgr(ani_env* env);
    
    static ani_status BindResMgrCleaner(ani_env* env);
    
    static ani_status BindStaticFunc(ani_env* env);
    
    static ani_status BindContext(ani_env* env);
};
}
}
}
#endif