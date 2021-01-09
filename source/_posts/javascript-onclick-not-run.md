---
title: jquery动态改变onclick属性失效的问题
date: 2013-12-04 11:13:58
tags:
---

<li id="" onclick =""></li>

虽然在很多时候，都是非常不推荐这样写的，但是还有有些时候会碰到这样的代码。

那么你可能在某些时候需要去修改onclick 的属性如下：

``` javascript
$("#id").attr("onclick",url);
```
这样的代码，在chrome和firefox 中可以得到你想要的结果。

遗憾的是在ie中，至少是ie7 以下是没有效果的，至于i8好像也是没有效果的。

办法就是使用jquery 绑定.
``` javascript
$("#id").attr("onclick","").click(function(){
//
});
```

这里需要先清除onclick属性的值，再去绑定方示。

如果本来就没有设定onclick属性，刚不需要清除.
``` javascript
$("#id").click(funciton(){
//
});
```




