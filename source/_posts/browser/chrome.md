---
title: 现代浏览器的内部构造
date: 2022-04-07 17:36:28
tags: browser
description: 现代浏览器的背后实现, 关于 Chrome 中从URL的页面渲染完成的具体过程...
---

### 有言在先
本篇文章，大部分为译文，主要来自google chrome 发布的浏览器实现架构。之前看过几次，想着尝试翻译一下。

### CPU
Central Processing Unit - or CPU. 中央处理器。

### GPU
Graphics Processing Unit - or GPU. 图形处理器。不同于 CPU， GPU 主要擅长同时跨多核同时处理简单任务。就像GPU名字一样，GPU被开发出来就是用来处理图像的。
这也是为什么在图像上下文中"使用GPU"或者 "GPU支持" 会和 渲染速度和交互流畅度相关联。随着 GPU 加速计算，越来越多的计算在 GPU 单独计算上成为可能。

当你打开一个应用在电脑或者手机上，CPU和GPU开始提供动力。一般来说，应用使用GPU还是CPU是通过操作系统来决定的。

### 使用进程和线程执行代码

进程可以描述为应用程序的执行程序。线程是存在于进程内部并执行其进程程序的任何部分的线程。
当你启动应用，一个进程被创建。代码可能会创建多个线程帮助你完成工作，但是这是可选的。操作系统给进程提供了一块内存区域进行工作，并且所有的应用状态都保存在这块私有区域内。
当你关闭应用，进程会被处理掉，并且操作系统会释放内存。

{% asset_image process-memory.svg %}

一个进程可以要求操作系统启动另外一个进程去执行不同的任务。当这件事情发生时，不同的部分的内存将会分配给不同的进程。
如果两个进程需要进行通信，那么可以使用(IPC)Inter Process Communication 内部进程通信。许多应用使用这种方式进行设计，所以当其中一个worker进程没有响应时，可以在不停止应用的情况下重新进程。

### 浏览器架构
所以web浏览器架构是使用进程还是线程呢？可以是一个进程带多个不同的线程，也可以是多个不同的带有多个线程的进程通过IPC进行通信。

{% asset_image web-browser-process.png %}

注意不同的架构代表不同的实现细节。这里没有标准规范关于如何实现浏览器架构。一种浏览器的实现方式可能和另一种完全不一样。

以下为关于chrome 浏览器的最新架构。

最上层是 浏览器进程, 用于协调关注浏览器的其他部的不同进程。
对于渲染器进程，会创建多个进程并将其分配给每个选项卡。直到最近，Chrome 还在可能的情况下为每个选项卡提供了一个进程；现在它尝试为每个站点提供自己的进程，包括 iframe。

{% asset_image multipal-process-chrome.png %}

### 进程控制

下面描述了浏览器的进程对应的控制类型:

Browser:  控制chrome 应用，包括 地址条，书签，返回和前进按钮
Renderer: 控制浏览器显示的tab的任何事情
Plugin: 控制浏览器使用的任何插件，比如flash
GPU: 与其他进程隔离处理 GPU 任务。它被分成不同的进程，因为 GPU 处理来自多个应用程序的请求并将它们绘制在同一个表面上。


可能会有更多的进程比如 Extension进程或者 utility processes. 你可以从 chrome的任务管理器中看到具体的进程信息以及内存使用情况。

### 多进程架构的好处

大多数情况下， 都可以认为每个tab都有一个自己的 render process。加入你有3个tab页面，运行在各自独立的render  process上面。
如果突然其中一个变得无响应，然后你可以关闭那个无响应的页面，然后关注其他处于活动状态的tab.如果所有的tab都运行在同一个process中，那么一旦其中一个页面挂了，浏览器也没了。

另外一个分离浏览器工作为多进程的好处就是安全和沙箱机制。由于操作系统提供了一种限制进程权限的方法，因此浏览器可以对某些进程的某些功能进行沙箱处理。

由于进程有自己的私有的内存空间，他们进程包括通用的基础设施（比如V8引擎）。这就表示了更多的内存开销因为它们不能像在使用线程一种进行共享内存。
为了节省内存，Chrome 限制了它可以启动的进程数。限制取决于您的设备有多少内存和 CPU 能力，但是当 Chrome 达到限制时，它会开始在一个进程中运行来自同一站点的多个选项卡。

### 节省内存-Chrome服务

