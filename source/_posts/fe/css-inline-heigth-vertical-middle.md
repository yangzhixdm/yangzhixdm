---
title: line-height 与垂直居中
date: 2014-03-29 18:06:22
tags:
description: 关于 line-height 的一些基础知识。
---
在此之前，对于line-height 与垂直居中的问题，经常碰到。 

比如，图片与span在同一个box中的时候，竟然会各种偏移。要想达到理想的效果真的是各种难。

有时间，决定认真的啃一啃。

### line-heigth:

1. line-height: 顾名思义，行高，指代文本中，行与行之间的基线间的距离。

2. 基线： 这里的基线不是指代文字的底部，而是英文字母x 的下沿。

3 行距： 行距表示文字的高度与行高的差距。

4. 内容区域： 行中每一个元素都有一快内容区域，主要由字体的大小决定。

5 行内框： 在默认条件下，行内框是等于内容区域的。在如果设置了行高，那么，行内框就会等于行高，并且在内容区域的两侧添加空白。

6 行框： 行框在默认条件下是等于行内框，但是如果在同一行中，有行高超过了当前元素的行内框高度的时候，当前元素的行框就会默认为最高的行高的高度。

这里的行框的告诉只与本行中的最高的行高的关，与关元素的告诉无关。这也是导致图片和文字在同一行时不能对齐的原因。

7 在有图片时，图片会将行框的高度撑到图片的高度，但同时默认的对其方式为基级对齐方式。

具体如下图所示：
![](/postimg/20140329182925796.png)

### vertical-align

垂直居中，这里需要注意的一点就是，vertical-align 只能对行内元素有效，对说类似div,p之类的块级元素是无效的。

需要垂直居中，只需要：vertical-align:middel

### zoom

曾经一直很奇怪很多的网站为什么给元素总是要加一个zoom:1的样式。

原来这一切都是为了兼容ie 6/7/8 , 用来触发ie的haslayout内部属性。导致ie重新计算自身的高度。

大多数因为浮动所产生和定位问题bug 都可以用zoom来解决。

### inline-block

inline属性的特点是，行内元素，能够在同一行内显示，但是其高度，宽度，行高，margin,padding 等元素都不能设置。所以就产生了inline-block;

inline-block行内元素，但不失block 的效果。在是在ie 6/7下没有被完全支持。

关于ie 6/7对于 inline-block的不完全支持，也可以采用上面的方法解决。

如：

display:inline-block; *display:inline; zoom:1; width:80px; height:20px; margin:10px; padding:10px; text-align:center; background:#cfc;


这样就能够使在ie 6/7中的inline元素具有inline-block的效果。

给个example:
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<title>span</title>
	<style type="text/css">
		.test{
			line-height: 50px;
		}
		.test img{
			vertical-align: middle;
			height: 50px;
			line-height: 50px;
		}
		.test-span{
			line-height: 50px;
		}
	</style>
</head>
<body>
<p class="test"><img src="未标题-1.jpg" width="265" height="203" /><span class="test-span">dd</span></p>
</body>
</html>
```
这里会存在一个小问题, 图片img会的p元素的顶部存在3像素作用的间距。

百度之，原来是个bug , 解决办法蛮多

第一种就是换doctype为<!doctype html>

其它如果不改doctype可以去设置含有img元素的高度，并且需要float一下就ok了。
``` css
.test{
  line-height: 50px;
  flost:left;
  height: 50px;
}
```
注意问题：这里需要垂直居中，是采用设置img的vertical-align的属性，而不是文本。

当然这也不是绝对的,具体需要看那一个元素的高度更高，就去设置哪一个元素的vertical-align属性。

当然还有一个经验之谈就是在切图的时候，将图的高度切为单数。这样的ie6/7下就不会有多出的1px的问题。
