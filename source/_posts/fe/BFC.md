---
title: BFC
date: 2019-2-22
tags: css
---

#### BFC
块级作用上下文，以下是来自W3C的官方解释：

>Floats, absolutely positioned elements, block containers (such as inline-blocks, table-cells, and table-captions) that are not block boxes, and block boxes with 'overflow' other than 'visible' (except when that value has been propagated to the viewport) establish new block formatting contexts for their contents.

> 浮动元素，绝对定位元素，以及非块级盒子的块级容器，以及overflow的值不为visible的块级盒子（当这个visible的值直接传播到viewport时会有例外）都会为它们的内容创建一个块级格式化环境。

#### BFC 作用
##### 修复margin折叠
> Box垂直方向的距离由margin决定。属于同一个BFC的两个相邻Box的margin会发生重叠
如果让两个元素不属于同一个BFC，就不会发生margin重叠了。

##### 清除浮动
> 计算BFC的高度时，内部的浮动元素也会参与计算。可以用来解决float元素导致的父元素坍塌的问题。

##### 自适应布局
> 之前一直没有弄明白为什么可实现自适应布局？或者说原因是什么？终于在 [w3c](https://www.w3.org/TR/CSS21/visuren.html#floats)文档上找到了一段。

> The border box of a table, a block-level replaced element, or an element in the normal flow that establishes a new block formatting context (such as an element with 'overflow' other than 'visible') must not overlap the margin box of any floats in the same block formatting context as the element itself. If necessary, implementations should clear the said element by placing it below any preceding floats, but may place it adjacent to such floats if there is sufficient space. 

> 表格的border盒子，块级替换元素，以及一个overflow值不为visible的正常流的元素会创建一个新的BFC,并且这个元素不会与在同一个父BFC中的浮动元素的空白盒子（margin box )相重叠。如果有必要的话，正常的实现应该清除上面的元素，并且把它放在浮动元素的下面，但是也可能放在浮动元素的旁边，如果还有合适的空间的话。