---
author : Artichoke
---

在完成 Editor 之后，我认为我们可以开始做提供给编辑器使用的 Project System 了，Project System 是用于给编辑器保存一些项目信息的，使用 yaml-cpp （使用 cmake fetch content 引入）序列化项目信息到 .vproj，

第一版的项目信息应该包括：

版本信息
项目名称
作者
Asset 目录（路径相对于.vproj）
上一次编辑的场景（相对路径指向 assets/scenes/xxx.vscene ）
约定好项目文件 .vproj 所在的目录是项目的根目录。

除此之外我们需要提供一种 file dialog 的功能，因为我们不可能让用户每次都手动输入路径，对此我们可以使用 https://github.com/btzy/nativefiledialog-extended 这个库（使用 cmake fetch content 引入）。
编辑器应该提供一种通用的 ImGUI 对话框窗口抽象接口，然后创建一个 New Project 对话框窗口，填写对应的信息并创建。编辑器默认不打开任何项目。

除了 New Project，还需要 Open Project ，Open Project 直接调用 nativefiledialog-extended 打开系统文件对话框进行选择就好了。

同时我们应该提供 Scene 的序列化与反序列化功能，Asset 暂时使用路径索引，场景文件后缀使用 .vscene。对于 Scene 的 Serialization/Deserializtion 由于其中最多的最重复的就是对个个 Component 的 Serialization/Deserializtion 这很繁琐，但由于 C++ 并没有反射特性（C++26 才引入静态反射），所以在 v0.1.0 版本之前我并不打算引入各种奇技淫巧的反射功能，我们直接手写死 Component 的反射信息，以及编辑器中 Component 在 Inspector Panel 的信息，由于未来会引入反射系统，所以未来确定是会进行重构的，所以我们直接把所有组件的反射信息写入 Scene Serialization/Deserializtion 中。  

需要注意的是，`MeshRendererComponent` 中我们使用的是 `render::MeshHandle` 与 `render::MaterialHandle` 这导致不能持久化保存，因为我们不能通过保存的值重建这些信息，所以我们需要暂时将 `MeshRendererComponent` 改为 path 引入，在反序列化时直接加载，并且我们现在的 `render::MaterialHandle` 没有定义文件格式，所以暂时我们不对这个字段进行 Serialization/Deserializtion 留到我们开发 Asset Pipeline 时再解决。

同时需要提供 New/Open/Save/Save As Scene 功能，暂时不标记 Scene 的 dirty 状态，也不做 undo redo。 

具体的 New 时直接在 Editor 中新建 Scene ， Open Scene / Save As 直接调用 nativefiledialog-extended 打开系统文件对话框进行操作。

序列化与反序列化相关的它们明确的是 Editor 功能，这里给出如何判断一个功能是否应该放在 Core 的方法：如果一个功能在 Runtime ，或者设计 Core 需要的系统的时候用不到，那么明确不应该放入 Core 。很显然序列化和反序列化有且仅有编辑器会使用到，所以不应该放入 Core 。


具体的，.vproj 应该类似下面这种格式  
``` 
# .vproj
format_version: 1
project:
  name: Demo
  author: Artichoke
  asset_directory: assets
  last_scene: assets/scenes/main.vscene
```

New Project 行为 ：Project 目录不能为空，assets 目录自动创建，如果目录下已经存在一个 .vproj 则明确不能再创建一个项目，并且不进行覆盖，不创建默认场景。  
Open Project 时自动恢复 last_scene 如果 last_scene 目标丢失，则加载空场景  
目前 on_attach() 现在直接创建默认场景，需要丢弃