Chrome目前开始将不同的部分整合成不同的服务然后在单独的进程中运行。当浏览器运行在足够好的硬件上时，它可能将每个服务分割成不同的进程用来提供稳定性。当时如果在一个资源不够的情况下，chrome可能整合服务放在同一个进程中用来节省内存。

### 单帧渲染进程-站点隔离
站点隔离是一个最近才被提出的Chrome特性，它为每个 跨站点的 iframe 运行一个隔离的 渲染进程.我们一直在讨论每个选项卡模型一个渲染器进程，它允许跨站点 iframe 在单个渲染器进程中运行，并在不同站点之间共享内存空间。运行 a.com 和b.com 在一个相同的render process 似乎没问题。同源策略是浏览器安全的核心模块。它保证一个网站不能访问数据从另外一个网站在没有同意的情况下。
绕过此策略是安全攻击的主要目标。进程隔离是一个有效的方式去隔离站点。借助 Meltdown 和 Spectre，我们需要使用进程来隔离站点变得更加明显。自 Chrome 67 起，默认情况下在桌面上启用站点隔离，选项卡中的每个跨站点 iframe 都会获得一个单独的渲染器进程。

{% asset_image process-seperate.png %}

启用站点隔离是一项多年的工程努力。站点隔离不只是注入不同的渲染进程那么简单。它从根本上改变了iframe的通信方式。在不同进程上运行 iframe 的页面上打开 devtools 意味着 devtools 必须实施幕后工作以使其看起来无缝。即使运行一个简单的Ctrl+F 去寻找一个单词在页面上也也意味着搜索要跨越不同的渲染进程。这就是为什么 站点隔离是一个重要的里程碑的原因。

### 从 browser process 开始
如上所说，任何选项卡之外的所有内容都由浏览器进程处理。browser进程有像 UI 线程（绘画浏览器的 button input 框），Network线程（处理从网络中结束数据的堆栈），storage 线程（控制处理文件的权限）等线程。当你输入一个URL 在浏览器中，那么你的输入将会被浏览器的UI线程进行处理。

### 简单的导航

当一个用户开始输入一个 Url地址栏，那么最开始 UI thread 开始询问，这是一个 搜索还是一个 URL？在 chrome，地址栏也是一个输入栏，所以UI thread 需要解析和决定是讲你的内容发送给搜索引擎还是进入你要访问的站点。

### 开始导航
当用户输入 enter, UI thread线程开始一个 network call 去获取内容。加载图标开始旋转在tab中，然后 network线程开始通过对应的协议（例如 DNS搜索） 为请求建立 TLS 连接。

注意，network thread 可能会收到一个服务端的响应，例如 301。在这种情况下，network thread 和 ui thread 进行通信，这个服务端需要请求跳转。然后另外一个url会重新被初始化。

### 读取请求
一旦响应体开始返回，那么network thread会在必要时查看 最前部的几个字节。 头部的 content-type 会提示数据类型，但是也可能有误差，MIME Type sniffing 在这里被使用。这是一个棘手的事情在chrome源代码中。

如果返回的数据是HTML文件，下一步讲你数据给到render process进行解析，但是如果是一个zip 文件或者其他文件，那么就表示是一个下载请求，需要将数据给到 下载管理器进行解析。

当然这里 安全检查也会同时进行，如果域名或者请求数据是一个危险网站，那么network thread 会提示显示一个危险网站。同时 Cross Origin Read Blocking(CORB)检查也会同时进行，确保敏感的跨站数据不会进入到渲染进程。

### 寻找 render process

一旦所有的检查已经结束，并且 netword thread 确认浏览器可以导航到对应的请求站点，此时 network 线程会告诉 UI 线程数据已经 准备好了。UI 线程开始寻找一个 render Process 去准备渲染浏览器页面。

因为netword 请求需要花费几百毫秒去获取请求返回，一个用于加速的优化被应用。当UI thread 发送 URL 给到 network thead, 它已经知道将要导航到哪一个站点。UI 线程尝试主动查找或启动与网络请求并行的render process。因此，如果一切顺利，当network thread 返回数据是，render process 已经完全准备好了。如果导航重定向跨站点，则可能不会使用此备用进程，在这种情况下，可能需要不同的进程。

### Commit navigation
现在数据和render process 已经准备好了，而且 IPC 通信已经从 browser process 发送到 render process用于 commit navigation. 它也接收 数据流，因此 render process 可以持续接收 HTML 数据。一旦 browser process 得知 commit已经在 render process中发生，那么 navigation 操作完成，同时 document 开始进入 `loading 阶段`。

