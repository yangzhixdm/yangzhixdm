---
title: 移动端填坑
date: 2016-12-08 13:23:19
tags:
---
###  1. IOS 表单元素点击自带背景阴影
最近一直做移动端前端开发，发现一个神奇的东西，对于IOS下的，input或者label元素，
点击之后，总是会出现一个背景阴影的现象。
之前一直怀疑是Fastclick这个哥们搞的鬼，最后查了fastclick的源码，fastclick并没有对它进行过处理，最后发现是IOS系统自带的背景色。需要消除的话，只需要一行CSS代码即可：
在你需要消除效果的元素上加上：
```CSS
//透明度设置为0，去掉点击链接和文本框对象时默认的灰色半透明覆盖层(iOS)或者虚框(Android)
-webkit-tap-highlight-color:rgba(0,0,0,0);
```
接下下，一切又恢复了平静。

### 2. elementFromPoint
发现了一个比较有意思的方法，可以根据坐标获取屏幕上的节点。
``` html
<!DOCTYPE html>
<html lang="en">
<head>
<title>elementFromPoint example</title>

<script>
function changeColor(newColor) {
  elem = document.elementFromPoint(2, 2);
  elem.style.color = newColor;
}
</script>
</head>

<body>
<p id="para1">Some text here</p>
<button onclick="changeColor('blue');">blue</button>
<button onclick="changeColor('red');">red</button>
</body>
</html>
```
MDN上的概述如下：
返回当前文档上处于指定坐标位置最顶层的元素, 坐标是相对于包含该文档的浏览器窗口的左上角为原点来计算的, 通常 x 和 y 坐标都应为正数.

但是还有一点附注：
If the element at the specified point belongs to another document (for example, an iframe's subdocument), the element in the DOM of the document the method is called on (in the iframe case, the iframe itself) is returned. If the element at the given point is anonymous or XBL generated content, such as a textbox's scroll bars, then the first non-anonymous ancestor element (for example, the textbox) is returned.

If the specified point is outside the visible bounds of the document or either coordinate is negative, the result is null.

具体就是说如是存在多个document的话，如iframe， 那么它会直接返回节点所在的iframe，如果节点的坐标超出了可视的边界或者坐标为负数， 那么会返回 null值。
如果坐标所在的位置的节点，是匿名的或者说是XBL生成的内容（如textbox的滚动条，那么会返回它的第一个不为匿名元素的父节点）