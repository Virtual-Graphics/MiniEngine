---
author : Artichoke
---

对于 ImGUI 我的想法是引擎的内核中不应该出现 ImGUI ，除非我们有真实的理由需要在 Runtime 时使用 ImGUI （也许未来我们需要类似 Runtime 控制台终端的东西，用于对 Runtime 进行 Debug 调试等等），但是目前我的想法是尽可能让内核保持轻量，外部应用、工具链等自行配置 ImGUI （现在想如果 Runtime 需要使用 ImGUI 内核也没必要依赖 ImGUI）。但是这需要直接依赖我们渲染器的 gfx 这不是问题，（但如果这样我们的渲染器就变成不可替换了，如果想实现可替换渲染器则需要引擎自己维护一套图形 API 对渲染器的进行适配，这并没有减少多少工作量，并且性能也会下降，所以对于这个引擎不要求可替换渲染器这个功能，不过我们引擎的渲染器依然能单独拿出去使用）。

现有的 app - model viewer 我打算专门作为我们渲染器的验证应用，包括检验功能，性能统计，调试 Debug 等等，所以它需要依赖 ImGUI。除此之外未来的 Editor 也需要依赖 ImGUI ,所以我们可以在 apps 下创建一个共享通用的 common 的目录，其中可以依赖内核创建公用的 ImGUI 代码。所以我们其实可以提供两种内核静态库，一种是无 ImGUI 版本的（现有的 vulcal_engine ），一种是与 common 一起的带有 ImGUI 版本的 (新增应用侧静态库 vulcal_app_imgui，不能说是内核把，是依赖于 vulcal_engine 构建的静态库)，需要 ImGUI 的应用链接 ImGUI 版本的内核，不需要 ImGUI 的应用链接无 ImGUI 版本的内核。

## RenderTarget 相关
对于带 ImGUI 内核的版本我们需要考虑两种情况：
1. ImGUI 窗口 override 在 渲染结果上 （普通情况），此时其实就是一种多 pass 做法，渲染器正常渲染，最后添加一个 pass 渲染 ImGUI ，不过这个 pass 不在渲染器中而是在 APP 中自己创建。  
2. 将渲染结果以纹理的形式呈现在 ImGUI 纹理上 （如 Editor ，Thumbnail等等），这种情况我们在设计渲染器的时候已经提前考虑过了，我们渲染器提供了渲染到 RenderTarget 的方案，纹理可以直接使用 RenderTarget 的数据进行呈现。