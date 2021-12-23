---
title: parseInt和parseFloat的问题
date: 2021-12-23 10:48:41
tags: fe
description: 前方有坑。
---

#### 有言在先
我们常会用到parseInt和parseFloat来对数据进行数字类型的转换。parseInt会把相应的数据转成整数或NaN，parseFloat则可以将数转成浮点数。

``` js
parseInt('2'); // 2
parseFloat('2.3'); // 2.3
```

``` js
parseInt('3cc'); // 3
parseFloat('2.5k'); // 2.5
```

原本以为’3cc’和’2.5k’会被转成NaN，没想到js这么灵活，都给转过去了。但这有时候并不是我想要的。为啥会出来这样的情况呢？其实看看这两者用法的介绍就可以了解了。原文是这样的：

> parseInt() 方法首先查看位置 0 处的字符，判断它是否是个有效数字；如果不是，该方法将返回 NaN，不再继续执行其他操作。但如果该字符是有效数字，该方法将查看位置 1 处的字符，进行同样的测试。这一过程将持续到发现非有效数字的字符为止，此时 parseInt() 将把该字符之前的字符串转换成数字。

> parseFloat() 方法与 parseInt() 方法的处理方式相似，从位置 0 开始查看每个字符，直到找到第一个非有效的字符为止，然后把该字符之前的字符串转换成整数。


### more
如果你不能接受parseInt和parseFloat这种灵活的特性，可以用Nubmer来进行数值类型的转换。
``` js
Number('7kk'); // NaN
Number('7'); // 7
```
