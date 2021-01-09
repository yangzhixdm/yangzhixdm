---
title: scrollTop与offset()
date: 2014-03-25 23:04:58
tags:
---
``` javascript
jquery scrollTop() 与offset()!
```

曾经很长一段时间，很多人问我下面一段代码的好处是什么？
``` javascript
;(function($){
    //do something

})(jQuery);
```

但我一直就是这么写着，似乎觉得它本来就是这个样子的。

对说书中的那么套理论，可以完全不在乎，只知道是为了防止污染，对于污染一词，也许和环境污染一样。

只到有一天，我碰过了两个高手对决(jQuery,prototype) 的时候，对此恍然大悟。

jquery,和prototype一样都是使用了$做为简写引用。那么如果不采用命名空间的方式，$只为能其中一家所使用。

并且在某些时候会有性能的问题（个人猜想，比如你写一个$，那么它可能会先去jquery中寻找，发现没有，再去prototype中寻找）。

上述可以直接使用，或许在某些时候能节省一些性能。

对于offset与scrollTop.在网上有很多的文章，但个人觉得都有些牵强。以下为个人总结。

scrollTop 是该元素，被浏览器卷走的高度,通俗而言，就是因为滚动条的下拉，导致元素的一部分被浏览器的头部遮掉。

而这一部分就是scrollTop的值。

offset() 则是该元素距离文档顶部的的距离。这里一定要注意是文档的顶部的距离，而不是说浏览器顶部的距离。

说说注意的地方：

如果说本身没有滚动条的话，直接设置scrollTop()的话，可能是无效的。

具体情况可以通俗说就是：

比如，你的文档只有那么高，一个屏幕就能够显示了，或者说即使出现了滚动条

但是元素的顶部还没有滚到浏览器的顶部，文档就已经结束了的情况下：

你强行的输入：
``` javascript
scrollTop(0);
```

这样的情况肯定是无效的。

偶有所得，以记录之，以防忘记。