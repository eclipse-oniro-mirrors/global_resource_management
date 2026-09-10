# AGENTS.md

## 1. 代码概览

本 AGENTS.md 适用于 `resource_management` 仓库根目录（`/base/global/resource_management`）。本仓库无更细粒度的子目录 AGENTS.md，本文件即为本模块唯一的架构与开发指引。

本仓库提供 OpenHarmony **资源管理（resource_management）** 能力，核心职责是解析 restool 编译产物 `resources.index` 索引文件，根据当前 configuration（语言/区域/横竖屏/mcc/mnc）与 device capability（设备类型/分辨率/颜色模式）选出最匹配的应用资源，对应用侧提供 ArkTS/JS NAPI、ANI、Cangjie FFI、Native C ABI、子系统间 C++ inner_api 等多语言接口。最重要的架构边界是 **核心引擎（`frameworks/resmgr/`）与对外接口层（`interfaces/`）之间的解耦**——核心引擎不直接暴露给应用，应用侧仅通过 `interfaces/` 的多语言绑定访问；所有资源解析逻辑集中在核心引擎，接口层只做转发。

### 目录职责

| 目录 | 职责 | 高风险标记 |
|------|------|-----------|
| `frameworks/resmgr/` | 核心引擎：ResourceManager 实现、HAP 资源模型、索引解析（V1/V2）、配置匹配、overlay/主题/系统资源 | 🔴 修改影响全局资源匹配与下发 |
| `frameworks/resmgr/include/` | 核心引擎内部头文件（HapManager、HapParser、ResConfigImpl 等） | 🔴 内部接口，不可对外暴露 |
| `frameworks/resmgr/include/utils/` | 工具类：common.h、date_utils.h、errors.h、psue_manager.h、string_utils.h、utils.h | 🟢 |
| `frameworks/resmgr/src/` | 核心引擎实现 .cpp（约 26 个源文件） | 🔴 |
| `interfaces/inner_api/include/` | 子系统间 C++ 抽象接口：res_common.h、res_config.h、resource_manager.h、rstate.h | 🔴 inner_api 兼容性边界 |
| `interfaces/js/kits/` | NAPI 绑定入口：resource_manager_napi.cpp + sendable_resource_manager_napi.cpp | 🔴 NAPI 签名是公共 API |
| `interfaces/js/innerkits/core/` | NAPI 共享 addon（resmgr_napi_core）：addon/sync_impl/async_impl/context/utils/resource_table_loader | 🟡 共享实现，NAPI 变更影响多入口 |
| `interfaces/ets/ani/` | ArkTS ANI 绑定 + 预生成 .abc 字节码 | 🔴 ANI 签名是公共 API |
| `interfaces/cj/` | Cangjie FFI 绑定 | 🟡 |
| `interfaces/native/resource/include/` | Native C ABI：ohresmgr.h、raw_file.h、raw_dir.h、raw_file_manager.h、resmgr_common.h | 🔴 Native 公共 API 兼容性边界 |
| `dfx/hisysevent_adapter/` | HiSysEvent 适配：hisysevent_adapter.cpp/.h | 🔴 故障归因事件定义 |
| `dfx/api_metrics/` | 可选 API 调用直方图上报：api_metrics_reporter.cpp/.h | 🟡 特性标志控制 |
| `test/fuzztest/` | libFuzzer 模糊测试 + corpus | 🟢 |
| `test/resource/data/` | 测试 HAP：all.hap / all_fa.hap / newModule.hap / err-config.*.hap / overlay/ / theme/ | 🟢 |
| `hisysevent.yaml` | 故障/行为事件定义（GLOBAL_RESMGR domain） | 🔴 事件定义是硬性约束 |
| `resmgr.gni` | 特性开关：resource_management_support_icu / resource_management_api_metrics | 🟡 默认值变更影响下游产品 |
| `bundle.json` | 组件元信息、构建目标、依赖声明 | 🔴 依赖与目标声明 |

### 速查入口

