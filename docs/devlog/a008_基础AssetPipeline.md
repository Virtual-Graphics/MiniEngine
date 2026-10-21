---
author : Artichoke
---

终于来了 Asset Pipeline , Asset Pipeline 是游戏引擎中用于管理外部资源的系统，为什么叫做 Pipeline ，因为外部的资源会通过一道道工序，变成我们需要的样子，就和工厂的流水线管道一样，比如一个 .obj 文件是不能直接交给渲染器渲染的，提交给渲染器的的资源至少是 MeshHandle MaterialHandle 这类，所以 Pipeline 会将 .obj 加工成这种我们引擎内部需要的样式。  

具体的较为完整的 Asset Pipeline （不准确）例子：.obj -> import / 放入 assets 目录下 -> 被 file watcher 发现内容变更 -> 触发 reimpoter / impoter -> editor 会使用默认的或者配置过的 impoter 设置生成对应的 .meta 文件 -> 并生成二进制缓存文件 -> Core 中的 Asset Manager 加载至 CPU 内存可读形式 -> 系统需要时通过 AssetID 向 Asset Manager 请求资源。

在设计 Asset Pipeline 之前，我们使用了很多临时方案来代替资源价值和资源管理的功能，当然我们第一版设计的需要简单一些，我们需要保持简单和基本可用。

## 第一版目标和范围

完成 Mesh、Material 从源文件导入、生成缓存、运行时加载与缓存，到场景通过 AssetID 引用、保存恢复和实际渲染的完整闭环。

| 资源类型 | 源文件 | 导入缓存 | 内容 |
| --- | --- | --- | --- |
| Mesh | `.obj` | `.vmesh` | 当前 MeshData 所需的顶点和索引 |
| Material | `.vmat`，YAML | `.vmatc` | 漫反射色、高光色、光泽度 |

Material 是独立资源，可以由多个实体共享。第一版沿用当前渲染材质能力，暂不加入 Texture、贴图引用和 OBJ 的 MTL 材质导入；材质由编辑器创建和指定。

实现采用同步导入、同步加载、手动重新导入，以及关闭项目时统一释放缓存的方式。自动文件监听、异步任务、增量构建和复杂依赖追踪留到实际需要时再设计。


## 资源身份与文件组织

复用现有 UUID 作为 AssetID，并在 `asset/asset_id.h` 中区分 `MeshID` 与 `MaterialID`。持久化的资源身份与 Renderer 的 MeshHandle、MaterialHandle 分开；场景保存 AssetID，渲染句柄仅在当前运行期间有效。

Asset 目录沿用 `.vproj` 中的配置，导入缓存统一放在项目根目录的 `.vulcal_engine/` 下。例如：

```text
项目/
├─ sandbox.vproj
├─ assets/
│  ├─ bunny.obj
│  ├─ bunny.obj.meta
│  ├─ white.vmat
│  ├─ white.vmat.meta
│  └─ scenes/
│     └─ main.vscene
└─ .vulcal_engine/
   ├─ <MeshID>.vmesh
   └─ <MaterialID>.vmatc
```

源文件与 `.meta` 一起提交版本库，移动或重命名源文件时也要一起移动对应的 `.meta`，以保留身份。丢失 `.meta` 后重新生成 ID，会使原来的场景引用失去对应关系。

`.vulcal_engine/` 加入忽略列表，其中的文件均可通过源文件重新生成。第一版直接按十进制 AssetID 和缓存类型确定文件路径。


## `.meta` 元数据

`.meta` 保存资源身份和导入配置。第一版先包含以下字段：

| 字段 | 含义 |
| --- | --- |
| `format_version` | `.meta` 自身的格式版本 |
| `id` | 主要导入缓存的 AssetID |
| `importer` | 处理源文件的导入器，目前为 `obj`、`material` |

例如 `bunny.obj.meta`：

```yaml
format_version: 1
id: "1234567890123456789"
importer: obj
```

材质的 `.meta` 使用相同结构，`importer` 为 `material`。

当前 OBJ 的顶层 ID 对应一个 Mesh，材质源文件的顶层 ID 对应一个 Material。顶层 `importer` 描述如何处理源文件，实际缓存类型由导入器确定。

具体导入选项确定后，再加入 `import_settings`。

源路径由源文件和 `.meta` 的位置确定；导入状态和错误信息由 AssetDatabase 记录和展示；缓存路径由 ID 和类型推导。第一版不在 `.meta` 中保存这些派生信息，也不增加时间戳、内容哈希或依赖列表。

## 导入与重新导入

用户手动把源文件放入项目的 Asset 目录，第一版不提供从外部复制文件的 Import 操作。

