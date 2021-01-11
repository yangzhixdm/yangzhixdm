---
title: css中的border
date: 2014-03-27 15:52:50
tags:
description: 哈哈哈，其实就是css border的一些疑问。
---
曾经一直以为html中的border是直角的，直到有一天：
``` css
.test{
  border: 20px solid;
  border-top-color:#ff0000;
  border-right-color:#00ff00;
  border-bottom-color:#0000ff;
  border-left-color:#FFFF00;
  width:100px;
  height: 100px;
  background: #272822;
}
```

效果如下：
![](/postimg/20140327154959234.png)

突然有一种被欺骗了的感觉。
![](/postimg/20140327155144187.png)

我们可以选择理解一个border为两个三角形再加一个矩形。如上所示。

![](/postimg/20140327155449234.png)

绘制小尾巴:
![](/postimg/20140327155603921.png)
``` css
border-right: 6px solid black;
width: 100px;
height: 70px;
border-bottom-right-radius: 70px 70px;
```

当对一个角应用圆角样式，如果这个角相邻的两个border一个有定义而一个无定义，那么绘制的结果就是由粗到细的“小尾巴了”。[在整个绘制过程中，同一个图形它的绘制方法有很多种，例如一个矩形可以用 width x height构成，也可以由border x height(width)构成，甚至可以由border组合(width = height = 0)构成，根据情况选择吧。]

偶有所得，以记录之，以防忘记。