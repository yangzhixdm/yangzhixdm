---
title: 关于addEventListener绑定事件的一些细节
date: 2016-11-18 15:50:37
tags:
description: 关于addEventListener绑定事件的一些细节，主要针对其中的一些额外参数的理解。
---
最近在读 IScroll 源码，发现其中的_initEvent 事件方法有点奇怪。如下：

![这里写图片描述](/postimg/20161118152848077.png)

其中的 utils.addEvent 方法如下：
![这里写图片描述](/postimg/20161118152921077.png)

这里出现的问题是：
之前我们使用addEventListener 绑定事件的时候，第二个参数不都是传入一个function么，为什么这里传入的是一个this ? 竟然是一个对象？
于是百度之，网上清一色的都是说第二个参数都是传入一个function.
之后查看了一个javscript的api，果然发现了问题。

![这里写图片描述](/postimg/20161118153238485.png)

这里解释得很清楚，第二个参数listener,是可以是一个实现了EventListener接口的对象，或者一个javascript function.（好吧，百度不行去谷歌）

那么接下来的问题是，怎么样才算是实现了EventListener接口呢？ javascript 又没有 implement 之类的关键字。

![这里写图片描述](/post/img/20161118153604896.png)

从这里得到一些信息，所有的javascript function 对象都会实现这个EventListener接口，并且会自动调用
handleEvent方法。

得到了这些结论之后，再细查IScroll的源码,果然如此:

![这里写图片描述](/postimg/20161118154050832.png)
IScroll 偷偷的实现了  handleEvent方法。
总结一下：
1 addEventListener 的第二个参数是可以传递一个funciton，或者一个对象，只要这个对象实现了EventListener接口。
2 那么每次我们绑定addEventLisenter的时候，如果是一个实现了EventLisener接口的对象，那么会自动调用handerEvent。

举个栗子 ：
![这里写图片描述](/postimg/20161118154923617.png)