| 任务类型 | 先看 |
|---|---|
| 资源匹配逻辑（最优匹配/限定词/设备类型过滤） | `frameworks/resmgr/src/res_config_impl.cpp` + `hap_manager.cpp` |
| resources.index 解析（V1 全量/V2 mmap 懒解析） | `frameworks/resmgr/src/hap_parser.cpp` / `hap_parser_v1.cpp` / `hap_parser_v2.cpp` |
| HAP 资源模型（HapResource/HapManager/版本判定） | `frameworks/resmgr/src/hap_resource_manager.cpp` + `hap_resource_v1.cpp` / `hap_resource_v2.cpp` |
| 区域匹配（RFC-4647/likely-subtags/locale 排序） | `frameworks/resmgr/src/locale_matcher.cpp` + `res_locale.cpp` |
| overlay 覆盖资源 | `frameworks/resmgr/src/hap_resource_v2.cpp`（OverlayResource）+ `hap_manager.cpp`（AddAppOverlay/RemoveAppOverlay/UpdateOverlayInfo） |
| 主题包加载 | `frameworks/resmgr/src/theme_pack_manager.cpp` + `theme_pack_resource.cpp` + `theme_pack_config.cpp` |
| 系统资源管理 | `frameworks/resmgr/src/system_resource_manager.cpp` |
| 厂商扩展（dlopen .so 插件） | `frameworks/resmgr/src/resource_manager_ext_mgr.cpp` |
| rawfile 访问（Native OH_Raw*） | `frameworks/resmgr/src/raw_file_manager.cpp` + `interfaces/native/resource/include/raw_file.h` |
| Native C ABI（ResourceManager C-API） | `frameworks/resmgr/src/native_resource_manager.cpp` + `interfaces/native/resource/include/ohresmgr.h` |
| NAPI 绑定修改 | `interfaces/js/kits/src/resource_manager_napi.cpp` → `interfaces/js/innerkits/core/src/` |
| ANI 绑定修改 | `interfaces/ets/ani/resourceManager/` |
| Cangjie FFI 绑定修改 | `interfaces/cj/src/` |
| inner_api 接口修改 | `interfaces/inner_api/include/resource_manager.h`（抽象接口）+ `frameworks/resmgr/src/resource_manager.cpp`（工厂函数）+ `resource_manager_impl.cpp`（实现） |
| 错误码修改 | `interfaces/inner_api/include/rstate.h`（RState 枚举） |
| 日志/DFX 修改 | `frameworks/resmgr/include/hilog_wrapper.h` + `hisysevent.yaml` + `dfx/hisysevent_adapter/` |
| 构建配置/特性开关 | `resmgr.gni` + `bundle.json` + `frameworks/resmgr/BUILD.gn` |
| 安全上限常量（解析边界校验） | `frameworks/resmgr/include/res_desc.h`（MAX_RES_KEY_COUNT 等） |
| 二进制格式结构体（与 restool 镜像） | `frameworks/resmgr/include/res_desc.h` |

### 关联仓库

本模块无 IPC 服务、无 SA 注册，所有调用均为同进程函数调用。按调用关系界定上下游：

| 仓库 | 路径 | 关系 | 依赖方式 | 关键交互点 |
|------|------|------|---------|-----------|
| restool | `/developtools/global_resource_tool` | 构建期上游 | 文件契约：产出 `resources.index` | restool 写入端结构体在 `include/resource_table.h`，本模块镜像结构体在 `frameworks/resmgr/include/res_desc.h`，字节布局严格一致；V1（`"Restool "` 开头）→ V1 解析器，V2 → mmap 懒解析器 |
| ability_runtime | `/foundation/ability/ability_runtime` | 运行期上游 | `external_deps += ["resource_management:global_resmgr", "resource_management:resmgr_napi_core", ...]`，进程内共享库调用 | 创建 RM 实例（`MainThread::HandleLaunchApplication`）、多模块 RM（`ContextImpl::CreateModuleContext`）、配置变更（`ScheduleConfigurationUpdated` → `UpdateResConfig`）、overlay/HQF 补丁/共享 HAR 资源加载 |
| ace_engine | `/foundation/arkui/ace_engine` | 运行期上游 | `external_deps += ["resource_management:global_resmgr"]`，进程内共享库调用 | `$r('app.string.xxx')` 解析后经 `ResourceAdapterImplV2` 转发到 inner_api；关键调用：`GetStringById`/`GetColorById`/`GetMediaById`/`GetFloatById`/`GetRawFileDescriptorFromHap`/`GetResourceLimitKeys`/`GetOverrideResourceManager` |

