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
import { RawFileDescriptorInner } from 'global.rawFileDescriptorInner';
import { DrawableDescriptor } from '@ohos.arkui.drawableDescriptor';

class ResMgrCleaner {
    private nativeResMgr: long = 0;
    
    constructor(ptr:long) {
        this.nativeResMgr = ptr;
    }

    native destoryResMgr(): void;
}

function cleanCallback(cleanner:ResMgrCleaner): void {
    cleanner.destoryResMgr();
}

let destoryRegister = new FinalizationRegistry<ResMgrCleaner>(cleanCallback);
let unregisterToken = new object();

export default namespace resourceManager {
    loadLibrary("resmgr_ani.z");

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
        mcc: int;
        mnc: int;
    }

    export class DeviceCapability {
        screenDensity: ScreenDensity;
        deviceType: DeviceType;
    }

    export interface ResourceManager {
        getDeviceCapability(callback: AsyncCallback<DeviceCapability, void>): void;
        getDeviceCapability(): Promise<DeviceCapability>;

        getConfiguration(callback: AsyncCallback<Configuration, void>): void;
        getConfiguration(): Promise<Configuration>;

        getStringByName(resName: string, callback: AsyncCallback<string, void>): void;
        getStringByName(resName: string): Promise<string>;

        getStringArrayByName(resName: string, callback: AsyncCallback<Array<string>, void>): void;
        getStringArrayByName(resName: string): Promise<Array<string>>;

        getMediaByName(resName: string, callback: AsyncCallback<Uint8Array, void>): void;
        getMediaByName(resName: string, density: int, callback: AsyncCallback<Uint8Array, void>): void;
        getMediaByName(resName: string): Promise<Uint8Array>;
        getMediaByName(resName: string, density: int): Promise<Uint8Array>;

        getMediaBase64ByName(resName: string, callback: AsyncCallback<string, void>): void;
        getMediaBase64ByName(resName: string, density: int, callback: AsyncCallback<string, void>): void;
        getMediaBase64ByName(resName: string): Promise<string>;
        getMediaBase64ByName(resName: string, density: int): Promise<string>;

        getStringSync(resId: long): string;
        getStringSync(resId: long, ...args: (string | double)[]): string;

        getStringByNameSync(resName: string): string;
        getStringByNameSync(resName: string, ...args: (string | double)[]): string;

        getBoolean(resId: long): boolean;
        getBooleanByName(resName: string): boolean;

        getInt(resId: long): int;
        getIntByName(resName: string): int;

        getDouble(resId: long): double;
        getDoubleByName(resName: string): double;

        getStringValue(resId: long, callback: AsyncCallback<string, void>): void;
        getStringValue(resId: long): Promise<string>;

        getStringArrayValue(resId: long, callback: AsyncCallback<Array<string>, void>): void;
        getStringArrayValue(resId: long): Promise<Array<string>>;

        getIntPluralStringValueSync(resId: long, num: int, ...args: (string | double)[]): string;
        getIntPluralStringByNameSync(resName: string, num: int, ...args: (string | double)[]): string;

        getDoublePluralStringValueSync(resId: long, num: double, ...args: (string | double)[]): string;
        getDoublePluralStringByNameSync(resName: string, num: double, ...args: (string | double)[]): string;

        getMediaContent(resId: long, callback: AsyncCallback<Uint8Array, void>): void;
        getMediaContent(resId: long, density: int, callback: AsyncCallback<Uint8Array, void>): void;
        getMediaContent(resId: long): Promise<Uint8Array>;
        getMediaContent(resId: long, density: int): Promise<Uint8Array>;

        getMediaContentBase64(resId: long, callback: AsyncCallback<string, void>): void;
        getMediaContentBase64(resId: long, density: int, callback: AsyncCallback<string, void>): void;
        getMediaContentBase64(resId: long): Promise<string>;
        getMediaContentBase64(resId: long, density: int): Promise<string>;

        getRawFileContent(path: string, callback: AsyncCallback<Uint8Array, void>): void;
        getRawFileContent(path: string): Promise<Uint8Array>;

        getRawFd(path: string, callback: AsyncCallback<RawFileDescriptor, void>): void;
        getRawFd(path: string): Promise<RawFileDescriptor>;

        closeRawFd(path: string, callback: AsyncCallback<void, void>): void;
        closeRawFd(path: string): Promise<void>;

        getRawFileList(path: string, callback: AsyncCallback<Array<string>, void>): void;
        getRawFileList(path: string): Promise<Array<string>>;

        getColor(resId: long, callback: AsyncCallback<long, void>): void;
        getColor(resId: long): Promise<long>;

        getColorByName(resName: string, callback: AsyncCallback<long, void>): void;
        getColorByName(resName: string): Promise<long>;

        getColorSync(resId: long): long;
        getColorByNameSync(resName: string) : long;

        addResource(path: string) : void;
        removeResource(path: string) : void;

        getRawFdSync(path: string): RawFileDescriptor;
        closeRawFdSync(path: string): void;

        getRawFileListSync(path: string): Array<string>;
        getRawFileContentSync(path: string): Uint8Array;

        getMediaContentSync(resId: long, density?: int): Uint8Array;

        getMediaContentBase64Sync(resId: long, density?: int): string;

        getDrawableDescriptor(resId: long, density?: int, type?: int): DrawableDescriptor;
        getDrawableDescriptorByName(resName: string, density?: int, type?: int): DrawableDescriptor;

        getStringArrayValueSync(resId: long): Array<string>;

        getMediaByNameSync(resName: string, density?: int): Uint8Array;
        getMediaBase64ByNameSync(resName: string, density?: int): string;

        getStringArrayByNameSync(resName: string): Array<string>;

        getConfigurationSync(): Configuration;
        getDeviceCapabilitySync(): DeviceCapability;
        getLocales(includeSystem?: boolean): Array<string>;

        getSymbol(resId: long) : long;
        getSymbolByName(resName: string) : long;

        isRawDir(path: string): boolean;

        getOverrideResourceManager(configuration?: Configuration): ResourceManager;
        getOverrideConfiguration(): Configuration;
        updateOverrideConfiguration(configuration: Configuration): void;
    }

    export type RawFileDescriptor = _RawFileDescriptor;
    export type Resource = _Resource;

    export class ResourceManagerInner implements ResourceManager {
        static { loadLibrary("resmgr_ani.z"); }

        private nativeResMgr: long = 0;
        private cleaner: ResMgrCleaner | null = null;

        constructor(context: long) {
            if (this.nativeResMgr == 0) {
                this.nativeResMgr = context;
            }
            this.cleaner = new ResMgrCleaner(this.nativeResMgr);
            destoryRegister.register(this, this.cleaner!, unregisterToken);
        }

        native getStringSync(resId: long): string;
        native getStringSync(resId: long, ...args: (string | double)[]): string;

        native getStringByNameSync(resName: string): string;
        native getStringByNameSync(resName: string, ...args: (string | double)[]): string;

        native getBoolean(resId: long): boolean;
        native getBooleanByName(resName: string): boolean;

        native getInt(resId: long): int;
        native getIntByName(resName: string): int;

        native getDouble(resId: long): double;
        native getDoubleByName(resName: string): double;

        native getIntPluralStringValueSync(resId: long, num: int, ...args: (string | double)[]): string;
        native getIntPluralStringByNameSync(resName: string, num: int, ...args: (string | double)[]): string;

        native getDoublePluralStringValueSync(resId: long, num: double, ...args: (string | double)[]): string;
        native getDoublePluralStringByNameSync(resName: string, num: double, ...args: (string | double)[]): string;

        native getColorSync(resId: long): long;
        native getColorByNameSync(resName: string) : long;

        native addResource(path: string) : void;
        native removeResource(path: string) : void;

        native getRawFdSync(path: string): RawFileDescriptor;
        native closeRawFdSync(path: string): void;
        native isRawDir(path: string): boolean;

        native getRawFileListSync(path: string): Array<string>;
        native getRawFileContentSync(path: string): Uint8Array;

        native getMediaContentSync(resId: long, density?: int): Uint8Array;

        native getMediaContentBase64Sync(resId: long, density?: int): string;

        native getDrawableDescriptor(resId: long, density?: int, type?: int): DrawableDescriptor;
        native getDrawableDescriptorByName(resName: string, density?: int, type?: int): DrawableDescriptor;

        native getStringArrayValueSync(resId: long): Array<string>;
        native getStringArrayByNameSync(resName: string): Array<string>;

        native getMediaByNameSync(resName: string, density?: int): Uint8Array;
        native getMediaBase64ByNameSync(resName: string, density?: int): string;

        native getConfigurationSync(): Configuration;
        native getDeviceCapabilitySync(): DeviceCapability;
        native getLocales(includeSystem?: boolean): Array<string>;

        native getSymbol(resId: long) : long;
        native getSymbolByName(resName: string) : long;

        native getOverrideResourceManager(configuration?: Configuration): ResourceManager;
        native getOverrideConfiguration(): Configuration;
        native updateOverrideConfiguration(configuration: Configuration): void;

        getStringValue(resId: long, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getStringSync(resId);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getStringValue(resId: long): Promise<string> {
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

        getStringArrayValue(resId: long, callback: AsyncCallback<Array<string>, void>): void {
            let p1 = taskpool.execute((): Array<string> => {
                return this.getStringArrayValueSync(resId);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Array<string>);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getStringArrayValue(resId: long): Promise<Array<string>> {
            return new Promise<Array<string>>((resolve, reject) => {
                taskpool.execute((): Array<string> => {
                    return this.getStringArrayValueSync(resId);
                }).then((content: NullishType) => {
                    resolve(content as Array<string>);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getDeviceCapability(callback: AsyncCallback<DeviceCapability, void>): void {
            let p1 = taskpool.execute((): DeviceCapability => {
                return this.getDeviceCapabilitySync();
            })
            p1.then((content: NullishType) => {
                callback(null, content as DeviceCapability);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getDeviceCapability(): Promise<DeviceCapability> {
            return new Promise<DeviceCapability>((resolve, reject) => {
                taskpool.execute((): DeviceCapability => {
                    return this.getDeviceCapabilitySync();
                }).then((content: NullishType) => {
                    resolve(content as DeviceCapability);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getConfiguration(callback: AsyncCallback<Configuration, void>): void {
            let p1 = taskpool.execute((): Configuration => {
                return this.getConfigurationSync();
            })
            p1.then((content: NullishType) => {
                callback(null, content as Configuration);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getConfiguration(): Promise<Configuration> {
            return new Promise<Configuration>((resolve, reject) => {
                taskpool.execute((): Configuration => {
                    return this.getConfigurationSync();
                }).then((content: NullishType) => {
                    resolve(content as Configuration);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getStringByName(resName: string, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getStringByNameSync(resName);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getStringByName(resName: string): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getStringByNameSync(resName);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getStringArrayByName(resName: string, callback: AsyncCallback<Array<string>, void>): void {
            let p1 = taskpool.execute((): Array<string> => {
                return this.getStringArrayByNameSync(resName);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Array<string>);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getStringArrayByName(resName: string): Promise<Array<string>> {
            return new Promise<Array<string>>((resolve, reject) => {
                taskpool.execute((): Array<string> => {
                    return this.getStringArrayByNameSync(resName);
                }).then((content: NullishType) => {
                    resolve(content as Array<string>);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaByName(resName: string, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute((): Uint8Array => {
                return this.getMediaByNameSync(resName);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Uint8Array);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaByName(resName: string): Promise<Uint8Array> {
            return new Promise<Uint8Array>((resolve, reject) => {
                taskpool.execute((): Uint8Array => {
                    return this.getMediaByNameSync(resName);
                }).then((content: NullishType) => {
                    resolve(content as Uint8Array);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaByName(resName: string, density: int, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute((): Uint8Array => {
                return this.getMediaByNameSync(resName, density);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Uint8Array);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaByName(resName: string, density: int): Promise<Uint8Array> {
            return new Promise<Uint8Array>((resolve, reject) => {
                taskpool.execute((): Uint8Array => {
                    return this.getMediaByNameSync(resName, density);
                }).then((content: NullishType) => {
                    resolve(content as Uint8Array);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaBase64ByName(resName: string, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getMediaBase64ByNameSync(resName);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaBase64ByName(resName: string): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getMediaBase64ByNameSync(resName);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaBase64ByName(resName: string, density: int, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getMediaBase64ByNameSync(resName, density);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaBase64ByName(resName: string, density: int): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getMediaBase64ByNameSync(resName, density);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaContent(resId: long, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute((): Uint8Array => {
                return this.getMediaContentSync(resId);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Uint8Array);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaContent(resId: long): Promise<Uint8Array> {
            return new Promise<Uint8Array>((resolve, reject) => {
                taskpool.execute((): Uint8Array => {
                    return this.getMediaContentSync(resId);
                }).then((content: NullishType) => {
                    resolve(content as Uint8Array);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaContent(resId: long, density: int, callback: AsyncCallback<Uint8Array, void>): void {
            let p1 = taskpool.execute((): Uint8Array => {
                return this.getMediaContentSync(resId, density);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Uint8Array);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaContent(resId: long, density: int): Promise<Uint8Array> {
            return new Promise<Uint8Array>((resolve, reject) => {
                taskpool.execute((): Uint8Array => {
                    return this.getMediaContentSync(resId, density);
                }).then((content: NullishType) => {
                    resolve(content as Uint8Array);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaContentBase64(resId: long, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getMediaContentBase64Sync(resId);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaContentBase64(resId: long): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getMediaContentBase64Sync(resId);
                }).then((content: NullishType) => {
                    resolve(content as string);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getMediaContentBase64(resId: long, density: int, callback: AsyncCallback<string, void>): void {
            let p1 = taskpool.execute((): string => {
                return this.getMediaContentBase64Sync(resId, density);
            })
            p1.then((content: NullishType) => {
                callback(null, content as string);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getMediaContentBase64(resId: long, density: int): Promise<string> {
            return new Promise<string>((resolve, reject) => {
                taskpool.execute((): string => {
                    return this.getMediaContentBase64Sync(resId, density);
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
                callback(null, content as Uint8Array);
            }, (err: Error): void => {
                callback(err as BusinessError, undefined);
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

        getRawFd(path: string, callback: AsyncCallback<RawFileDescriptor, void>): void {
            let p1 = taskpool.execute((): RawFileDescriptor => {
                return this.getRawFdSync(path);
            })
            p1.then((content: NullishType) => {
                callback(null, content as RawFileDescriptor);
            }, (err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getRawFd(path: string): Promise<RawFileDescriptor> {
            return new Promise<RawFileDescriptor>((resolve, reject) => {
                taskpool.execute((): RawFileDescriptor => {
                    return this.getRawFdSync(path);
                }).then((content: NullishType) => {
                    resolve(content as RawFileDescriptor);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        closeRawFd(path: string, callback: AsyncCallback<void, void>): void {
            let p1 = taskpool.execute((): void => {
                return this.closeRawFdSync(path);
            })
            p1.then(() => {
                callback(null, undefined);
            }, (err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        closeRawFd(path: string): Promise<void> {
            return new Promise<void>((resolve, reject) => {
                taskpool.execute((): void => {
                    return this.closeRawFdSync(path);
                }).then(() => {
                    resolve(undefined);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getRawFileList(path: string, callback: AsyncCallback<Array<string>, void>): void {
            let p1 = taskpool.execute((): Array<string> => {
                return this.getRawFileListSync(path);
            })
            p1.then((content: NullishType) => {
                callback(null, content as Array<string>);
            }, (err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getRawFileList(path: string): Promise<Array<string>> {
            return new Promise<Array<string>>((resolve, reject) => {
                taskpool.execute((): Array<string> => {
                    return this.getRawFileListSync(path);
                }).then((content: NullishType) => {
                    resolve(content as Array<string>);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getColor(resId: long, callback: AsyncCallback<long, void>): void {
            let p1 = taskpool.execute((): long => {
                return this.getColorSync(resId);
            })
            p1.then((content: NullishType) => {
                callback(null, content as long);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getColor(resId: long): Promise<long> {
            return new Promise<long>((resolve, reject) => {
                taskpool.execute((): long => {
                    return this.getColorSync(resId);
                }).then((content: NullishType) => {
                    resolve(content as long);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }

        getColorByName(resName: string, callback: AsyncCallback<long, void>): void {
            let p1 = taskpool.execute((): long => {
                return this.getColorByNameSync(resName);
            })
            p1.then((content: NullishType) => {
                callback(null, content as long);
            }).catch((err: Error): void => {
                callback(err as BusinessError, undefined);
            });
        }

        getColorByName(resName: string): Promise<long> {
            return new Promise<long>((resolve, reject) => {
                taskpool.execute((): long => {
                    return this.getColorByNameSync(resName);
                }).then((content: NullishType) => {
                    resolve(content as long);
                }, (err: Error): void => {
                    reject(err);
                });
            });
        }
    }

    export native function getSysResourceManager(): ResourceManager;
    export native function transferToDynamicResource(object: Object): Any;
    export native function transferToStaticResource(object: Any): Object;
}