打开项目或点击 Content 面板的刷新时：

1. 扫描 Asset 目录，识别当前支持的 OBJ 和 Material 源文件。
2. 为新资源生成 UUID 和对应的 `.meta`；已有资源沿用其 ID。
3. 在内存中建立资源记录，生成缺失的导入缓存。
4. 在 Content 面板显示资源及导入结果。场景、图片等其他文件暂不进入这两类资源的导入流程。

已有源文件修改后，由用户手动执行 Reimport；普通刷新不检测内容变化，也不自动重新导入已有缓存。

Reimport 沿用原 AssetID，同步读取源文件并生成缓存。成功后使对应的 AssetManager 缓存失效，下次使用时加载新缓存。失败时显示文件路径和错误原因，保留已有缓存与已加载资源；首次导入失败的资源保持不可用状态。

编辑材质并保存时，先更新 `.vmat` 源文件，再执行同样的重新导入流程。

## 导入缓存

Mesh 和 Material 都生成简单的二进制缓存，Runtime 统一读取这些缓存。

- `.vmesh` 保存顶点、索引及读取它们所需的数量信息。
- `.vmatc` 保存当前材质的漫反射色、高光色和光泽度，共 7 个浮点数。
- 缓存包含格式标记和版本号，逐项写入明确的数据字段，避免直接写入含有容器或布局填充的 C++ 对象。

缓存格式的编解码由 Core 的 `mesh_bin` / `material_bin` 统一定义，避免读写两份布局；外部导入模块负责解析源文件并决定何时写入。Runtime 不需要解析 OBJ、材质 YAML 或 `.meta`，也不需要扫描源文件目录。

## Core 与外部模块的职责

沿用 [a007](./a007_基础ProjectSystem.md) 的归属原则：Runtime 和 Core 系统设计不需要的功能，应放在 Core 外部。

| 部分 | 所属 | 职责 |
| --- | --- | --- |
| AssetID | Core | 表达持久化资源身份 |
| 源文件解析与缓存生成 | 外部导入模块 | 解析 OBJ、读取材质 YAML、生成导入缓存，供 Editor 和 Model Viewer 共用 |
| AssetDatabase | APPS | 扫描 Asset 目录、维护 `.meta`，在内存记录 ID、资源类型、源路径和导入状态 |
| AssetManager | Core | 根据 ID 读取缓存，创建并缓存 Renderer 句柄，管理缓存失效和释放 |
| Renderer | Core | 持有实际 GPU 资源，提供创建、使用和销毁接口 |
| Content、Inspector 的资源操作 | Editor | 浏览资源、创建与编辑材质、给实体赋值、触发导入 |

外部导入模块独立于 Editor，避免 Model Viewer 为解析模型而依赖编辑器 UI、项目会话或 AssetDatabase。第一版按实际类型调用具体导入函数即可。

现有 `vulcal_engine/asset/` 中的 OBJ、图片源文件 loader 移出 Core，相关源格式解析依赖也移到外部。图片 loader 的迁移不代表本轮增加 Texture Pipeline。MeshData、运行时缓存读取和渲染资源数据仍属于 Core。

现有 MeshCache 删除，其加载、复用和释放职责由 AssetManager 接替。Core 不参与源文件扫描、`.meta` 维护或重新导入。

## AssetManager 与场景衔接

AssetManager 接收 `.vulcal_engine` 目录，提供获取 Mesh、获取 Material、使单个资源缓存失效和清空全部缓存的能力。同一资源重复使用时复用渲染句柄，GPU 对象继续由 Renderer 持有。

`MeshRendererComponent` 改为保存 Mesh AssetID、Material AssetID 和可见性。场景序列化保存这两个 ID，替换原来的 Mesh 路径引用，并让材质从不持久化的瞬态句柄改为持久化 AssetID。

SceneExtractor 通过 AssetManager 将 ID 解析为 MeshHandle 和 MaterialHandle，再构建 RenderData，取消路径拼接和 MeshCache 的连接。Material ID 为空时使用共享默认材质；Mesh ID 为空或资源加载失败时跳过无法完成资源解析的绘制，并提供错误诊断。

关闭或切换项目时清空 AssetManager 的缓存，在 Renderer 仍然有效时释放对应网格和材质。第一版统一管理项目期间的资源生命周期，不引入逐实体引用计数或自动资源回收策略。


## 具体文件

