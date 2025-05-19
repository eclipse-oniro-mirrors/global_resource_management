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

import { AsyncCallback, BusinessError } from '@ohos.base';
import { RawFileDescriptor as _RawFileDescriptor } from 'global.rawFileDescriptor';
import { Resource as _Resource } from 'global.resource';

export default namespace resourceManager {
    loadLibrary("resourceManager_ani.z");

    export enum Direction {
        DIRECTION_VERTICAL = 0,
        DIRECTION_HORIZONTAL = 1
    }

    export enum DeviceType {
        DEVICE_TYPE_PHONE = 0x00,
        DEVICE_TYPE_TABLET = 0x01,
        DEVICE_TYPE_CAR = 0x02,
        DEVICE_TYPE_PC = 0x03,
        DEVICE_TYPE_TV = 0x04,
        DEVICE_TYPE_WEARABLE = 0x06,
        DEVICE_TYPE_2IN1 = 0x07
    }

    export enum ScreenDensity {
        SCREEN_SDPI = 120,
        SCREEN_MDPI = 160,
        SCREEN_LDPI = 240,
        SCREEN_XLDPI = 320,
        SCREEN_XXLDPI = 480,
        SCREEN_XXXLDPI = 640
    }

    export enum ColorMode {
        DARK = 0,
        LIGHT = 1
    }

    export class Configuration {
        direction: Direction;
        locale: string;
        deviceType: DeviceType;
        screenDensity: ScreenDensity;
        colorMode: ColorMode;
        mcc: number;
        mnc: number;
    }

    export class DeviceCapability {
        screenDensity: ScreenDensity;
        deviceType: DeviceType;
    }

    export interface ResourceManager {
        getStringValue(resource: Resource, callback: AsyncCallback<string, void>): void;
        getStringValue(resource: Resource): Promise<string>;

        getStringSync(resId: number): string;
        getStringSync(resId: number, ...args: (string | number)[]): string;
        getStringSync(resource: Resource): string;
        getStringSync(resource: Resource, ...args: (string | number)[]): string;

        getNumber(resId: number): number;
        getNumber(resource: Resource): number;

        getStringValue(resId: number, callback: AsyncCallback<string, void>): void;
        getStringValue(resId: number): Promise<string>;

        getRawFileContent(path: string, callback: AsyncCallback<Uint8Array, void>): void;
        getRawFileContent(path: string): Promise<Uint8Array>;

        getColorSync(resId: number): number;
        getColorSync(resource: Resource): number;

        getRawFileContentSync(path: string): Uint8Array;

        getIntPluralStringValueSync(resId: number, num: number, ...args: (string | number)[]): string;
        getIntPluralStringValueSync(resource: Resource, num: number, ...args: (string | number)[]): string;
        getIntPluralStringByNameSync(resName: string, num: number, ...args: (string | number)[]): string;

        getDoublePluralStringValueSync(resId: number, num: number, ...args: (string | number)[]): string;
        getDoublePluralStringValueSync(resource: Resource, num: number, ...args: (string | number)[]): string;
        getDoublePluralStringByNameSync(resName: string, num: number, ...args: (string | number)[]): string;
    }

    export type RawFileDescriptor = _RawFileDescriptor;
    export type Resource = _Resource;

    export class ResourceManagerInner implements ResourceManager {
        static { loadLibrary("resourceManager_ani.z"); }

        private nativeResMgr: long = 0;

        constructor(context: long) {
            if (this.nativeResMgr == 0) {
                this.nativeResMgr = context;
            }
        }

        native getStringSync(resId: number): string;
        native getStringSync(resId: number, ...args: (string | number)[]): string;
        native getStringSync(resource: Resource): string;
        native getStringSync(resource: Resource, ...args: (string | number)[]): string;
        native getNumber(resId: number): number;
        native getNumber(resource: Resource): number;
        native getColorSync(resId: number): number;
        native getColorSync(resource: Resource): number;
        native getRawFileContentSync(path: string): Uint8Array;

        native getIntPluralStringValueSync(resId: number, num: number, ...args: (string | number)[]): string;
        native getIntPluralStringValueSync(resource: Resource, num: number, ...args: (string | number)[]): string;
        native getIntPluralStringByNameSync(resName: string, num: number, ...args: (string | number)[]): string;

        native getDoublePluralStringValueSync(resId: number, num: number, ...args: (string | number)[]): string;
        native getDoublePluralStringValueSync(resource: Resource, num: number, ...args: (string | number)[]): string;
        native getDoublePluralStringByNameSync(resName: string, num: number, ...args: (string | number)[]): string;

        getStringValue(resId: number, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getStringSync(resId);
            })
            p1.then((content: NullishType) => {
                callback(new BusinessError(), content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, "");
            });
        }

        getStringValue(resId: number): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getStringSync(resId);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getStringValue(resource: Resource, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getStringSync(resource);
            })
            p1.then((content: NullishType) => {
                callback(new BusinessError(), content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, "");
            });
        }

        getStringValue(resource: Resource): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getStringSync(resource);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getRawFileContent(path: string, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute((): Uint8Array => {
                return this.getRawFileContentSync(path);
            })
            p1.then((content: NullishType) => {
                callback(new BusinessError(), content as Uint8Array);
            }, (err: Error): void => {
                callback(err as BusinessError, new Uint8Array());
            });
        }

        getRawFileContent(path: string): Promise<Uint8Array> {
            return new Promise<Uint8Array>((resolve, reject) => {
                taskpool.execute((): Uint8Array => {
                    return this.getRawFileContentSync(path);
                }).then((content: NullishType) => {
                    resolve(content as Uint8Array);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }
    }

    export native function getSystemResourceManager(): ResourceManager;
}
