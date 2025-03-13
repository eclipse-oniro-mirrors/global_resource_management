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

import { RawFileDescriptor as _RawFileDescriptor } from 'global.rawFileDescriptor';
import { Resource as _Resource } from 'global.resource';

export class BusinessError<T = void> {
    code: number;
    data: T;
}
type AsyncCallback<T, E = void> = (err: BusinessError<E>, data: T) => void;

export default namespace resourceManager {
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
        getString(resId: number, callback: AsyncCallback<string, void>): void;
        getString(resId: number): Promise<string>;

        getStringValue(resource: Resource, callback: AsyncCallback<string, void>): void;
        getStringValue(resource: Resource): Promise<string>;

        getStringSync(resId: number): string;
        getStringSync(resId: number, args: Array<string | number>): string;
        getStringSync(resource: Resource): string;
        getStringSync(resource: Resource, args: Array<string | number>): string;

        getNumber(resId: number): number;
        getNumber(resource: Resource): number;

        getStringValue(resId: number, callback: AsyncCallback<string, void>): void;
        getStringValue(resId: number): Promise<string>;

        getRawFileContent(path: string, callback: AsyncCallback<Uint8Array, void>): void;
        getRawFileContent(path: string): Promise<Uint8Array>;

        getColorSync(resId: number): number;
        getColorSync(resource: Resource): number;

        getRawFileContentSync(path: string): Uint8Array;

        getPluralStringValueSync(resId: number, num: number): string;
        getPluralStringValueSync(resource: Resource, num: number): string;
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
    
        native static create(): ResourceManagerInner;
        native getStringSync(resId: number): string
        native getStringSync(resId: number, args: Array<string | number>): string
        native getStringSync(resource: Resource): string
        native getStringSync(resource: Resource, args: Array<string | number>): string
        native getNumber(resId: number): number 
        native getNumber(resource: Resource): number 
        native getColorSync(resId: number): number 
        native getColorSync(resource: Resource): number 
        native getRawFileContentSync(path: string): Uint8Array;
        native getPluralStringValueSync(resId: number, num: number): string 
        native getPluralStringValueSync(resource: Resource, num: number): string

        getString(resId: number, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute(():undefined => {new ResourceManagerInner(this.nativeResMgr).getStringSync(resId);
                return undefined})
            p1.then((e: NullishType) => {
                let content = e as string;
                let r: BusinessError = { code: 0, data: undefined };
                callback(r, content)
            }, (err: Object): void => {
                console.log("getString callback");
            });
        }

        getString(resId: number): Promise<string> {
            let p = new Promise<string>((resolve: (v: string) => void, reject: (error: Object) => void) => {
                let p1 = taskpool.execute(():undefined => {
                    new ResourceManagerInner(this.nativeResMgr).getStringSync(resId); return undefined})
                p1.then((e: NullishType) => {
                    let content = e as string;
                    resolve(content);
                }, (err: Object): void => {
                    console.log("getString promise");
                });
            });
            return p;
        }

        getStringValue(resId: number, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute(():undefined => {
                new ResourceManagerInner(this.nativeResMgr).getStringSync(resId); return undefined})
            p1.then((e: NullishType) => {
                let content = e as string;
                let r: BusinessError = { code: 0, data: undefined };
                callback(r, content)
            }, (err: Object): void => {
                console.log("getStringValue callback");
            });
        }

        getStringValue(resId: number): Promise<string> {
            let p = new Promise<string>((resolve: (v: string) => void, reject: (error: Object) => void) => {
                let p1 = taskpool.execute(():undefined => {
                    new ResourceManagerInner(this.nativeResMgr).getStringSync(resId); return undefined})
                p1.then((e: NullishType) => {
                    let content = e as string;
                    resolve(content);
                }, (err: Object): void => {
                    console.log("getStringValue promise");
                });
            });
            return p;
        }

        getStringValue(resource: Resource, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute(():undefined => {
                new ResourceManagerInner(this.nativeResMgr).getStringSync(resource); return undefined})
            p1.then((e: NullishType) => {
                let content = e as string;
                let r: BusinessError = { code: 0, data: undefined };
                callback(r, content)
            }, (err: Object): void => {
                console.log("getStringValue resource callback");
            });
        }

        getStringValue(resource: Resource): Promise<string> {
            let p = new Promise<string>((resolve: (v: string) => void, reject: (error: Object) => void) => {
                let p1 = taskpool.execute(():undefined => {
                    new ResourceManagerInner(this.nativeResMgr).getStringSync(resource); return undefined})
                p1.then((e: NullishType) => {
                    let content = e as string;
                    resolve(content);
                }, (err: Object): void => {
                    console.log("getStringValue resource promise");
                });
            });
            return p;
        }

        getRawFileContent(path: string, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute(():undefined => {
                new ResourceManagerInner(this.nativeResMgr).getRawFileContentSync(path); return undefined})
            p1.then((e: NullishType) => {
                let content = e as Uint8Array;
                let r: BusinessError = { code: 0, data: undefined };
                callback(r, content)
            }, (err: Object): void => {
                console.log("getRawFileContent callback");
            });
        }
        
        getRawFileContent(path: string): Promise<Uint8Array> {
            let p = new Promise<Uint8Array>((resolve: (v: Uint8Array) => void, reject: (error: Object) => void) => {
                let p1 = taskpool.execute(():undefined => {
                    new ResourceManagerInner(this.nativeResMgr).getRawFileContentSync(path); return undefined})
                p1.then((e: NullishType) => {
                    let content = e as Uint8Array;
                    resolve(content);
                }, (err: Object): void => {
                    console.log("getRawFileContent promise");
                });
            });
            return p;
        }
    }

    export native function getSystemResourceManager(): ResourceManager;
}