具体的应该至少创建如下代码文件：  
1. vulcal_engine/asset  
- asset_manager.h/.cpp   
- asset_id.h , 提供具体类型的 assetid ，如 asset::MeshID ,asset::MaterialID 等等  
- mesh_bin.h/.cpp 用于定义规范约定 .vmesh 二进制文件的格式，以及它的 decoder/encoder
- material_bin.h/.cpp 用于定义规范约定 .vmatc 二进制文件的格式，以及它的 decoder/encoder 
- 其他需要的模块。  

2. apps/common/asset  
- obj_loader.h/.cpp
- image_loader.h/.cpp，本轮不建 image importer
- material_file.h/.cpp，读写 `.vmat` YAML
- obj_importer.h/.cpp
- material_importer.h/.cpp
- asset_database.h/.cpp，只接收路径，不依赖 Editor
- 其他需要的模块

第一版不抽象 loader / importer 接口，按具体资源类型直接调用。

3. apps/editor/panels
- content_panel.h/.cpp

## 编辑器交互

第一版增加简单的 Content 面板，平铺显示资源名称和类型，并提供：

- 刷新：扫描目录、发现新资源、生成缺失缓存。
- Reimport：重新导入选中资源。
- 创建材质：填写名称，生成 `.vmat`、`.meta` 和材质缓存。

Hierarchy 与 Content 共用 Inspector，通过当前选择决定显示内容：

- 选中实体：显示组件，MeshRenderer 提供 Mesh、Material 下拉选择，替换临时路径文本输入。
- 选中 Mesh：显示源路径、AssetID 和导入状态。
- 选中 Material：显示资源信息，并编辑漫反射色、高光色、光泽度；点击保存后更新源文件和导入缓存。

选择状态由 EditorContext 统一管理。第一版的 Content 面板以完成上述操作为目标，目录树、拖拽赋值和缩略图可后续补充。

## Model Viewer

Model Viewer 保持独立的渲染预览工具定位，继续接受 `model_viewer.exe <path-to-model.obj>`，给定源模型即可展示渲染器效果。未来再增加 GLB、glTF、FBX 等输入格式。

它复用外部导入模块中的源文件解析步骤，在内存中得到 MeshData 后直接交给 Renderer，自行管理预览所需的网格、材质和场景。该流程不要求创建项目、生成 `.meta`、建立 AssetDatabase 或写入磁盘导入缓存。

Editor 则在源文件解析后继续生成缓存，再通过 Core AssetManager 加载。两条流程共享解析代码，各自保留所需的后续处理。

## 后续的复合资源与打包

### glTF 完整模型

未来支持 glTF 时，需要能够把完整模型放入场景，保留节点层级、局部变换、多个网格及其材质关系。因此一份源文件可能生成多份缓存，其中描述组合关系的 Model 是主要资源，子资源包括 Mesh、Material，后续还可能包括 Texture。

届时 `.meta` 的顶层 ID 对应完整 Model，再增加 `subassets` 保存各个子资源的稳定 ID。重新导入时如何匹配源文件中的子资源，留到实际实现 glTF 时确定。

同时区分两类关系：

- 源文件依赖：例如 glTF 引用的 `.bin`、图片，由导入器发现，供编辑阶段使用。
- 缓存引用：例如 Model 引用 Mesh、Material，Material 引用 Texture，通过 AssetID 写入缓存，供 Runtime 使用。

第一版仍只实现 OBJ Mesh 和独立 Material；Model 资源、节点层级、模型实例化、子资源管理和依赖追踪在 glTF 阶段再实现。

### `.vpak`

未来计划把导入缓存打包为一个 `.vpak` 文件。届时调整运行时读取缓存的方式，场景继续通过 AssetID 引用资源。当前直接从 `.vulcal_engine` 目录加载，不提前实现打包器或多种存储后端。

## 第一版验收

- 手动放入 OBJ 后，刷新能生成 `.meta` 和 `.vmesh`，Content 面板可以选择该资源。
- 创建、编辑和保存 Material 后，生成 `.vmat`、`.meta`、`.vmatc`，指定给实体后能看到材质效果，多个实体可以共享材质。
- 保存场景并重开项目后，两个 AssetID 及其渲染结果恢复；移动源文件与配套 `.meta` 后保留身份。
- 删除 `.vulcal_engine` 后重新打开项目，可以从源文件重新生成缓存并恢复渲染。
- 手动 Reimport 能更新显示；导入失败时报告错误并保留原有可用缓存。
- Model Viewer 可以在没有项目的情况下预览 OBJ。
- Core 可以独立构建，运行时加载不依赖源文件解析器、YAML、Editor 或 AssetDatabase。

场景格式升级为 v2，Mesh / Material 保存十进制 AssetID。v1 不提供迁移代码，打开时报告格式不兼容；sandbox 场景已手动转换。
