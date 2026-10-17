---
author : deepseek v4.1 flash
---
# Shadow Pass

a002 里定下的第一版线性管线是 `shadow pass -> base pass`，这一篇把 shadow pass 从纸面落到代码里。目标很克制：只做一张平行光的阴影贴图，PCF 3x3，不碰 CSM、不做视锥自适应、不做阴影剔除。

## 为什么先做深度-only 的 RenderTarget

阴影贴图只需要深度，不需要颜色。原来的 `RenderTarget` 强制创建一个 color attachment，如果为了阴影去挂一张永远不看的 RGBA 贴图，既浪费显存也让接口语义变得含糊。

所以把 `Spec::color_format` 改成 `std::optional`，同时增加 `depth_sampler`，让深度附件也能配置成比较采样器（`compare_func = LessEqual`），着色器里就能直接用 `sampler2DShadow`。有两点必须注意：

- depth-only 的 framebuffer 必须显式设置 `glNamedFramebufferDrawBuffer(GL_NONE)`，否则 FBO 不完整；`glReadBuffer` 也一并设为 `GL_NONE`。
- `color()` 从返回引用改成返回指针，因为 depth-only 时它不存在。接口变化不大，但调用方需要改，`render_to_texture` 测试同步更新了。

## pass 抽象

原来的 `Pipeline` 把 program、UBO、pipeline state 全塞在一起，加第二个 pass 之后就开始变味了。这次抽出一个 `Pass` 基类：

- 公共部分（program、pipeline state、shader 加载、状态应用）放在基类；每个 pass 只需要实现 `prepare` / `draw` / `finish`，以及可选的 `enabled` / `name`。
- pass 之间通过 `PassContext` 传递结果：`ShadowPass` 在 `prepare` 里写入 `shadow_map_` 和 `light_view_proj_`，`ForwardPass` 直接读，不需要额外接口。
- `Pipeline` 构造时按顺序把 pass 放进 `passes_`，渲染时遍历即可；顺序就是创建顺序，加一个 pass 只需在构造函数里 push 一行。

```cpp
Pipeline::Pipeline() {
    passes_.push_back(std::make_unique<ShadowPass>());
    passes_.push_back(std::make_unique<ForwardPass>());
}
```

`Renderer` 负责把资源句柄解析成 `DrawItem{ mesh, material, world_transform }`，pass 只看到已经解析好的数据，不再接触 handle map。

这样职责边界清楚，后面换 Frame Graph 时，遍历 `passes_` 的地方直接换成按依赖排序的 node 图即可。

## 光源相机放在渲染器内部

`RenderData` 里只有光的朝向和一组阴影参数（`center_`、`extent_`、`near_`、`far_`、`map_size_`），没有矩阵。Extractor 不需要懂光源相机怎么搭，只需给出一个包围盒中心；渲染器内部做 `lookAt + ortho`。光源方向接近 ±Y 时换一个 up 轴，避免 lookAt 退化。

这是一个 trade-off：灵活性不如直接传矩阵，但符合 a002 里“引擎侧不需要理解渲染器内部细节”的约束。以后真的需要自定义投影（例如 CSM 的分片矩阵），再在 schema 里加口子。

## 阴影采样

顶点着色器额外把世界坐标变换到光源裁剪空间并插值，片元里做透视除法、映射到 [0,1]。三个细节：

1. **normal bias**：沿法线偏移世界坐标，主要解决斜面上的 acne。
2. **depth bias**：按 `1 - N·L` 缩放，掠射角下需要更大偏移。
3. **3x3 PCF**：用 `sampler2DShadow` 的硬件比较，九次采样后平均，边缘不会太硬。

shadow pass 这边没有开 front-face culling，只用了 polygon offset。原因是这一版会往 shadow map 里画单面的地面，按“只画背面”的常规做法会让地面从光源视角消失；等场景里的网格都保证封闭之后，再考虑切回剔除正面来进一步压 acne。

采样坐标落在 [0,1] 之外时直接返回无阴影，避免贴图边界的重复采样把阴影“拖”出去。

## 验证

`model_viewer` 加了一块程序化生成的地面，相机从上方俯视。旋转 bunny 时可以看到投影随之变化。为了不靠肉眼看，我临时写了一个 probe：同一帧分别用 `shadow_.enabled_ = true/false` 渲染到 256x256 的 RenderTarget，读回像素对比。结果是有约 2.3% 的像素发生变化，阴影区域亮度明显低于无阴影版本，变化区域落在模型下方、贴图范围内。probe 验证完就删掉了，不留在仓库里。

## 遗留

- 正交盒是固定 extent，物体超出范围会丢阴影，后面要做视锥拟合。
- 没有阴影剔除，shadow pass 会画所有 item。
- 没有 CSM，大场景下精度会不够。
- PCF 核是硬编码的，之后可以提成参数或换 PCSS。
