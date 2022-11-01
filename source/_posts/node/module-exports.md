---
title: module.exports
date: 2021-11-22 11:26:38
tags: node
description: 关于module.exports 和 exports 的区别.
---

### 前言
其实说到底就是 `引用` 的问题，直接修改 exports 是不会修改 module.exports 的。 

类似于: 
``` javascript
let obj = {
  prop: {
    name: 'yang'
  }
}
let copyObj = obj.prop
copyObj = {
  name: 'hello'
}
```
这里会发现， obj 中的属性根本就没有变化，但是如果是使用属性复制就可以达到目的。
``` javascript
let obj = {
  prop: {
    name: 'yang'
  }
}
let copyObj = obj.prop
copyObj.name = 'hello'
```
这样的赋值，则可以影响 obj 中的属性。

### 原理
重新给 copyObj 重新赋值会导致之前的引用 断开，重新指向新的对象。而如果只是修改其中的属性，那么只会导致其中的属性变更，而不会导致copyObj引用变更。
这里还是涉及 对象 和 基本属性在内存中的存储方式。基本属性存储在栈中，而对象则会在内存中重新开辟堆栈。所以直接给 copyObj 赋值一个对象，就相当于是重新开辟了堆栈，并赋值给copyObj引用。如果只是修改其name属性，那么只是修改了栈中的值而已。
{% asset_image obj-fuzhi.jpeg %}


### Node module 的处理方案

``` javascript
// -------- node.js core --------

var module = {
  exports: {

  }
};

exports = module.exports;

// -------- 下面正常写代码 --------

exports.name = 'Alan';
exports.test = function () {
  console.log('hi')
};

// 给导出的对象添加属性，直接这样就好了
console.log(module) // { exports: { name: 'Alan', test: [Function] } }

exports = {
  name: 'Bob',
  add: function (a, b) {
    return a + b;
  }
}

// 不过 ① exports 是一个引用，直接赋值给它，只是让这个变量等于另外一个引用
console.log(exports) // { name: 'Bob', add: [Function] }
// 并不会修改到导出的对象
console.log(module) // { exports: { name: 'Alan', test: [Function] } }

module.exports = {
  name: 'Bob',
  add: function (a, b) {
    return a + b;
  }
}

// ∵① 所以 只有通过 module.exports 才能真正修改到 exports 本身
console.log(module) // { exports: { name: 'Bob', add: [Function] } }

```