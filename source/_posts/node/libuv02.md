---
title: libuv 中 Event loop 以及 Thread pool 机制
date: 2022-01-25 20:34:58
tags: node
description: 关于 libuv 中 Event loop 以及 Thread pool 机制的一些探索
---

### 有言在先
以下东西有点乱，有些东西我也没有想清楚，甚至都都是一些猜想，用于记录，以后方便需要答案。
### 问题

以下代码用于读取一个大概 100M 大小的文件，并记录其使用的时间：
``` javascript
const fs = require('fs')
console.time(1)
fs.readFile('111.gz', (err, data) => {
  if (err) {
    return console.log(err)
  } else {
    console.timeEnd(1)
  }
})
```
// 大约 260MS

{% asset_image thread-pool-single.png %}


接下来使用循环读取同一个问题件10次：
``` javascript
const fs = require('fs')
for (let i = 0; i < 10 ; i++) {
  console.time(i)
  fs.readFile('111.gz', (err, data) => {
    if (err) {
      return console.log(err)
    } else {
      console.timeEnd(i)
    }
  })
}
```
{% asset_image thread-pool-multiple.png %}

为什么单个查询的时候就 260 MS, 怎么就 10 循环就 每个查询都变成了 1900 MS 呢？

#### thread pool
nodejs， 其中 v8 执行 js 之后会将 对应的异步请求 交个 thread pool 或者 event loop。对于 `fs.readFile` 这种系统调用，会交给 `thread pool` 线程池进行处理。
nodejs中默认的开辟的 `thread pool` 个数是 `4` 个，如果需要增加可以使用  `process.env.UV_THREADPOOL_SIZE` 进行修改。最大值可以被设置为 `128`.

>  如果等待的io请求超过了 线程池`thread pool` 线程的数量，那么后面的 io 请求就会进行等待，需要等待前面处理完成之后才能有机会处理后面的请求。

网上给出的各种资料，以及官方给出的逻辑都是上面这种说法。但是如果按照上面的说法，前面的例子的结果应该是下面这样才对：
```
260ms
260ms
260ms
260ms
520ms
520ms
520ms
520ms
880ms
880ms
```
当然时间可能有误差，但是应该是这种一批一批的时间（因为每次都是4个嘛），而不是所有的请求的时间都是一样的。

> 所以那为什么会变成最上面那样呢？
#### thread pool 猜想
具体情况我也不清楚，但是网上看到一篇文档可能是正确答案。

{% asset_image thread-pool-list.png %}

首先 默认的 线程池线程只有 4个，而这里有 10 个 io 请求，那么接下来当发现 线程池 thread pool 中不足时，怎么办呢？

首先线程会去请求文件，然后将请求文件操作交给操作系统，当前线程并不确定文件是什么时候返回，所以直接先暂时去执行后面的任务。

{% asset_image thread-pool-list2.png %}

{% asset_image thread-pool-list3.png %}

最终 thread pool 中所有线程 被 `7 8 9 10` 所在的io 请求占据。但是这个时候如果其他的请求数据已经成功了怎么办呢？ 
这个时候是没有线程进行接收的，所以就必须等到目前的线程池中的线程先处理完当前的任务才能进行处理。
所以必须等到 `7 8 9 10` 都处理完了，4个线程开始释放出来，就可以开始处理前面的请求，所以就导致了所有的请求消耗的时间基本保持相同。

{% asset_image thread-pool-list4.png %}

### Event loop 和 Thread pool

> The event loop to execute JavaScript and also performs some asynchronous operation orchestration there (e.g. sending/receiving network traffic, depending on OS support for true asynchronous operations as exposed via libuv)

> event loop 执行 JavaScript 和执行 一些 异步操作（比如 发送/接受网络数据转换，取决于通过libuv暴露的操作系统对于的的异步操作的支持)

> The worker pool (aka the thread pool) handles asynchronous I/O operations for which there is weak kernel support, namely, file system operations (fs) and DNS operations (dns); and is also used for asynchronous CPU-bound work in Node.js core modules, namely compression (zlib) and cryptography (crypto).

> 线程池处理内核支持较弱异步 I/O 操作，即文件操作（fs) 和  dns 操作，同时还用于 异步cpu密集型工作在nodejs核心模块，即 zib和 cryto.