此时，地址栏更新，安全指示器和站点设置 UI 反映了新页面的站点信息。该选项卡的session历史记录将被更新，因此后退/前进按钮将逐步浏览刚刚导航到的站点.为了便于在关闭选项卡或窗口时恢复选项卡/会话，会话历史记录存储在磁盘上.

### 初始化load 完成

一旦 navigation 被commited , render process 携带着 加载的资源开始渲染页面。当render process `finishes` render， 它将会发送一个 IPC 给browser render( 这个操作在页面上所有的frame的 onload 事件触发并执行完成之后)。同时，UI thread 挺尸加载图标在tab上。

之所以说 'finishes' ，因为 客户端 javascript 可以继续 load 额外的资源并且渲染新的视图在这之后。
(这里应该是指 javascript 额外继续加载js, 如果是script 标签情况，那么会在render process阻塞渲染并 IPC 到 browser 中的 network thread 中继续加载。)

### 导航到不同的站点
根据上面内容导航到简单的网站已经完成。但是如果用户输入了不同的URL地址呢？ browser process 会通过相同的步骤导航的不同的站点。但是在导航之前，它需要进行检测当前渲染的站点如果它有关注 `beforeunload` 事件。

`beforeunload` 事件可以创建一个疑问 `Leave this site`? 提醒如果你尝试导航离开或者关闭tab. 任何事情在tab内部包括你的js代码都由render process处理, 因此 browser process 必须检测你当前的render process 当新的导航请求进来的时候。 

tip: 不要添加额外没用的 `beforeunload`事件，它将会在增加额外的消耗，应为它在跳转之前必须要进行检测。除非在必要的时候进行检测。

当这个导航跳转被初始化在 render process 中（例如你点击一个link或者客户端js 执行了 window.loacation="https://xxx.com")，render process 首先检测 `beforeunload` 事件。然后通过相同的 流程作为 browser process 初始化跳转。唯一的区别是导航请求是从渲染器进程启动到浏览器进程的。

当新的跳转需要跳转到不同的页面，一个新的隔离的render process 将会被用来处理新的跳转请求，然而就的render process 会用来继续处理一些事件，例如`unload`。

### Service Worker
Service Worker 是一种写 网络代理的方式在你的代码中。允许 Web 开发人员更好地控制本地缓存的内容以及何时从网络获取新数据。如果 service woker 被设置从缓存中加载页面，那么将不用从网络中发送请求获取数据。

service worker 是运行在render process 中的 javascript 代码。但是当跳转请求进入时，browser process 是怎么知道这个站点有一个 service worker呢？

当 service woker 被注册，那么 service woker 的作用域(scope) 将会被保存在一个引用(reference). 当跳转发生，network thread 检查domain针对 注册的service worker scope。
如果一个 service worker 被注册到URL，那么 UI thread 找到一个 render process 用来处理 service worker 代码。service worker 可能加载数据从cache中，也可能从网络中从新加载新的数据。

### 预加载
你可以看到完整的过程在 browser process 和 render process中可能会存在延迟，如果service worker最终需要从网络中获取数据。Navigation Preload 是一种提升处理速度机制通过加载资源并行启动service worker。它通过请求头标记这些请求，允许服务端决定发送不同的数据给不同的请求，例如，仅仅是更新数据而不是更新文档。

