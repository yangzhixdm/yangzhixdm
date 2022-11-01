---
title: Vue nextTick渲染机制
date: 2019-03-20
tags: Vue
---

#### 有言在先
问题来自官方文档中提到的nextTick，之前一直忙着写组件的东西，没有太关注，现在仔细查了一下Vue的渲染机制

#### Watcher的update
> Vue 利用Object.defineProperty 会对data中的属性进行数据劫持。在访问和设置属性的值时，会相应的触发对应的getter的setter方法。那么在重新设置data中属性值时，会相应的触发存储在Dep中的notify方法，并遍历相应的watcher进行update.
``` javascript
  Dep.prototype.notify = function notify() {
    // stabilize the subscriber list first
    var subs = this.subs.slice();
    for (var i = 0, l = subs.length; i < l; i++) {
      subs[i].update();
    }
  };
```
> 而 watcher中的update则会触发 queueWatcher
将每个watcher压入到队列中，并且在下一个任务（nextTick）时，开始任务调度（flushSchedulerQueue）。其中waiting 默认为false。

``` javascript
  function queueWatcher(watcher) {
    var id = watcher.id;
    if (has[id] == null) {
      has[id] = true;
      if (!flushing) {
        queue.push(watcher);
      } else {
        // if already flushing, splice the watcher based on its id
        // if already past its id, it will be run next immediately.
        var i = queue.length - 1;
        while (i > index && queue[i].id > watcher.id) {
          i--;
        }
        queue.splice(i + 1, 0, watcher);
      }
      // queue the flush
      if (!waiting) {
        waiting = true;
        nextTick(flushSchedulerQueue);
      }
    }
  }
```
> flushSchedulerQueue中主要执行watcher中run方法，执行更新，并且重置调度状态resetSchedulerState。
所以这里要考虑的时，数据的更新其实是在下一个nextTick中进行处理。
那么需要考虑的是nextTick中做了什么?
在 Vue.js 里是数据驱动视图变化，由于 JS 执行是单线程的，在一个 tick 的过程中，它可能会多次修改数据，但 Vue.js 并不会傻到每修改一次数据就去驱动一次视图变化，它会把这些数据的修改全部 push 到一个队列里，然后内部调用 一次 nextTick 去更新视图，所以数据到 DOM 视图的变化是需要在下一个 tick 才能完成。nextTick 采取的策略是默认走 micro task，对于一些 DOM 交互事件，如 v-on 绑定的事件回调函数的处理，会强制走 macro task。更新任务其实是在当前代码执行之后的下一个事件循环之后才进行更新。
Vue.js 在绑定 DOM 事件的时候，默认会给回调的 handler 函数调用 withMacroTask 方法做一层包装，它保证整个回调函数执行过程中，遇到数据状态的改变，这些改变都会被推到 macro task 中。
对于 macro task 的执行，Vue.js 优先检测是否支持原生 setImmediate，这是一个高版本 IE 和 Edge 才支持的特性，不支持的话再去检测是否支持原生的 MessageChannel，如果也不支持的话就会降级为 setTimeout 0。
``` javascript
  function nextTick(cb, ctx) {
    var _resolve;
    callbacks.push(function () {
      if (cb) {
        try {
          cb.call(ctx);
        } catch (e) {
          handleError(e, ctx, 'nextTick');
        }
      } else if (_resolve) {
        _resolve(ctx);
      }
    });
    if (!pending) {
      pending = true;
      if (useMacroTask) {
        macroTimerFunc();
      } else {
        microTimerFunc();
      }
    }
    // $flow-disable-line
    if (!cb && typeof Promise !== 'undefined') {
      return new Promise(function (resolve) {
        _resolve = resolve;
      });
    }
  }
```
而这个macroTimerFunc中则是使用了setImmediate/MessageChannel /setTimeout进行实现。
``` javascript
  if (typeof setImmediate !== 'undefined' && isNative(setImmediate)) {
    macroTimerFunc = function macroTimerFunc() {
      setImmediate(flushCallbacks);
    };
  } else if (typeof MessageChannel !== 'undefined' && (isNative(MessageChannel) ||
  // PhantomJS
  MessageChannel.toString() === '[object MessageChannelConstructor]')) {
    var channel = new MessageChannel();
    var port = channel.port2;
    channel.port1.onmessage = flushCallbacks;
    macroTimerFunc = function macroTimerFunc() {
      port.postMessage(1);
    };
  } else {
    /* istanbul ignore next */
    macroTimerFunc = function macroTimerFunc() {
      setTimeout(flushCallbacks, 0);
    };
  }
```
``` javascript
  // Determine microtask defer implementation.
  /* istanbul ignore next, $flow-disable-line */
  if (typeof Promise !== 'undefined' && isNative(Promise)) {
    var p = Promise.resolve();
    microTimerFunc = function microTimerFunc() {
      p.then(flushCallbacks);
      // in problematic UIWebViews, Promise.then doesn't completely break, but
      // it can get stuck in a weird state where callbacks are pushed into the
      // microtask queue but the queue isn't being flushed, until the browser
      // needs to do some other work, e.g. handle a timer. Therefore we can
      // "force" the microtask queue to be flushed by adding an empty timer.
      if (isIOS) {
        setTimeout(noop);
      }
    };
  } else {
    // fallback to macro
    microTimerFunc = macroTimerFunc;
  }
```
#### 常见问题
```
<templete>
<div>
  <span id="content">{{a}}</span>
</div>
</template>
new Vue({
  data() {
    return {
      a: 1
   }
  created(){
    this.a =1;
    console.log(document.getElementById('content').innerHTML;)
  }
));
```