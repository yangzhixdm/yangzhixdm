---
title: splice与slice
date: 2018-10-08
tags:
---

#### 有言在先
关于 splice 和 slice 的区别。

####  splice
``` js
    splice(index, delnum, addItem.......)
```
- 可以理解为删除方法
- 最后一个参数为可变参数，用来做增加用，第一个参数为删除元素的index，delnum为偏移量，即删除个数。
- delnum可为0，表示不删除。
- addItem.... 可省略，表示不添加。

#### slice
``` js
    slice(start,end)
```
- 同样为删除方法
- start参数为起始索引，end为结束索引。end参数可省略。
- 如果end参数缺失，则为从start至数组结束皆移除。
- 如果end为负数，则end的值为数组末尾开始算起的元素。

#### 二者异同
- 皆可用于删除数组元素
- splice可用于添加，slice不能
- slice不操纵原数组，splice修改原数组。
- slice返回值为删除元素之后的数组，splice返回值为被删除的元素，且原数组已移除需要删除的元素。