### render process 的内部机制
render Process 是对于 web性能的许多方面内容。如果希望继续深入研究可以关注 [the Performance section of Web Fundamentals](https://developers.google.com/web/fundamentals/performance/why-performance-matters/)

### render process 处理web内容

render process 为在tab内的发生的所有事情负责。在render process, `main thread` 处理大多数的代码。 有时候你的部分代码被workder thread 处理如果你使用了 `web worker`或者 `service worker`.
`compositor` 和`raster` thread 也运行在render process的内部，让页面渲染更加的流程和高效。

render process 的核心工作是转换 HTML, CSS 以及 javascript 成为用户可以交互的网页。

### Parsing
#### 解析 DOM
当 render process 接收一个 commit 信息从 navigation 并且开始接收 html 数据， main thread 开始 parse text string(HTML) 并且开始 将它转换为一个 Document Object Model(DOM).

DOM 是浏览器对页面的内部表示，也是 Web 开发人员可以通过 JavaScript 与之交互的数据结构和 API。
解析一个 HTML document 成为 DOM 被定义为HTML Standard. 您可能已经注意到，将 HTML 提供给浏览器永远不会引发错误。这是因为 HTML 规范旨在优雅地处理这些错误。

#### 子元素加载

网站竟然使用外部的资源如 images css 和 javascript. 这些文件需要被加载从network或者 cache. main thread 在解析 document成为DOM时发现这些资源，将会请求他们`一个接一个(one by one)`。但是为了提高速度，'preload scanner' 同步运行。比如 <img>或者 <link> 在html 页面上， preload scanner 查看了 通过html生成的token，并发送请求给 broswer process中的network thread。

{% asset_image sub-source-load.png %}

#### JS 会阻塞解析

当HTML 解析 发现 script标签，它将会暂停解析 HTML 文档，并且必须加载，解析，执行 javascript load. why ? 因为 js 代码可以change 文档的内容（比如通过document.write修改整个Dom树结构)。
这就是为什么 HTML 解析器必须等待 js代码执行才能恢复对 DOM文档的解析。 

#### 浏览器如何加载资源
Web 开发人员可以通过多种方式向浏览器发送提示，以便很好地加载资源。如果JS代码没用使用doument.wirte(), 你可以在 script标签中添加 `async` 或者 `defer` 属性。
浏览器在之后 异步地加载和执行 JS代码，并且不会阻塞 解析。你也可以使用 Javascript Module 如果支持的话。
<link rel="preload"> 也是一种方式通知浏览器这个资源被定义为本次跳转需要，并且你希望尽快download下来。

#### 样式计算
只有一个 DOM 并不能完全知道一个页面将会是什么样子，因为我们可以定义页面的样式通过CSS。main thread 通过解析 CSS 且 为每一个DOM节点探测 计算后的样式。关于哪种样式应用到哪一个节点是基于CSS的选择器。你可以通过 DevTools中的 computed属性查看信息。

{% asset_image style-computed.png %}

即使你没有提供任何 CSS ，每个DOM 依旧有一个 Computed style. \<h1\> 比 \<h2\> 显示要更大。因为浏览器有默认的 style 脚本。

#### Layout 布局
现在 render process 知道了 文档的结构以及 每个节点的样式，但是这还是不足以渲染页面。
想象一下你尝试描述一个画面给你的朋友通过一个手机。"这里有一个大的红色圆圈和一个蓝色的方块" 并不足以让你你的朋友完全精确的画出你要的东西。

Layout 是一个确定元素几何位置的过程。main thread 遍历 DOM 并计算样式，然后创建带有(像x, y)坐标以及边界盒子大小信息的 `layout tree`。
Layout tree 和 DOM tree 有一定的相似性，但它只包含了页面上显示的元素的信息。
如果display:none 被设置，那么该节点将不在layout tree 上面。但是如果 visibility:hidden, 将会出现在 layout tree中。
类似地，如果应用了具有类似 p::before{content:"Hi!"} 内容的伪类，它会包含在layout tree中，即使它不在 DOM 中.

{% asset_image layout-tree.png %}

决定页面的layout是一个挑战的任务。即使是最简单的页面布局，比如 从头到底都是 block 流也必须考虑文字的大小以及哪里进行换行，因为这些有可能影响到 段落的大小和形状，然后影响后面需要进行layout的段落。

CSS 可以让元素浮动到一侧，屏蔽溢出项，改变书写方向。你可以想象，layout 阶段是一个强大的过程。

#### Paint
只是有 DOM, style, layout，依然还是不能完成的渲染页面。
还是上面的例子，现在准备画一幅画。你知道里大小，形状，以及元素位置，但是你依旧必须知道绘画的顺序。

例如，`z-index` 可能会被设置到某些元素上，在这种情况下如果按元素在DOM上顺序进行渲染，将会得到一个错误的渲染结果。

在 paint 阶段，main thread 遍历 layout tree并且创建 paint 记录。 绘画记录是“先背景，后文字，后矩形”的绘画过程记录。
如果您使用 JavaScript 在 \<canvas\> 元素上绘图，您可能对这个过程很熟悉。

{% asset_image paint-record.png %}

#### 更新渲染管道成本高昂
在渲染管道中要掌握的最重要的事情是，在每一步中，之前操作的结果都用于创建新数据。
如果某些东西修改了layout tree,然后对于受到影响的DOM文档绘画顺序需要重新生成。

