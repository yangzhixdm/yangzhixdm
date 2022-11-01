---
title: 函数式编程
date: 2018-10-29
tags:
---

#### 函数式编程
>相比于命令式编程关心解决问题的步骤，函数式编程是面向数学的抽象，关心数据（代数结构）之间的映射关系。函数式编程将计算描述为一种表达式求值。

>在函数式语言中，函数被称为一等函数（First-class function），与其他数据类型一样，作为一等公民，处于平等地位，可以在任何地方定义，在函数内或函数外；可以赋值给其他变量；可以作为参数，传入另一个函数，或者作为别的函数的返回值。

#### 纯函数
纯函数是这样一种函数，即相同的输入，永远会得到相同的输出，而且没有任何可观察的副作用。

``` javascript
//不纯
var minimum = 21;  //外部变量会影响checkAge函数的运行结果

var checkAge = function() {
  return age > minimum;
}

//纯函数
var checkAge = function() {
  var minimum = 21;
  return age > minimum;
}
```
那么关于函数的bind是纯函数的一种实现。
``` javascript
function bind(context) {
  var self = this;
  return function() {
    self.apply(context, arguments);
  }
}

var obj = {
  name: "this is obj"
}

var say = function() {
  console.log(this.name);
}

var fn = say.bind(obj); 
//返回一个纯函数方法，即在任何时候，只要输入一样，那么返回的结果都是一样的。

fn();  //this is  obj
```

#### 柯里化(currying)
> 在计算机科学中，柯里化（Currying）是把接受多个参数的函数变换成接受一个单一参数(最初函数的第一个参数)的函数，并且返回接受余下的参数且返回结果的新函数的技术。
``` javascript
function curry(fn) {
  var args = Array.prototype.slice.call(arguments, 1);
  return function() {
    var innerArgs = Array.prototype.slice.call(arguments);
    var finalArag = args.concat(innerArgs);
    return fn.apply(null, finalArag);
  }
}

function add(num1, num2) {
  return num1 + num2;
}

var curryAdd = curry(add, 5); //确定了第一个参数，之后返回一个纯函数
curryAdd(3); //8
```
也可以如下调用：
``` javascript
var curryAdd = curry(add, 5, 12); //确定二个参数，之后返回一个纯函数
curryAdd(); //17
```
#### 更复杂的bind方法
``` javascript
function bind(fn, context) {
  var args = Array.prototype.slice.call(arguments, 2);
  return function() {
    var innerArgs = Array.prototype.slice.call(arguments);
    var finalArag = args.concat(innerArgs);
    return fn.apply(context, finalArag);
  }
}

var handler = {
  message: "Event handler",
  handlerClick: function(name, event) {
    console.log(this.name + ":" + name + ":" + event.type)
  }
}

var btn = document.getElementById('my-btn');
EventUtil.addHandler(btn, "click", bind(handler.handlerClick, handler, 'my-btn');
```
