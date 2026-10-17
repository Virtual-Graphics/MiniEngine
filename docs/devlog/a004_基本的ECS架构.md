---
author : Artichoke
---

# 实现基本的 ECS 架构

我们使用 [Entt](https://github.com/skypjack/entt) 这个库作为我们的 ECS 框架，我们将在 Entt 的基础上搭建出 Scene Entity Component ，并且在第一版中我们不实现 Entity 的层级结构。  

此外我们需要提供第一版 Component 包括：  
- ID Component  
- Tag Component  
- Transform Component
- Mesh Renderer Component
- Camera Component
- Environment Component
- Light Component

并且我们的 Scene 应该提供一种复制的方法，因为未来我们的编辑器会实现类似 Edit Mode、Play Mode、Simulate Mode。当我们从 Edit Mode 切换到其他 Mode 的时候，这需要我们保存现有的 Scene ，然后切换到其他 Mode ，这会导致 Scene 发生变化，我们希望在返回 Edit Mode 的时候能够恢复刚开始的样子。

在实现好 Scene 模块基本的结构后，我们需要实现 Scene Extractor , Scene Extractor 会从 Scene 中提取出 Renderer 所需的数据，然后在 APP 中组织数据传递，但是有时候我们不需要使用 Scene 中的主 Camera ，比如 Editor Camera，Thumbnail 等等，这需要我们提供多种策略给 Scene Extractor 。

- Scene 的职责是管理 Entity 包括 CRUD，CRUD 再具体一点是 create_entity、destory_entity、add_component、get_component、has_component 、remove_component 等等。
- Entity 是 entt::Entity 的引用
- Scene Extractor 的职责是从 Scene 中提取 RenderData 。
