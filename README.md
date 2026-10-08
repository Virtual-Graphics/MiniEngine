# MiniEngine

MiniEngine 如名字一样它是一个非常 Mini 的游戏引擎，但相比作者的第一代游戏引擎 [Fermion](https://github.com/Yang-Junjie/Fermion.git) 体量会
大很多而且功能会更完善系统更 Robust。除此之外作者还有一个名为 [FinalEngine](https://github.com/Yang-Junjie/FinalEngine.git) 的与 MiniEngine 
同架构大致相同的游戏引擎，不过 FinalEngine 与 MiniEngine 不同的是其中的 Graphics Api 使用的是 Vulkan 并且有 AI 参与，而 MiniEngine 所有的
代码将全部手写，并且使用 OpenGL 4.6 以及其中的大部分系统都会比 FinalEngine 轻量化很多。

## 为什么开发一个新引擎
说到这，我为什么又要开发一个游戏引擎，开发一个我心目中理想的游戏引擎一直都是我追随的目标，我认为我很大情况下是一个理想主义者，对于功利性的
事我有点不感兴趣，即使它能给我带来财富，好了不扯远了，到目前为止加上现在这两个我已经开发 6 个完成度各不相同的引擎，它们的架构都不一样而它们
的架构目标的都是向着我理想中游戏引擎的架构出发的，而我目前发现我自己的能力完全不足以实现这个目标，也许配上顶级 AI 我能做到，但是我自己将
看不懂 AI 设计的各种复杂系统，所以我打算从自己熟悉的架构出发（也就是 Hazel）再以这些日子我取得的进步再重新写一个非我理想架构的游戏引擎，
这次应该能做得更好，也是为了回应之前在 Fermion 视频中提到的：“也许未来我还会开发一个新的更完整的游戏引擎”。

## 为什么存在 Final & Mini 两个版本
我在这里说实话吧，因为我认为现在的面试评价体系在被 AI 的影响下已经和原来很不一样了，在我看来现在的还使用着手写时代的项目作为项目经验完全
约等于没有经验，现在应该需要的是在 AI 的配合下开发出一个具有重大影响、重大突破、功能非常完整的可使用可部署甚至可商用的项目，而 FinalEngine
就是冲着这个目标去的。而 MiniEnigne 也就是本项目将作为我个人的长期性的游戏引擎存在，我非常喜欢那种一把一把把一个软件拉扯大的感觉，这会让你
对这个软件产生感情，对我非常喜欢这种感觉，AI 写出来的东西我几乎没有任何感觉。

## MiniEneing 的工作形式
现在初步基本由我一个人完成对 MiniEngine 的全部工作，并且不支持任何人添加 Pull Request ，如果你想指出一些问题应该直接联系作者，至于联系方式
嘛，既然你都能看到这个仓库了，还会没有作者的联系方式？作者将会在业余时间开发，哈哈，其实作者更本没有工作，大三在读学生，应该说作者所有的
时间都是业余时间，但是作者除了 MiniEninge 要开发外，还有更多的其他的事情要做，作者只会在觉得这个时间段是开发 MiniEngine 合适的时间段进行
开发。并且将在 docs/ 下写下这次开发的 devlog，也许这些 devlog 可以作为你学习 MiniEngine 的历史的重要一手资料。并且 Artichoke 也会在 docs/
下整理系统架构，当然这也许会不完整，但最终 MiniEnigne 应该需要一个官网展示关于 MiniEngine 的所有信息。

## MiniEninge v0.0.5 版本之前应该实现的功能
我不想给 v0.0.5 版本添加太多功能，因为我想快点达到 v0.0.5 这个目标，哈哈，反正又不商用，快点达到一个里程碑也是对自己的及时反馈。
### 渲染
[] forward pipeline  
[] Blinn-Phong Shading  
[] Directional Light  
[] separate 2d pipeline  


### 场景 
[] ECS using Entt  
[] Scene Graph 先不支持 hierarchy  
[] Scene serialize & deserizalize  


### 资产
没有完整的 AssetPipeline 直接从 disk 中加载，没有缓存，最简单的形式。  
[] obj   
[] image   

嗯对先这样吧。
