---
title: javascript类工厂的实现
date: 2014-08-01 01:19:17
tags:
description: 工厂模式。
---
最近在研究关于javascript类的实现，通过类来实现重用与共享。

但是很多时候不可能总是一个个的在原型链上做动作，所以作为一个框架，那么需要一个类工厂来做支撑。

这一点上，jquery就有点迷茫了，prototype.js,mootools,在这一点上似乎做得不错。
``` javascript
function A(name){  
  this.name = name;  
}  
var a1 = new A("aa1");  
var a2 = new A("aa2"); 

function Animal(){

}
Animal.prototype.say = function(){
  console.log("animal say");
}
function Human(){

}

Human.prototype = Animal.prototype;

Human.prototype.eat = function(){
  console.log("human eat");
}

console.log(Human.prototype);
console.log(Animal.prototype);
```