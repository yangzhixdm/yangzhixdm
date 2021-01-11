---
title: jQuery深入之大图轮播原理
date: 2014-02-17 09:48:17
tags:
description: 就是一个轮播而已。
---
也是用了很久的jQuery.

最近也在写自己的那个小站。对于一些特效还是想弄清一原理。让自己对一些东西有更深入的了解。

说说大图轮播的原理：

1.很多时候使用方法Ul列表去展示。当然这也有一些好处，比如float对齐之类的。当然直接用p或者div也行。

2.了解overflow属性。在溢出情况下的处理。其实轮播就是在不断的处理li溢出的情况。

3 jQuery animate的动画效果。当然如果不用这个也行。一个setInterval就能搞定它。不过jQuery还是做了一些封装。

4 可能还需要一些基础的理解就是对定位的熟悉。margin和postion的了解。

5 之后就是循环轮播了，循环轮播需要对节点进行重新的修改。

具体而言就是在轮播到最后一张图片的时候，修改节点，将第一个节点，添加到列表的最后一个位置。如下：
``` javascript
	$(this).css({'margin-left':0}).children('li').last().after($(this).children('li').first());
```

看一下核心代码：

html:
``` html
<div class="list" id = "sidle">
  <ul>
    <li><img src="1.jpg" width="538" height="198" /></li>
    <li><img src="2.jpg" width="538" height="198" /></li>
    <li><img src="3.jpg" width="538" height="198" /></li>
    <li><img src="4.jpg" width="538" height="198" /></li>
  </ul>
</div>
```
css:
``` css
.list{
	width:538px;
	height:198px;
	overflow:hidden;
	margin:50px auto;
}
.list ul{
	width:2152px;
	height:198px;
	margin:0;
	padding:0;
}
.list ul li{
	float:left;
	width:538px;
}
```

js:个人稍微封装了一下：
``` javascript
	(function($){
		var silde = {
			init:function(){
				this.auto();
			},
			auto:function(){
				var _root = this,
					$ul = $("#sidle").find("ul"),
					$lis = $ul.children("li"),
					width = $lis.eq(0).width();
				setInterval(function(){
					$ul.animate({
							'margin-left':'-'+ width +'px'
						},
						'slow',
						function(){
							//此处保证能循环轮播
							//将已经轮播过的节点的第一张图片添加到末尾的位置
							//再将margin-left重置为0px;
							//这样就能一直的循环下去.
							$ul.css({'margin-left':0}).
								children('li').
								last().
								after(
									$ul.children('li').first()
								);
						});
					},2000
				);
			}
		};
		$(function(){silde.init();})
	})(jQuery);
```