## 2. 知识路由

遇到问题先定位场景，再读对应文档。以下文档包含完整领域概念和操作指引，不是可选背景阅读。

### 按任务路由

| 任务类别 | 先读 |
|---|---|
| 公共 API 或 SDK 行为变更 | `interfaces/inner_api/include/rstate.h`（错误码兼容性边界）+ `interfaces/inner_api/include/resource_manager.h` |
| resources.index 二进制格式变更 | `frameworks/resmgr/include/res_desc.h`（结构体定义与安全上限）+ `frameworks/resmgr/src/hap_parser_v1.cpp` / `hap_parser_v2.cpp` |
| 资源匹配算法变更 | `frameworks/resmgr/src/res_config_impl.cpp`（IsMoreSuitable）+ `hap_manager.cpp`（MatchBestResource） |
| locale 匹配/排序变更 | `frameworks/resmgr/src/locale_matcher.cpp` + `frameworks/resmgr/src/res_locale.cpp` + `likely_subtags_key_data.cpp` / `likely_subtags_value_data.cpp` |
| overlay/主题包/HQF 补丁变更 | `frameworks/resmgr/src/hap_resource_v2.cpp`（OverlayResource）+ `theme_pack_manager.cpp` + `hap_manager.cpp` |
| 系统资源加载变更 | `frameworks/resmgr/src/system_resource_manager.cpp` |
| NAPI 绑定修改 | `interfaces/js/kits/src/resource_manager_napi.cpp` + `interfaces/js/innerkits/core/src/resource_manager_addon.cpp` |
| ANI 绑定修改 | `interfaces/ets/ani/resourceManager/` 目录结构 |
| Native C ABI 修改 | `interfaces/native/resource/include/` 头文件 + `frameworks/resmgr/src/native_resource_manager.cpp` + `raw_file_manager.cpp` |
| DFX/日志/故障归因变更 | `frameworks/resmgr/include/hilog_wrapper.h` + `hisysevent.yaml` + `dfx/hisysevent_adapter/hisysevent_adapter.h` |
| 构建特性开关变更 | `resmgr.gni` + `frameworks/resmgr/BUILD.gn` |
| 跨平台 IDE 预览变体 | `frameworks/resmgr/BUILD.gn` 中 `global_resmgr_win` / `global_resmgr_mac` / `global_resmgr_linux` target |

### 按路径路由

| 修改路径 | 先读 |
|---|---|
| `frameworks/resmgr/src/` | `frameworks/resmgr/include/` 对应头文件 + `interfaces/inner_api/include/resource_manager.h`（接口契约） |
| `frameworks/resmgr/include/` | `res_desc.h`（结构体与安全上限）+ `hap_resource.h`（资源模型抽象） |
| `interfaces/inner_api/` | `rstate.h`（错误码）+ `res_common.h`（KeyType/ResType 枚举）+ `resource_manager.h`（公共接口） |
| `interfaces/native/` | `resmgr_common.h` + `raw_file.h` / `ohresmgr.h`（Native 公共 API 兼容性） |
| `interfaces/js/kits/` | 对应 `resource_manager_napi.cpp`（NAPI 模块注册入口） |
| `interfaces/js/innerkits/core/` | `resource_manager_addon.cpp`（NAPI 共享实现）+ `libresmgr_napi_core.versionscript` |
| `interfaces/ets/ani/` | ANI 注册入口 + 预生成 `.abc` 字节码 |
| `interfaces/cj/` | `interfaces/cj/src/` FFI 绑定 |
| `dfx/` | `hisysevent.yaml`（事件定义）+ `hisysevent_adapter/hisysevent_adapter.h` |

### 按术语路由

当任务、issue、日志、API 名称或变更文件包含以下术语时，在规划前先读对应文档：

