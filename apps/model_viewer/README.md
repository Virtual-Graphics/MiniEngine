# Model Viewer

从命令行传入一个 OBJ 模型路径，相对路径以启动时的工作目录为基准：

```powershell
.\build\bin\model_viewer.exe .\assets\StanfordBunny.obj
```

缺少参数或模型加载失败时，以非零状态退出。当前保留示例的相机、模型变换和光照设置。
地面由示例直接创建和释放 GPU 网格，不注册为文件资源。