### Event loop phases
```
timers: this phase executes callbacks scheduled by setTimeout() and setInterval().
pending callbacks: executes I/O callbacks deferred to the next loop iteration.
idle, prepare: only used internally.
poll: retrieve new I/O events; execute I/O related callbacks (except close callbacks, and the ones scheduled by timers, and setImmediate()); node will block here when appropriate.
check: setImmediate() callbacks are invoked here.
close callbacks: some close callbacks, e.g. socket.on('close', ...).
```
这里边对于第二个阶段其实是很模糊的，以前叫做 i/O callbacks, 现在叫做  pending callbacks。
对一 pending callbacks 阶段似乎都没有怎么仔细的解释。来自官方的解释：
> In this phase I/O-related callbacks will be executed, deferred to the next loop iteration. For example, if you registered a callback fired after write or read to file has finished, or some network operation is completed, this callback will run in this phase.

> In this phase, the event loop executes system-related callbacks if any. For example, let's say you are writing a node server and the port on which you want to run the process is being used by some other process, node will throw an error ECONNREFUSED, some of the *nix systems may want the callback to wait for execution due to some other tasks that the operating system is processing. Hence, such callbacks are pushed to the pending callbacks queue for execution.

> The asynchronous I/O request is recorded into the queue and then the main call stack can continue working as expected. In the second phase of the Event Loop the I/O callbacks of completed or errored out I/O operations are processed.

但是在 poll phases 阶段也看到这么一段：
> This phase is the one which makes Node.js unique. In this phase, the event loop watches out for new async I/O callbacks. Nearly all the callbacks except the setTimeout, setInterval, setImmediate and closing callbacks are executed.

> It’s basically a phase where nodejs looks for a new IO events and do it’s best to execute their callbacks immediately if possible. If not, it will defer a callback execution and register this as a pending callback. So it has two main responsibilities:

> Calculate how long it should block until next I/O polling
> Process events in the polling queue.

> It also checks if there are any timer callbacks and if so, it jumps to that timer phase and execute them right away. This means that it can jump back not completing the iteration. If there are no timers awaiting it will continue.


你就会发现这里的 poll 和 pending callbacks 阶段都是和 io 有关系，那么他们有啥区别呢。
找了很多资料基本没有说清楚的。

有几个疑问：
1 都是 io 处理，他们有什么区别？
2 在poll 阶段，会进行 schedule timer phases, 是不是 第二个阶段也是 poll 进行分发的？

在 reddit 上有人提出了疑问，但是似乎也还是没有得到答案。[libuv - How is the Poll Phase differentiated from I/O Callbacks phase?](https://www.reddit.com/r/node/comments/75m6cx/libuv_how_is_the_poll_phase_differentiated_from/?st=ja484wrx&sh=4c279c13)

### 参考
[你真的懂 Node.js 裡 Libuv 如何使用 Thread Pool 嗎？](https://briancodeitup.blog/2021/10/18/%E4%BD%A0%E7%9C%9F%E6%87%82-Node-js-%E8%A3%A1-Libuv-%E5%A6%82%E4%BD%95%E4%BD%BF%E7%94%A8-Thread-Pool%E5%97%8E%EF%BC%9F/)
[How does thread pool works in node.js?](https://medium.com/@joydipand/how-does-thread-pool-work-in-node-js-c48f3b3662a9)
[On problems with threads in node.js](https://kariera.future-processing.pl/blog/on-problems-with-threads-in-node-js/)
[How does the Event Loop work in Node.js](https://www.mariokandut.com/how-does-the-event-loop-work-in-node-javascript/)
[When is the thread pool used?](https://stackoverflow.com/questions/22644328/when-is-the-thread-pool-used)
[Understanding the Node.js event loop phases and how it executes the JavaScript code.](https://dev.to/lunaticmonk/understanding-the-node-js-event-loop-phases-and-how-it-executes-the-javascript-code-1j9)
[Behind Node.js - The Event Loop](https://nexocode.com/blog/posts/behind-nodejs-event-loop/)

[Introduction to the event loop in Node.js](https://developer.ibm.com/tutorials/learn-nodejs-the-event-loop/)
[IBM-Developer](https://github.com/jstevenperry/IBM-Developer/tree/master/Node.js/Course)
[jstevenperry](https://jstevenperry.wordpress.com/)