| 术语 | 风险提示 | 先读 |
|---|---|---|
| ResourceManager / RM | 不是通用"资源管理器"，而是 OpenHarmony 应用资源加载与匹配的核心对象，一个应用可拥有多个 RM 实例（每模块独立） | `interfaces/inner_api/include/resource_manager.h` + `frameworks/resmgr/include/resource_manager_impl.h` |
| resources.index | 不是通用"索引文件"，而是 restool 编译产出的二进制格式，有 V1（全量解析）和 V2（mmap 懒解析）两版本，结构体与 restool 写入端严格镜像 | `frameworks/resmgr/include/res_desc.h` + `hap_parser_v1.cpp` / `hap_parser_v2.cpp` |
| HapResource / HapManager | 不是通用"资源容器"，而是 OpenHarmony HAP 资源模型——HapResource 是单 HAP 资源抽象（有 V1/V2/System/Overlay 派生），HapManager 聚合多个 HAP 做跨 HAP 最优匹配 | `hap_resource.h` + `hap_resource_v2.h` + `hap_manager.h` |
| Overlay / 覆盖资源 | 不是通用"覆盖"，而是 OpenHarmony 应用动态覆盖机制，OverlayResource 在匹配时替换非 overlay 值，支持运行期增删 | `hap_resource_v2.cpp`（OverlayResource）+ `hap_manager.cpp` |
| ThemePack / 主题包 | 不是通用"主题"，而是 OpenHarmony 皮肤包与图标包加载机制，有独立的 ThemePackManager/ThemePackResource/ThemeConfig | `theme_pack_manager.h` + `theme_pack_resource.h` + `theme_pack_config.h` |
| HQF / 补丁 | 不是通用"补丁"，而是 OpenHarmony 应用级 HQF 补丁资源，通过 AddPatchResource 加载 | `hap_manager.cpp` |
| SystemResource / 系统资源 | 不是通用"系统资源"，而是 OpenHarmony 系统 HAP（ohos.global.systemres）的资源加载单例，按 ID 范围区分系统与应用资源 | `system_resource_manager.h` |
| LocaleMatcher / 区域匹配 | 不是通用"locale 比较"，而是 RFC-4647 风格的 locale 匹配与排序，含 likely-subtags 最大化（如 zh → zh-Hans-CN） | `locale_matcher.h` + `res_locale.h` |
| KeyParam / KeyType / 限定词 | 不是通用"键值对"，而是 OpenHarmony 资源限定词体系（LANGUAGE/REGION/SCRIPT/ORIENTATION/SCREEN_DENSITY/DEVICETYPE/COLORMODE/MCC/MNC/INPUTDEVICE），组合形成 `en_US-phone-dark` 目录名 | `interfaces/inner_api/include/res_common.h` |
| ResType / 资源类型 | 不是通用"类型枚举"，而是 OpenHarmony 资源类型体系（string/strarray/integer/intarray/boolean/color/float/plural/pattern/theme/symbol/media/rawfile/profile 等） | `interfaces/inner_api/include/res_common.h` |
| rawfile | 不是通用"原始文件"，而是 HAP 包内原样拷贝的资源，通过 Native OH_Raw* 系列接口和 RawFileDescriptor（fd 直通）访问 | `interfaces/native/resource/include/raw_file.h` + `raw_file_manager.cpp` |
| $r / 资源引用 | 不是通用"变量引用"，而是 ArkUI 组件中引用资源的语法 `$r('app.string.xxx')`，由 ace_engine 解析后转发到本模块 inner_api | `interfaces/inner_api/include/resource_manager.h` |
| RState / 错误码 | 不是通用"状态码"，而是本模块的错误码枚举，9001001-9001010 是已发布的公共错误码合约 | `interfaces/inner_api/include/rstate.h` |
| NAPI / ANI / CJ FFI / Native | 四种多语言绑定机制，共享核心引擎，签名变更都是公共 API 变更 | `interfaces/js/` + `interfaces/ets/ani/` + `interfaces/cj/` + `interfaces/native/` |
| mmap / 懒解析 | 不是通用"内存映射"，而是 V2 格式的按需解析策略——初次访问 GetIdValues(id) 时才读取值字节 | `hap_parser_v2.cpp` + `hap_resource_v2.cpp` + `mmap_file.h` |
| PsueManager | 不是通用"管理器"，而是伪区域管理，用于 override 资源管理器的临时配置覆盖（不污染应用原始配置） | `frameworks/resmgr/include/utils/psue_manager.h` |
| ResourceManagerExtMgr | 不是通用"扩展管理器"，而是通过 dlopen 厂商扩展 .so 注入自定义 ResourceManager 实现的加载器 | `frameworks/resmgr/include/resource_manager_ext_mgr.h` |
| DFX / HiLog / HiSysEvent | 不是通用"日志"，而是 OpenHarmony 故障归因体系，事件定义在 hisysevent.yaml 是硬性约束 | `hisysevent.yaml` + `hilog_wrapper.h` + `dfx/hisysevent_adapter/` |