如果您正在为元素设置动画，浏览器必须在每一帧之间运行这些操作。我们的大多数显示器每秒刷新屏幕 60 次 (60 fps)；当你移动东西时每一帧都显示在屏幕上，动画对人眼来说会很流畅。但是，如果动画错过了中间的帧，那么页面将出现“janky”。

{% asset_image frame-miss.png %}

即使你的渲染操作跟上屏幕刷新，这些计算在主线程上运行，这意味着当你的应用程序运行 JavaScript 时它可能会被阻塞。

{% asset_image js-block-frame.png %}

你可以分离你的js代码成为一些小的片段，并且使用`requestAnimationFrame()`分派到每一帧去执行。

{% asset_image seperate-js-chunk.png %}

### Compositing 组合
#### 如何画一个页面
现在浏览器知道文档的结构，元素的样式，页面的几何属性，以及绘画的顺序，那么怎么去绘画呢？
转换这些信息为像素点到屏幕上的过程，就叫做rasterizing（删格化）

也许处理这个问题的一种天真的方法是在视口内栅格化部分。如果用户滚动页面，然后移动删格化的框架，接下来将会通过删格更多来填补丢失的部分。这就是 Chrome 在首次发布时处理光栅化的方式。然而，现代浏览器运行一个更复杂的过程，称为合成。

#### 组合
Compositing是一个将页面一些部分拆分到不同的层(layer)中，分别删格化他们，然后组合成为了一个页面在一个叫做 compositor thread的单独隔离的线程中。
如果滚动发生，由于layer已经被删格化过了，那么所有剩下来要做的事情就是组合成一个新的 frame(帧)。动画能够被实现也是用相同的方式，通过移动layer以及组合新的 frame.

#### 切分 layers

为了找出哪些元素需要被放到layer中，main thread 遍历 layout tree 创建layer tree（这部分被叫做 'Update Layer Tree' 在 devTools的 performance panel中）。
如果页面某些确定的部分应该被单独分成layer（例如 滑入侧菜单）但是却没有被分入layer, 你可以标记给浏览器通过使用 `will-change` 属性在css中。

{% asset_image divide-layer.png %}

你可以尝试去给每一个元素一个layer，但是compositing 在多个layer可能会导致更慢相比删格页面中每帧的一些小的部分。所以测量应用程序的渲染性能是非常重要的。


#### 主线程的光栅和合成
一旦 layer tree 被创建以及 绘制顺序被确定，那么main thread 提交这些信息给到 compositor thread.
compositor thread 然后删格每个layer. layer可以像一个完整页面一样很大，所以 compositor thread 切分他们成小块，并发送每个小块到 raster thread。
Raster thread 删格每个小块然后存储他们到 `GPU` 内存中。

{% asset_image compositor-raster-process.png %}

compositor thread 会优先选择不同的 raster thread, 因此在视窗内或附近的事物会被优先删格。一个layer可能会有多个删格块为了不同的方案例如需要被放大的情况。
一旦瓦片被光栅化，合成器线程通过合成 draw quads 收集瓦片信息并创建一个 compositor frame 帧.

> Draw quads: 包含瓦片方块在内存中的位置信息以及在页面合成的情况下方块在页面中哪个地方被绘制的信息。
> Compositor frame: 代表页面的一个 frame 信息的 Draw quads 集合。

一个 compositor frame 然后被提交给 broswer process 通过  IPC 的方式。此时，可以从 UI 线程添加另一个合成器框架以更改浏览器 UI 或从其他渲染器进程添加以进行扩展.
这些 compositor frame 被发送给 GPU 用于显示在屏幕上。如果 滚动事件发生，合成器线程 会创建一个新的 compositor frame 然后发送给 GPU.

{% asset_image compositor-frame.png %}

使用合成器的好处是完成这些工作不用涉及 main thread. 合成器线程不需要等待 样式计算以及javascript执行。这就是为什么 compositing onluy animations 被认为是最高效流畅的方式。
如果 layout 或者 paint 需要被重新重算，那么main thread 将需要被涉及参与进来。

### 浏览器输入事件

当用户的动作类似 触碰屏幕，浏览器是最先接收到动作。然后，browser process 只知道动作发生的的位置因为tab内部的内容都是在render process中发生的。因此browser process 发送事件类型(例如 touchstart) 和 事件发生的坐标给到render process。render procss 适当的时候接收事件通过发现event target，并执行绑定的事件监听函数。

