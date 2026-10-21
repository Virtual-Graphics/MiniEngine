# VulcalEngine

[![Windows CI](https://github.com/Virtual-Graphics/MiniEngine/actions/workflows/ci-windows.yml/badge.svg?branch=main)](https://github.com/Virtual-Graphics/MiniEngine/actions/workflows/ci-windows.yml?query=branch%3Amain)
[![Linux CI](https://github.com/Virtual-Graphics/MiniEngine/actions/workflows/ci-linux.yml/badge.svg?branch=main)](https://github.com/Virtual-Graphics/MiniEngine/actions/workflows/ci-linux.yml?query=branch%3Amain)

VulcalEngine 如名字一样它是一个非常 Mini 的游戏引擎，但相比作者的第一代游戏引擎 [Fermion](https://github.com/Yang-Junjie/Fermion.git) 体量会大很多而且功能会更完善系统更 Robust 。除此之外作者还有一个名为 [FinalEngine](https://github.com/Yang-Junjie/FinalEngine.git) 的与 VulcalEngine 的架构大致相同的游戏引擎，不过 FinalEngine 与 VulcalEngine 不同的是其中的 Graphics Api 使用的是 Vulkan 并且有 AI 参与（大部分是 vibe coding），而 VulcalEngine 将采用非 vibe coding 的形式，并且使用 OpenGL 4.6 以及其中的大部分系统都会比 FinalEngine 轻量化很多。

## 为什么开发一个新引擎
说到这，我为什么又要开发一个游戏引擎，开发一个我心目中理想的游戏引擎一直都是我追随的目标，我认为我很大情况下是一个理想主义者，对于功利性的事我有点不感兴趣，即使它能给我带来财富，好了不扯远了，到目前为止加上现在这两个我已经开发 6 个完成度各不相同的引擎，它们的架构都不一样而它们的架构目标的都是向着我理想中游戏引擎的架构出发的，而我目前发现我自己的能力完全不足以实现这个目标，也许配上顶级 AI 我能做到，但是我自己将看不懂 AI 设计的各种复杂系统，所以我打算从自己熟悉的架构出发（也就是 [Hazel](https://github.com/TheCherno/Hazel)）再以这些日子我取得的进步再重新写一个非我理想架构的游戏引擎，这次应该能做得更好，也是为了回应之前在 [Fermion 视频](https://www.bilibili.com/video/BV13oAUzJEis/?spm_id_from=333.1387.homepage.video_card.click&vd_source=b60fadc84379e52fc7b933fab5a8751c)中提到的：“也许未来我还会开发一个新的更完整的游戏引擎”。

## 为什么存在 Final & Vulcal 两个版本
我在这里说实话吧，因为我认为现在的面试评价体系在被 AI 的影响下已经和原来很不一样了，在我看来现在的还使用着手写时代的项目作为项目经验完全
约等于没有经验，现在应该需要的是在 AI 的配合下开发出一个具有重大影响、重大突破、功能非常完整的可使用可部署甚至可商用的项目，而 FinalEngine 就是冲着这个目标去的。而 VulcalEngine 也就是本项目将作为我个人的长期性的游戏引擎存在，我非常喜欢那种一把一把把一个软件拉扯大的感觉，这会让你对这个软件产生感情，对我非常喜欢这种感觉，AI 写出来的东西我几乎没有任何感觉。

## 其他
现在初步基本由我一个人完成对 VulcalEngine 的全部工作，并且不支持任何人添加 Pull Request ，如果你想指出一些问题应该直接联系作者，至于联系方式嘛，既然你都能看到这个仓库了，还会没有作者的联系方式？作者将会在业余时间开发，哈哈，其实作者更本没有工作，大三在读学生，应该说作者所有的时间都是业余时间，但是作者除了 VulcalEngine 要开发外，还有更多的其他的事情要做，作者只会在觉得这个时间段是开发 VulcalEngine 合适的时间段进行开发。并且将在 docs/ 下写下这次开发的 [devlog](https://codeberg.org/Akiyama_Artichoke/MiniEngine/src/branch/main/docs/devlog)，也许这些 devlog 可以作为你学习 VulcalEngine 的历史的重要一手资料。并且 Artichoke 也会在 docs/ 下整理系统架构，当然这也许会不完整，但最终 VulcalEngine 应该需要一个官网展示关于 VulcalEngine 的所有信息。

说点题外话，关于我对好的软件产品的定位，现在无可否认的是 AI 的编码能力比人类高出的不是几个数量级，无论是速度还是其他方面对人类都是降为打击。我个人也是 AI Coding 的重度依赖者，虽然我向往那种所有代码都是人类写的，慢慢推进的开发模式，但是的确 AI 的速度，小范围代码质量确实带给了我很多便利，但是不可否认的，如果自己对项目一点都不管控，直接写一个 Prompt 约束项目然后全部交给 AI ，无论 AI 的上下文有多大，无论采用了什么本地存储记忆方式，文档记录方式，随着项目越来越大，AI 会在项目中添加各种没必要的复杂度 （拉屎），使得项目的系统一致性急剧下降，AI 可能没有类似于人类脑子里的一种“全局约束函数”东西，并且如果你切换其他 AI 工作这种现象会越发明显（我没 Token 了我要换 AI），约束靠 prompt 传递会衰减，所以项目越大、越偏离初始 prompt，它越倾向于"就地加一层"而不是回头重构。对此我在这个项目中使用 AI 的工作流是自己完全写好架构文档，AI 完全按照我写的架构方案去实现，并且尽可能 Review 掉 AI 所有的代码，慢一点没事，此外 AI 可以无限制的对代码进行 Review，但是修改方案必须是我给的，或者是 AI 给方案我采纳，如果我要实现什么功能那么我一定得知道它是怎么实现得并且设计好 API ，AI 具体是怎么实现的可以不考虑（但我会 Review），我们要做的是工艺品产品，而不是流水线产品，我们要具有工匠精神，而是不是各种为了赚钱而快速开发出一款屎来。

## ABOUT
采用 MIT 开源证书，本引擎能随意修改并作为商用，并且引擎开发的游戏也能随意商用，提供最高的自由度。

[Roadmap](https://codeberg.org/Akiyama_Artichoke/MiniEngine/projects/71874) 查看项目未来的规划。  
[Github](https://github.com/Virtual-Graphics/MiniEngine) 仓库。  
[Codeberg](https://codeberg.org/Akiyama_Artichoke/MiniEngine) 仓库。