在规划中声明：
- 任务类别
- 已读知识文档
- 发现的约束
- 是否需要使用 Skill

## 3. 约束与边界

### 架构与领域不变量

- 公共接口（`interfaces/inner_api/`、`interfaces/native/`、`interfaces/js/`、`interfaces/ets/`、`interfaces/cj/`）表达稳定的能力意图，不暴露核心引擎内部实现细节。
- 核心引擎（`frameworks/resmgr/`）通过 `interfaces/inner_api/include/resource_manager.h` 纯虚抽象接口对外暴露，应用侧不直接引用 `frameworks/resmgr/include/` 内部头文件。
- 本模块作为**进程内共享库**被 ability_runtime 和 ace_engine 调用，**无 IPC 服务、无 SA 注册**——所有调用都是同进程函数调用。
- `resources.index` 的二进制格式与 restool 写入端（`developtools/global_resource_tool`）字节布局严格镜像，结构体定义在 `frameworks/resmgr/include/res_desc.h`，不可随意修改。
- V1 格式（version 以 `"Restool "` 开头）走全量解析；V2 格式走 mmap 懒解析——版本判定在 `HapResourceManager::Load()` 中完成，两条解析路径独立维护。
- debug 签名的 HAP 可携带畸形 `resources.index`，本模块将其视为**不可信输入**——所有解析步骤必须校验 `offset + len <= bufLen`、count 字段不超上限、`KEYS`/`IDSS`/`DATA` magic tag。
- 安全上限常量定义在 `res_desc.h`：`MAX_RES_KEY_COUNT`、`MAX_KEY_PARAMS_COUNT=32`、`MAX_RES_TYPE_COUNT=64`、`MAX_RES_ID_COUNT=0x01000000`、`MAX_INDEX_FILE_SIZE=UINT32_MAX`，不可放宽。
- DFX 事件定义（`hisysevent.yaml`）是故障归因的硬性合约，事件名、参数类型、级别不可随意修改。
- `resmgr.gni` 中的特性标志（`resource_management_support_icu`、`resource_management_api_metrics`）控制编译时行为，默认值变更影响所有下游产品。
- 系统（ID ≥ `0x7000000`）与应用（ID ≥ `0x1000000`）资源 ID 范围区分，核心引擎按 ID/名称分发到对应资源。
- `libresmgr.versionscript` 限制 `global_resmgr` 仅导出 `OHOS::Global::Resource*` 符号——内部工具类不可泄露到动态符号表。
- override 资源管理器的配置变更不污染应用原始配置（通过 PsueManager 管理临时配置覆盖）。

### 禁止事项

- 不要在 `interfaces/inner_api/`、`interfaces/native/`、`interfaces/js/` 中修改公共 API 签名、错误码值、方法语义，除非任务明确要求且有兼容性评估。
- 不要修改 `rstate.h` 中已发布的错误码数值（9001001-9001010）——它们是公共合约。
- 不要修改 `res_desc.h` 中与 restool 镜像的结构体字段顺序和大小——它影响二进制兼容性。
- 不要修改 `hisysevent.yaml` 中已发布事件的参数类型或删除参数——影响线上故障归因。
- 不要放宽 `res_desc.h` 中的安全上限常量——它们是解析不可信输入的防护栏。
- 不要为通过测试而删除日志、HiSysEvent 事件、错误码或诊断信息。
- 不要在 `frameworks/resmgr/include/` 中新增对 `interfaces/js/` 或 `interfaces/ets/` 的依赖——核心引擎不反向依赖绑定层。
- 不要在 `interfaces/native/` Native 头文件中引入 C++ 特性——Native C ABI 必须保持纯 C 兼容。
- 不要跳过 BUILD.gn 更新——添加源文件后必须更新对应 BUILD.gn 和 `bundle.json` 的 sub_component 列表。
- 不要新增第三方依赖——必须先在 `bundle.json` 的 `component.deps` 中声明并通过 License 审查。
- 不要修改 `libresmgr.versionscript` 的导出范围——它定义了 inner_api 的符号边界。

