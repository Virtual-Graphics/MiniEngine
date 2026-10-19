---
author : Artichoke
---

在完成 Editor 之后，我认为我们可以开始做 Project System 了，Project System 是用于给编辑器保存一些项目信息的，使用 yaml-cpp 序列化项目信息到 .miniproj，

第一版的项目信息应该包括：

版本信息
项目名称
作者
Asset 目录
上一次编辑的场景（未来可以移到本地用户配置)  
约定好项目文件 .miniproj 所在的目录是项目的根目录用 '/' 表示。

编辑器应该提供一种通用的对话框窗口，然后创建一个 New Project 对话框窗口，填写对应的信息并创建。编辑器默认不打开任何项目。除了 New Project，还需要 Open/Close Project。

同时我们应该提供 场景的序列化与反序列化功能，Asset 暂时使用路径索引，场景文件后缀使用 .miniscene。
同时需要提供 New/Open/Save/Save As Scene 功能，关闭场景、切换项目和退出时，提供“保存／放弃／取消”