{% asset_image process-browser-event.png %}

### 合成器接收输入事件

上面说 合成器会流畅的处理滚动事件通过组合删格化layer. 如果没有附加的输入事件监听函数绑定到页面，compositor thread 可以创建一个新的 compositor frame 完全独立于 Main thread.但是如果有一些监听函数绑定到page呢？合成器线程是怎么发现事件是否需要被执行呢？


### 非快速滚动区域

因为执行 javascript 是 main thread 的工作，当页面被合成，compositor thread 标记了页面中的一个区域表示没有事件绑定，这个区域就叫做非快速滚动区域。
通过这些信息，合成器线程可以确定发送输入事件给到main thread 如果事件发生在这个区域。如果事件不在这个区域，那么合成器线程就会创建一个新的 frame，并不用等待main thread.


### 事件回调注意事项
一个共同的事件回调处理模式在web开发中就是事件委托。由于事件冒泡，您可以在最顶层元素附加一个事件处理程序并根据事件目标委派任务。
``` js
document.body.addEventListener('touchstart', event => {
  if (event.target === area) {
      event.preventDefault();
  }
});
```

由于仅仅需要写一个 事件处理函数对于所有的元素，所以很吸引人。然而，如果你从浏览器的角度去看这个问题，现在，整个页面都被标记为 非快速滚动区域。
这意味着即使您的应用程序不关心来自页面某些部分的输入，那么合成器线程必须去和主线沟通，并等待主线程返回每次有事件进来的时候。合成器的平滑滚动能力就没有什么作用了。

为了缓和这个问题，你可以使用 `passive: true` 选项在你的事件回调方法。这个选项告诉浏览器你仍然希望监听这个事件在 main thread，但是合成器线程可以继续合称新的frame.

``` js
document.body.addEventListener('touchstart', event => {
  if (event.target === area) {
    event.preventDefault()
  }
}, {passive: true});
```
### 检查事件是否可取消

想象你有一个box在页面上，你希望限制滚动方向，只能横向滚动。

使用 `passive: true` 选项表示页面滚动可以更加的流畅，但是垂直滚动可能已经开始了在你希望使用 `preventDefault` 方法去显示滚动方向。 你可以检测通过使用 `event.cancelable`;
```js
document.body.addEventListener('pointermove', event => {
  if (event.cancelable) {
    event.preventDefault(); // block the native scroll
    /*
    *  do what you want the application to do here
    */
  }
}, {passive: true});
```

额外的，你可以用css rule 例如 `touch-action` 完全的排除事件回调。
``` css
#area {
  touch-action: pan-x;
}
```

### 发现event target

当合成器线程 发送input event 给main thread, 第一件事情就是执行一个hit test用来寻找 event target。 hit test 使用在渲染过程中生成的 paint record data，然后发现最接近事件发生坐标位置的元素。

### 最小化事件分派到主线程
上面说到 传统的显示刷新频率为 60次每秒，以及我们怎么样保持这个节奏使我们的动画更流畅。
对于input事件，通用的屏幕设备每秒直接60-120次触发事件，对于通用的鼠标设备，每秒可以100次触发事件。input event 有以一个更高的触发频率相对于屏幕刷新。

如果一个持续的touchmove事件，被发送给 main thread 以每秒120次的方式，与屏幕刷新的速度相比，它可能会触发过多的命中测试和 JavaScript 执行。


{% asset_image excessive-event.png %}

尽量减少对主线程的过多调用，Chrome 合并连续事件（例如 wheel、mousewheel、mousemove、pointermove、touchmove ）并将调度延迟到下一个 requestAnimationFrame 之前。


{% asset_image minimize-excessive.png %}

任何离散的事件（keydown, keyup,mouseup,movsedown, touchstart)以及 touchend 都是直接触发的。

### 使用 getCoalescedEvents 获取帧内事件

大多数的 web 应用，合成已经可以提供不错的体验。但是，如果你想构建类似拖动应用或者基于touchemove坐标画一个路径，你可能都是中间坐标在画一个流畅的路线的时候。、
在这种情况下，您可以在指针事件中使用 getCoalescedEvents 方法来获取有关这些合并事件的信息。

``` js
window.addEventListener('pointermove', event => {
  const events = event.getCoalescedEvents();
  for (let event of events) {
    const x = event.pageX;
    const y = event.pageY;
    // draw a line using x and y coordinates.
  }
});
```
