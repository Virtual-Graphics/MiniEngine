# Contributing for VulcalEngine

## Naming Conventions
- 所有的 文件 和 目录 都应该采用小写加下滑线命名即 **Snake Case**，特殊文件除外如 `README.md`、`CMakeLists.txt`。  
- 代码中的 `class`、`struct`、`enum` 等自定义类型采用首字母大写其他小写的 **大驼峰命名法**/ **PascalCase** 。  
- 代码中所有的函数都采用 **Snake Case**，所有的成员变量都采用 **Snake Case** 除此之外需要在末尾多添加一个 `_`。  
- 对于专有名词或缩写应该全部采用全大写命名，如 `AABB`、`UUID` 等等。  
- 灵活的使用合适的命名方法，在特殊位置可以不遵守上述规则。  

## Commit 规范
使用标准的 [Angular 规范](https://github.com/angular/angular/blob/main/contributing-docs/commit-message-guidelines.md)，
请确保每一个 Commit 只代表一个意图即尽可能保证单个 Commit 的原子性，并清晰地描述其目的。  


## 代码注释 
应该使用简单易懂且**简短**的英文描述。
