---
title: JavaScript中的定时器(Timer) 是如何工作的!
date: 2015-07-11 10:42:49
tags:
description: 作为入门者来说，了解JavaScript中timer的工作方式是很重要的。
---
作为入门者来说，了解JavaScript中timer的工作方式是很重要的。通常它们的表现行为并不是那么地直观，而这是因为它们都处在一个单一线程中。让我们先来看一看三个用来创建以及操作timer的函数。
``` javasctipt
var id = setTimeout(fn, delay); 
```
初始化一个单一的timer，这个timer将会在一定延时后去调用指定的函数。这个函数（setTimeout）将返回一个唯一的ID，我们可以通过这个ID来取消timer。
``` javascript
var id = setInterval(fn, delay);
```
与setTimeout类似，只不过它会持续地调用指定的函数（每次都有一个延时），直到timer被取消为止。

``` javascript
clearInterval(id);, clearTimeout(id);
```
接受一个timer的ID（由上述的两个函数返回的），并且停止timer的回调事件。

要弄明白这个定时器内部是怎么工作的，有一个很重要的概念需要被提出来：

1 定时器延迟是不准确的(guaranteed).因为所有的javascript 浏览器代码只有一个单线程执行，并且那些异步事件（如鼠标点击事件，和定时器）只会在出现线程空闲的时候去会执行。这有一个图表演示，如下：
{% asset_image 20150711094408429.png %}

这里有很多信息在这个图中需要去理解，但是完全理解它之后，你会对javascript中异步机制有一个清楚的认识。这个图是一维的:

垂直方向我们用毫秒单位来标记这个时间，这个蓝色的方块代表这个正在被执行的javascript代码。例如，这第一个被执行的javascript代码大约花了18毫秒，这个鼠标事件块大约花费了11毫秒，等等。

由于javascript 引擎永远只会执行一个片段的代码在同一个时间（因为这个单线程机制），那么这时每一个代码块将会阻塞（blocking）别的异步事件的运行。

这意味着当一个异步事件被调用（例如当一个鼠标点击，一个定时器触发firing,或者一个xmlhttprequest 过程完成），它将会被加入到队里，并延迟执行(至于具体如何被入到队列中，不同的浏览器有不同的实现，我们这里只考虑简单的情况)

从一开始，在第一个javasript中，有两个定时器被初始化了： 一个10 毫秒的 setTimeout时间和一个10 毫秒的setInterval事件（这里注意，仅仅只是初始化，亦或叫作定义）。

由于这个定时器开始的时间和位置，导致它们在第一个javascript块完成前就已经真正被调用(这里的调用，并非直接执行，这里需要注意,可以理解为只是准备调用，把该回调方法加入到队列)了。注意，无论怎么样（however），定时器都不会立刻执行(因为线程没有空闲的原因，它没办法直接执行)。相反，这个被延迟的方法会被加入到队列中，在某个可以执行的时刻（线程空闲的时刻）执行。

另外一点，在第一个javascript块中，我们可以看到还有一个鼠标事件被触发了。这个javascript 回调方法被关联到一个异步事件 (没人知道用户什么时候做这个动作，所以它被认为是异步的)，这个异步事件也不会立刻执行，和上面的定时器一样，也会被加入到队列中。

在第一个javascript 块执行结束之后，javascript 引擎就会立刻问一个问题： 还有什么在等待被执行的代码么？ 那么这个时间，有一个鼠标事件回调和定时器回调同时在等待。这个浏览器马上挑选一个(从图中看，是鼠标事件回调)立刻执行。这个定时器继续等待，直到下一个可能的时刻。

注意一点：在这个这个鼠标事件处理函数正在被执行的同时，第一个interval 回调函数也会调用。正如前面提到的定时器一样，它的回调方法会被加入到队列中。然而，注意当这个interval再一次被调用（这个时候这个定时器的回调方法正在被执行），那么这个时候，这个interval 的回调方法将会被删除（drop）。 如果由于主线程需要执行很长时间的代码块，导致你在队列中加入了很多个回调方法，那么当这个主线程结束之后，一连串的回调函数连续执行没有间隔，直到结束。比较好的做法，是暂时让浏览器休息等待一会，让队列中没有Interval回调。

我们在看到一些情况：在第三个interval 回调方法触发的时候，inteval自身正在执行（这里应该是下在执行第二个interval没有结束）。这里给我们展示了一个重要的信息：

interval 不会去关心当前的线程现在执行什么，它们会把自己的回调方法加入到队列中在任何情况下，即使它会让两个间隔的回调方法之间的时间减少。

最后，在第二个interval（图中应该是第三个，这里应为中间有一个被drop掉了）被 执行完之后，javasript引擎中已经没有东西可以用来执行了。这也就是说，浏览器现在正在等一个新的异步事件需要去触发(occur)。在第50毫秒的时候，再一次触发了inteval回调。这个时候，已经没有东西去阻塞它的执行，所以它加入到队列中之后就立刻执行了。

接下来，让我们看一个例子更好的理解setTimeout与setInterval的区别：
``` javascript
setTimeout(function(){
    /* Some long block of code... */
    setTimeout(arguments.callee, 10);
  }, 10);
 
  setInterval(function(){
    /* Some long block of code... */
  }, 10);
```
这两段代码可能在功能的实现上非常的相似，不经意一看，他们是完全一样的。尤其是这个setTimeout代码会在上一个回调函数执行之后至少隔10毫秒再执行一次回调方法（它可能会超过10毫秒，但不会少于10毫秒）。但是setInteval 却会尝试10毫秒就执行一个回调函数，不会去管上一个回调是什么时候执行的。

These two pieces of code may appear to be functionally equivalent, at first glance, but they are not. Notably the setTimeout code will always have at least a 10ms delay after the previous callback execution (it may end up being more, but never less) whereas the setInterval will attempt to execute a callback every 10ms regardless of when the last callback was executed.

这里有一些东西是我们从这里学到的，做一个总结：

1 javascript引擎仅仅只有一个单线程，正在执行的异步事件会加入到队列等待。

2 setTimeout与setInterval 是执行异步回调方法从根本上不一样的。

3 如果一个需要立即执行的定时器被阻塞了，它将被延迟执行，知道下一次线程空闲（那么被延迟的时间会超过定时器定义的时间）

4 interval 可能会没有延迟的连续执行回调方法，如果主线程了执行一个足够长的代码（比定时的延迟长）

所有的这些都是非常重要的知识对于了解javascript引擎是如何工作的。特别是对于大数量的回调事件发生的时候，为我们建立更好的应用代码建立好的基础。

----------------------------------------------------------------------------------------------------

原文出自jQuery的作者John Resig。

地址：http://ejohn.org/blog/how-javascript-timers-work/#postcomment