### 须先确认

- 修改 `interfaces/inner_api/` 中任何已有 API 的签名或语义。
- 修改 `interfaces/native/` Native C ABI 头文件中已有函数签名。
- 修改 `interfaces/js/` NAPI 或 `interfaces/ets/` ANI 绑定中已发布的方法签名。
- 涉及 `resources.index` 二进制格式变更（结构体字段、magic tag、版本判定逻辑）——影响与 restool 的跨工具兼容性。
- 涉及资源匹配算法变更（`IsMoreSuitable`、`MatchBestResource`）——影响所有应用的资源选择结果。
- 涉及 locale 匹配/排序逻辑变更——影响多语言环境下的资源匹配。
- 修改 `rstate.h` 中已发布的错误码数值或新增错误码。
- 修改 `hisysevent.yaml` 中已发布事件的定义。
- 修改 `resmgr.gni` 中特性标志的默认值（`resource_management_support_icu`）。
- 新增或修改第三方依赖、License。
- 删除或重命名公共 API。
- 修改安全上限常量（`res_desc.h` 中的 MAX_* 常量）。
- 修改 `libresmgr.versionscript` 导出符号范围。

### 反模式（不要这样做）

- ❌ 在 `frameworks/resmgr/` 中直接引用 `interfaces/js/` 或 `interfaces/ets/` 的头文件（核心引擎不应反向依赖绑定层）
- ❌ 不经兼容性评估修改公共 API 签名或错误码值（应先评估影响范围）
- ❌ 修改 `res_desc.h` 结构体字段但不与 restool 写入端同步（应同时更新两端）
- ❌ 为通过测试删除 HiSysEvent 事件或日志（应修复根因）
- ❌ 放宽安全上限常量以绕过解析校验失败（应排查输入数据合法性）
- ❌ 添加源文件但不更新 BUILD.gn 和 bundle.json（应同步更新构建定义）
- ❌ 修改特性标志默认值但不通知产品配置团队（应先协调）
- ❌ 在 Native C ABI 头文件中引入 C++ 特性（应保持纯 C 兼容）
- ❌ 在 override 资源管理器中直接修改应用原始配置（应通过 PsueManager 管理临时覆盖）

## 4. 验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

### 最小验证

- 编译验证：`./build.sh --product-name <product> --build-target resource_management`
- 编译核心库：`./build.sh --product-name <product> --build-target global_resmgr`
- 编译 rawfile 库：`./build.sh --product-name <product> --build-target librawfile`
- 编译 Native 库：`./build.sh --product-name <product> --build-target ohresmgr`
- 编译 NAPI 库：`./build.sh --product-name <product> --build-target resourcemanager`
- 单元测试：`run -t UT -tp resource_management`
- 模糊测试：`run -t UT -ts resmgr_fuzzer`

### 按变更类型验证

