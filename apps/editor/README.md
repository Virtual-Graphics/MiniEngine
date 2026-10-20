# Editor

源码按职责组织：

| 目录 | 职责 |
| --- | --- |
| `app/` | 应用入口、EditorLayer、当前项目/场景与选择状态 |
| `dialogs/` | ImGui 对话框及原生文件对话框封装 |
| `panels/` | Hierarchy、Inspector、Viewport 面板 |
| `project/` | 项目数据与项目/场景操作会话 |
| `scene/` | 默认场景工厂与独立编辑器相机 |
| `serialization/` | 项目/场景 YAML 格式、校验和文件读写 |
| `sandbox/` | 可通过 File → Open Project 打开的示例项目 |

Editor 启动时不自动打开项目。手动验证时选择 `sandbox/sandbox.vproj`，
即可恢复其中保存的场景。项目和场景中的资源路径均相对于 `.vproj` 所在目录。

Mesh Renderer 暂用 Inspector 的 `Mesh path` 文本框填写 OBJ 相对路径（如
`assets/StanfordBunny.obj`），按 Enter 应用，留空后按 Enter 清除。保存场景时仍会
校验资源路径位于项目的 Asset 目录。此输入和 OBJ 缓存都是 Asset Pipeline 出现前的脚手架；
缓存不自动重试失败的文件，重新打开项目会清空缓存。

序列化和对话框依赖只属于 Editor；关闭 `BUILD_APPS` 后，Core 仍可独立构建。
