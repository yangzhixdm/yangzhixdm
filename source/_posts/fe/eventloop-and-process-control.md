---
title: 事件循环与任务循环以及流程控制
date: 2018-10-28
tags:
---

#### 事件循环（Event loop）
JavaScript引擎本身所做的只不过是在需要的时候，在给定的任意时刻执行程序中的单个代码块。
>（需要注意的是，宿主环境不止仅仅只有JavaScript引擎，还有需要的各种模块，比如负责网络的Network Layer，负责渲染的UI Layer 等，JavaScript只是其中的一环而已）。
![layer](https://raw.githubusercontent.com/helloyangzhi/learn/master/layers.png)

JavaScript的宿主环境提供了一种机制来处理程序中多个块的执行，且执行每块时调用JavaScript引擎，这种机制被称之为事件循环。

事件循环机制伪代码：
``` javascript
// `eventLoop` is an array that acts as a queue (first-in, first-out)
var eventLoop = [ ];
var event;

// keep going "forever"
while (true) {
  // perform a "tick"
  if (eventLoop.length > 0) {
    // get the next event in the queue
    event = eventLoop.shift();

    // now, execute the next event
    try {
      event();
    }
    catch (err) {
      reportError(err);
    }
  }
}
```

比如 **setTimeout**
> setTimeout并没有把你的回调方法放挂在事件循环中。而是设定了一个计时器，当计时器到时后，环境会将你的回调方法放在事件循环中，这样在未来某个时刻的tick会摘下并且执行。

> 如果这时候事件循环中已经有20个项目，那么你的回调就会被等待。它需要排在其它项目的后面。

#### 任务队列（Job queue）
任务队列，它是挂在事件循环队列的每个tick之后的一个队列。在事件循环的每一个tick中，可通信出现的异步动作不会导致一个完整的新事件加到事件循环队列中，而会在当前tick的任务队列末尾添加一个项目（一个任务）。这就像是在说：“嗨，这里还有一件事将来要做，但要确保在其他任何事发生之前就完成它."
``` javascript
console.log( "A" );

setTimeout( function(){
	console.log( "B" );
}, 0);

// theoretical "Job API"
schedule( function(){
  console.log( "C" );

  schedule( function(){
    console.log( "D" );
  });
});

// 这里输出为 A C D B
```
因为任务处理是在**当前事件循环tick**的**结尾**处，且定时器出发是为了调度下一个事件循环tick（如果可用的话）。
_tips: 添加到任务队列中的任务处理会在当前tick运行完成之后，再执行当前任务队列中的代码。_

#### 流程控制
以下为微信面试的题目，需要用到流程控制方面的知识。
``` javascript
实现一个LazyMan，可以按照以下方式调用:
LazyMan(“Hank”)输出:
Hi! This is Hank!
 
LazyMan(“Hank”).sleep(10).eat(“dinner”)输出
Hi! This is Hank!
//等待10秒..
Wake up after 10
Eat dinner~
 
LazyMan(“Hank”).eat(“dinner”).eat(“supper”)输出
Hi This is Hank!
Eat dinner~
Eat supper~
 
LazyMan(“Hank”).sleepFirst(5).eat(“supper”)输出
//等待5秒
Wake up after 5
Hi This is Hank!
Eat supper
 
以此类推。
```
1. 需要实现链式调用，需要每次返回调用的对象。
2. 要实现sleepFirst，则需要使用流程控制，无论它何时调用，都要第一个执行，就需要手动调整循环队列中的数据。使用 setTimeout(function(){},0)就可以实现这一点。

demo:
``` javascript
function LazyMan(name) {

    return new _lazyMan(name);
}

function _lazyMan(name) {
    this.tasks = [];

    var task = function(){
        console.log('Hi! This is ' + name);
    };

    this.tasks.push(task);

    var self = this;
    setTimeout(function(){
        var task = self.tasks.shift();
        task && task();
    },0);
}

_lazyMan.prototype = {
    eat: function(name) {
        var task = function(){
            console.log('eat' + name);
        };

        this.tasks.push(task);
        var self = this;
        setTimeout(function(){
            var task = self.tasks.shift();
            task && task();
        },0);

        return this;
    },
    sleep: function(seconds) {
        var task = function(){
            console.log('Wake up after ' + seconds);
        };

        this.tasks.push(task);
        var self = this;
        setTimeout(function(){
            var task = self.tasks.shift();
            task && task();
        },seconds);

        return this;
    },
    sleepFirst: function(seconds) {
        var task = function(){
            console.log('Wake up after ' + seconds);
        };

        this.tasks.unshift(task);
        var self = this;
        setTimeout(function(){
            var task = self.tasks.shift();
            task && task();
        },seconds);

        return this;
    }
}
```

``` javascript
var task = self.tasks.shift();
task && task();
```
可以考虑将上面代码提取出来：
``` javascript
_lazyMan.prototype = {
  ...
  nextTick: function() {
    var task = this.tasks.shift();
    task && task();
  }
}
```