| 变更类型 | 最小验证 |
|---|---|
| 修改核心引擎（frameworks/resmgr/src/） | 编译通过 + `resource_manager_test` 及相关单测（hap_manager_test / hap_parser_test / hap_resource_test） |
| 修改资源匹配算法 | 编译通过 + `res_config_impl_test` + `res_config_test` + `resource_manager_test` |
| 修改 resources.index 解析逻辑 | 编译通过 + `hap_parser_test` + `res_desc_test` + `resource_manager_v1_v2_test` + `resmgr_fuzzer` 模糊测试 |
| 修改 locale 匹配逻辑 | 编译通过 + `locale_info_test` |
| 修改 overlay/主题包逻辑 | 编译通过 + `theme_config_test` + `theme_manager_test` + `theme_resource_test` |
| 修改系统资源管理 | 编译通过 + `system_resource_manager_test` |
| 新增/修改公共 API（interfaces/inner_api/） | 编译通过 + 全量单测 + NAPI/ANI/CJ/Native 各语言绑定编译 + 兼容性评估 + `rstate.h` 错误码一致性检查 |
| 新增/修改 NAPI 绑定 | 编译通过 + 对应 NAPI 模块单测 |
| 新增/修改 ANI 绑定 | 编译通过 + ANI 模块编译 + `.abc` 字节码生成验证 |
| 新增/修改 Native C ABI | 编译通过 + `native_resource_manager_test` + Native 头文件兼容性检查 |
| 修改 DFX/日志 | 编译通过 + `hisysevent.yaml` 事件定义未被破坏 |
| 修改特性标志 | 编译通过 + 受影响模块单测 + 检查 `resmgr.gni` 与 `bundle.json` features 一致性 |
| 修改 rawfile 访问 | 编译通过 + `rawfile_path_test` + Native rawfile 接口验证 |
| 仅测试变更 | 运行变更的测试 + 至少一个相邻相关测试 |

### 完成定义

任务完成仅当：
1. 请求的行为已实现。
2. 相关编译/测试/lint/兼容性检查已运行，或已说明无法运行的原因。
3. 最终回复包含：变更摘要、变更文件列表、验证命令和结果、兼容性/DFX 影响评估（如相关）。
4. 不包含无关的格式化、重构或附带变更。
5. 如涉及公共 API 变更，已标注兼容性影响。

### 最终回复格式

完成任务时，回复应包含：
- 变更摘要
- 变更文件列表
- 验证命令和结果
- 兼容性、DFX 或跨工具影响（如相关）
- 遗留风险或后续项

## 5. 代码检视

当用户表达"检视一下代码""请审一下""做一次 code review""深度扫描""安全审计""查高危 bug""接口审计"等检视意图时，**不要凭记忆审查**——优先通过 `skill` 工具加载可用的代码检视 skill，按其工作流执行。无匹配 skill 时，依据以下维度与约束手动执行。

### 检视流程

1. **界定范围**：明确目标路径（`frameworks/resmgr/src/`、`interfaces/` 等）或检视重点（通用/安全/高危 bug/API/外部输入）。未明确时向用户确认。
2. **执行检视**：按下方维度速查选择侧重点；通用检视应同时覆盖 bug + 逻辑 + 安全三个维度。
3. **合并报告**：把各维度发现汇总为一份 `codecheck_report_<scope>_<YYYYMMDD>.md`，跨维度去重后按 P0/P1/P2 排序。

### 检视维度速查

| 维度 | 何时用 |
|------|--------|
| 高影响缺陷（崩溃/挂死/OOM/UAF/死锁/泄漏） | 通用检视、合入前排查 |
| 逻辑影响与一致性 | 变更影响评估、匹配算法/边界 |
| 安全/商用前审查 | 安全专项、商用前 |
| 外部输入→持久化健壮性 | resources.index 解析、mmap、rawfile 访问 |
| 对外 API 一致性 | 接口/SDK 变更、inner_api/Native/NAPI 审计 |

### 检视约束

- **静态语言绑定默认排除**（`interfaces/ets/ani/`、`interfaces/cj/`），除非用户明确要求包含。
- **重点检视区**：`frameworks/resmgr/src/hap_parser_v1.cpp` / `hap_parser_v2.cpp`（二进制解析、不可信输入）、`hap_resource_v2.cpp`（mmap 懒解析）、`res_config_impl.cpp` + `hap_manager.cpp`（资源匹配逻辑）、`raw_file_manager.cpp`（文件访问）。
- **证据要求**：每条发现可追溯到 `file:line` + 触发路径，不收无证据的代码气味项。
- **检视阶段不改源码**：只产出报告与建议；修复由用户确认后另起任务，并回到本 AGENTS.md 的约束与边界/验证章节执行。
