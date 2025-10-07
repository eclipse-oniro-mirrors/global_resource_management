/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

function __resourceIdHar__(res) {
    const arr = res?.params?.[0]?.split('.');
    return globalThis.__resourceTables__?.[res?.moduleName]?.[arr?.[1]]?.[arr?.[2]] ?? -1;
}
export default { __resourceIdHar__ };
globalThis.__resourceIdHar__ = __resourceIdHar__;
