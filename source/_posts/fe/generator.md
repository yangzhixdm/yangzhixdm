---
title: Generator
date: 2018-10-09
tags:
description: Generator 了解
---

### generator 含义
1 状态机，封装了多个内部状态（使用next调用）
2 遍历器对象生成函数，执行Generator函数会返回一个遍历器对象

``` javascript
function* helloWorldGenerator() {
  yield 'hello';
  yield 'world';
  return 'ending';
}

var hw = helloWorldGenerator();
```
`yield 为暂停执行的标记，next方法可以恢复执行`。

``` javascript
hw.next()
//{ value: 'hello',  done: false }
hw.next()
//{ value: 'world',  done: false }
hw.next()
//{ value: 'ending',  done: true }
hw.next()
//{ value: undefined,  done: true }
```
tips: 从第四次之后再调用next都会是一样的值，因为在第三次的时候generator方法已经执行完毕了。

### yield
next方法执行逻辑：
1 遇到yield暂停后面的执行，并将紧跟在yield后的表达式的值作为返回的对象的value属性的值。
`只会执行yield后面的表达式，如果有赋值操作，会被放到下一次next执行`。
如下：
``` javascript
function* foo(){
  yield 'hello';
  console.log('1');
  yield 'word';
  console.log('2');
  yield 'ending';
  console.log('3');
}

var f = foo();
f.next();  //{ value: 'hello' , done: false }
f.next();  // 1 { value: 'world', done: false }
f.next();  // 2 { value: 'ending', done: false }
f.next();  // { value: undefined, done: true }
```
第一次调用，碰到了yield就会停止后面代码的执行，然后把  'hello'的值作为返回对象的value属性。
第二次调用，先执行console.log(1)，然后碰到了yield就会停止后面代码的执行，然后把  'word'的值作为返回对象的value属性。
第三次调用，先执行console.log(2)，碰到了yield就会停止后面代码的执行，然后把  'ending'的值作为返回对象的value属性。
第四次调用，generator方法已经执行完毕，然后把  undefined 的值作为返回对象的value属性，并且 done的值为 true。
``` javascript
function* foo(){
  yield func();
  console.log('1');
  yield 'word';
  console.log('2');
  yield 'ending';
  console.log('3');
}

function func(){ return 'func test'}
```

上面的代码可以看出，紧跟在yield后面的表达式是会被执行的，然后把执行之后的值，作为返回对象的vlalue值。

`即时思考 : 这里的yield后面的是直接执行的，如果后面跟一个Promise然后在resolve中执行next，就可以逃离异步的回调地域，以同步的方式书写。`

2 下一次调用next方法时再继续往下执行，直到遇到下一条yield语句。
3 如果没有遇到新的yield语句，就一直运行到函数结束，直到遇到return语句为止，并将return语句后面的表达式的值作为返回的对象的value属性值。
4 如果没有return语句，则返回对象的value属性为undefined。

需要注意的是yield语句后面的表达式，只有当调用了next方法、内部指针指向该语句时才会执行。

### next方法参数

yield语句本省没有返回值，或者说总是返回undefined，next方法可以携带一个参数，该参数会被当作`上一条`yield语句的返回值。
``` javascript
var x = 0;
function* foo() {
  var a = yield ++x;
  console.log(x);
  console.log(a); 
}

var f = foo();  //返回 generator对象
f.next(); // { value: 1, done: false }
f.next(2); // 把传入的 2 作为赋值给a（此时yield ++x，也就是上一条yield语句了），输出1 2 
```
next方法的参数表示上一条yield语句的返回值，所以第一次使用next方法时不能带有参数，v8引擎会自动忽略第一次使用next方法的参数。可以认为第一次next方法，用来启动遍历器对象，所以不能带参数。

### Generator与状态机
下面的clock就是一个状态机。
``` javascript
var ticking = true;

var clock = function() {
  if(ticking) {
    console.log('Tick');
  }else{
    console.log('Tock');
  }
  ticking = !ticking;
}

clock();
clock();
clock();
clock();

//每运行一次，就切换一次状态。

var clock = function* (_) {
  while(true) {
    yield _;
    console.log('Tick');
    yield _;
    console.log('Tock');
  }
}

var c = clock();
c.next();
c.next();
c.next();
c.next();
c.next();
//每调用一次next就切换一次状态，因为可以使用yield暂停住代码的执行。
```

### 异步操作同步化表达
``` javascript
function* loadUI() {
    showLoadingScreen();
    yield loadUIDataAsynchronous();
    hideLoadingScreen();
}

var loader = loadUI();
loader.next();//加载UI

loader.next();//卸载UI
```
### 与Promise组合
``` javascript
function getFoo() {
    return new Promise(function(resolve, reject){
        resolve('foo');
    });
}

var g = function* () {
    try {
        var foo = yield getFoo();
        console.log(foo);
    } catch(e) {
        console.log(e);
    }
}

function run(generator) {
    var it = generator();

    function go(result) {
        if(result.done) {
            return result.value;
        }

        return result.value.then(function(value) {
            return go(it.next(value));
        },function(error) {
            return go(it.throw(error));
        })
    }

    go(it.next());
}

run(g);
```