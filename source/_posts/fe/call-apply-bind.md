---
title: call，apply，bind
date: 2018-10-21
tags:
---


#### 作用
call，apply，bind 方法都可以实现方法调用。
call，apply可以用来修改上下文环境，修改this。
``` javascript
var o = {
  name: "this is o",
  say() {
    console.log(this.name);
  }
};
var name = "this is window";
o.say(); // this is o
var say = o.say;
say(); // this is window
say.call(o); //this is o
say.apply(o) // this is o
```
而bind可以用来修改上下文属性，修改this，只是有一点点区别。
``` javascript
var fn = say.bind(o); //获取传入this值之后的回调方法
fn(); //this is o
```
手动实现bind方法（精简版）。
``` javascript
var bind = function(context) {
  var self = this;
  return function(){
    self.call(context,arguments);
  }
}
```
#### 关于call与apply的区别
call的参数列表为一个一个参数，而apply的第二个参数必须为`数组`。
而bind则是先传入上下文对象，以及参数列表，然后返回绑定了上下文的方法但并不执行，需要手动调用。
``` javascript
var o = {
  name: 'this is o',
  say(x, y, z) {
    console.log(x, y, z);
  }
}
var name = 'this is name';
var saycall = o.say;
saycall.call(o,1,2,3);
var sayapply = o.say;
sayapply.apply(o,[1,2,3]);
```
而bind方法如下：
``` javascript
var say = o.say;
var saybind = say.bind(o,1,2); //传入this以及第一个和第二个参数
saybind(3);
```