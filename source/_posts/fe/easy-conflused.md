---
title: 一些容易混淆的问题
date: 2018-11-11
tags:
---
#### 有言在先
记录一些比较容易混淆或者出错的问题。

#### instance of
``` javascript
o instance of f
```
> 首先计算f.prototype，然后在o的原型链中查找这个值，如果找到，那么o是f（或者f的父类）的一个实例，表达式返回true，如果f.prototype不在o的原型链中，那么o就不是f的实例，返回false.（关于这一段，权威指南的中文翻译是有问题的，看了半天没看懂，还是得啃原文的）

> To evaluate the expression o instanceof f, JavaScript evaluates f.prototype,
and then looks for that value in the prototype chain of o. If it finds it, then o is an
instance of f (or of a superclass of f) and the operator returns true. If f.prototype is not
one of the values in the prototype chain of o, then o is not an instance of f and
instanceof returns false

> 需要注意的是instance of 判断的是 prototype，而与构造方法没有什么必然的联系。

#### 关于splice与slice
##### 1 splice(index, delnum, addItem.......) 
可以理解为删除方法
最后一个参数为可变参数，用来做增加用，第一个参数为删除元素的index，delnum为偏移量，即删除个数。
delnum可为0，表示不删除。
addItem.... 可省略，表示不添加。

##### 2 slice(start,end) 
同样为删除方法
start参数为起始索引，end为结束索引。end参数可省略。
如果end参数缺失，则为从start至数组结束皆移除。
如果end为负数，则end的值为数组末尾开始算起的元素。

##### 3 二者异同
1 皆可用于删除数组元素
2 splice可用于添加，slice不能
3 slice不操纵原数组，splice修改原数组。
4 slice返回值为删除元素之后的数组，splice返回值为被删除的元素，且原数组已移除需要删除的元素。

#### defer与async
果然是一图胜千言呀
![defer与async](https://raw.githubusercontent.com/helloyangzhi/learn/master/defer%E4%B8%8Easync.png)
> 图片转载地址(https://www.growingwiththeweb.com/2014/02/async-vs-defer-attributes.html)

#### -和+ 的隐形区别
``` javascript
-(-a[i]-b[i])
```
其实想表达的意思应该与
``` javascript
a[i] + b[i]
```
>这里面有一些细微的差别，那就是+ 和-的区别。
1 其中 + 和 - 都可以用做一元运算符。都会将操作数转换为number类型。其中+ 取正，而-取反。
2 +和-用做二元运算符时，+ 可以用做 加法运算，也可以用来表示连接运算符号，而至于是使用的哪一种，取决于操作数的类型。接来其中一个为对象，则会转换为其原始值。如果为string类型，则会调用对象的toString()转换，其它对象调用valueOf方法转换。但是多数对象都没具备可用的valueOf方法，因此会调用toString()方法进行转换。如果转换之后其中一个字符串为字符串，另一个操作数也会被转换为字符串，然后进行字符串连接操作。

> 而对于 - ，只会被用做减法运算符号，两边的操作数会自动进行转换为number类型。

以下来自 JavaScript权威指南：
> If either of  its operand values  is an object,  it converts  it to a primitive using the
object-to-primitive algorithm described in §3.8.3: Date objects are converted by
their toString() method, and all other objects are converted via valueOf(), if that
method  returns a primitive value. Most objects do not have a useful valueOf()
method, however, so they are converted via toString() as well.
After object-to-primitive conversion, if either operand is a string, the other is con-
verted to a string and concatenation is performed.
Otherwise, both operands are converted to numbers (or to NaN) and addition  is
performed.
Here are some examples:
1 + 2         // => 3: addition
"1" + "2"     // => "12": concatenation
"1" + 2       // => "12": concatenation after number-to-string
1 + {}        // => "1[object Object]": concatenation after object-to-string
true + true   // => 2: addition after boolean-to-number
2 + null      // => 2: addition after null converts to 0
2 + undefined // => NaN: addition after undefined converts to NaN