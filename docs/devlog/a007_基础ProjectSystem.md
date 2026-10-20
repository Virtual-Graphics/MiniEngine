---
author : Artichoke
---

## 背景与目标

Editor 完成后，开始实现供编辑器使用的 Project System，用于保存项目信息。项目信息使用 yaml-cpp（通过 CMake FetchContent 引入）序列化为 `.vproj` 文件。`.vproj` 所在目录即项目根目录，文件中的相对路径均以该目录为基准。编辑器启动时默认不打开任何项目。

第一版项目信息包括：

- 格式版本
- 项目名称
- 作者
- Asset 目录（路径相对于 `.vproj`）
- 上一次编辑的场景（相对于 `.vproj`路径，指向 `assets/scenes/xxx.vscene`）

## 依赖与编辑器交互

文件对话框使用 [nativefiledialog-extended](https://github.com/btzy/nativefiledialog-extended)（通过 CMake FetchContent 引入），避免让用户手动输入路径。

编辑器需要提供通用的 ImGui 对话框窗口抽象接口，并基于该接口实现 New Project 对话框：填写项目信息后创建项目。操作逻辑应该是 Editor -> File -> New Project -> 弹出 New Project 对话框 -> 填写项目名称 -> 通过对 nativefiledialog-extended 的封装选择 Project 目录 -> 填写作者名称（可选）-> 填写 Asset 目录（默认是 assets ）-> create project，然后 Editor 应该创建一个默认场景，其中应该包含一个 Sun (Directional Light) 、一个 Camera，该默认场景此时没有被保存在 Project 的 assets/scenes/ 目录需要用户决定是否保存以及保存在哪。

### New Project

- Project 目录不能为空。
- 自动创建 assets 目录。
- 如果目录下已经存在 `.vproj`，则不能再创建项目；不覆盖已有文件。
- 创建默认场景。

### Open Project

直接调用 nativefiledialog-extended 打开系统文件对话框选择项目。

打开项目时自动恢复 `last_scene`；如果 `last_scene` 指向的文件丢失，则加载默认场景。

## 项目文件格式

`.vproj` 内容类似：

```yaml
# xxx.vproj
format_version: 1
project:
  name: Demo
  author: Artichoke
  asset_directory: assets
  last_scene: assets/scenes/main.vscene
```

其中 `asset_directory` 是相对于 `.vproj` 的路径，`last_scene` 使用相对于 `.vproj` 路径指向 `assets/scenes/xxx.vscene`。

## Scene 序列化与反序列化

第一版提供 Scene 的序列化与反序列化功能，Scene 文件后缀为 `.vscene`，Asset 暂时使用路径索引。

C++ 目前没有反射特性（静态反射要到 C++26 才引入），因此在 v0.1.0 之前不引入各种取巧的反射实现，而是手写每个 Component 的反射信息，以及 Component 在 Inspector Panel 中显示的信息。由于未来会引入反射系统，这部分确定会重构，所以第一版直接将所有组件的信息写入 Scene 的序列化/反序列化代码中。

`MeshRendererComponent` 目前使用 `render::MeshHandle` 与 `render::MaterialHandle`，无法持久化保存，因为不能通过保存的数值重建这些资源。因此第一版需要暂时将 `render::MeshHandle` 改为 `std::filesystem::path` 或者 `std::string` ，在 SceneExtractor 中生成所需的 `render::MeshHandle`，并且缓存到 vulcal_engine/asset/mesh_cache 下（未来我们做好 AssetPipeline 就会简单许多加载的 Asset 会进行系统化统一的管理）。`render::MaterialHandle` 尚未定义文件格式，所以该字段暂时不进行序列化/反序列化，留到开发 Asset Pipeline 时再解决，暂时所有的物体都用同一个默认材质。

## Scene 文件操作

提供 New/Open/Save/Save As Scene 功能：

- New：直接在 Editor 中新建 Scene 一个默认场景。
- Open Scene / Save As：调用 nativefiledialog-extended 打开系统文件对话框进行操作。
- 暂时不标记 Scene 的 dirty 状态，也不实现 undo / redo。

## 功能归属：Core 还是 Editor

序列化与反序列化明确属于 Editor 功能。判断一个功能是否应放入 Core 的方法是：如果一个功能在 Runtime 中，或者在设计 Core 所需的系统时用不到，那么就不应放入 Core。显然，序列化和反序列化只有编辑器会用到，因此不应放入 Core。

## 当前代码状态

- `EditorLayer::on_attach()` 目前直接创建默认场景；这一行为需要丢弃，改为启动时不打开项目。
- `MeshRendererComponent` 目前持有 `render::MeshHandle` 与 `render::MaterialHandle`，需要按上述方案调整持久化方式。
