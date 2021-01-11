---
title: Vue 批量更新机制细谈
date: 2019-08-20 14:22:55
tags: Vue
description: 关于 Vue 中组件批量更新机制的探索，以及自身的一些认识。
---
####  Vue 更新机制

其实在Vue中更新机制采用的批量更新，配合发布订阅模式，以及事件循环机制实现。
首先其中涉及到响应式的一些知识。
每一个响应式属性，在被绑定响应式时，都会通过`Object.defineProperty`进行劫持，然后确定dep以及 watcher的关系。
``` javascript
  Object.defineProperty(obj, key, {
    enumerable: true,
    configurable: true,
    get: function reactiveGetter () {
      const value = getter ? getter.call(obj) : val
      if (Dep.target) {
        dep.depend()
        if (childOb) {
          childOb.dep.depend()
          if (Array.isArray(value)) {
            dependArray(value)
          }
        }
      }
      return value
    },
    set: function reactiveSetter (newVal) {
      const value = getter ? getter.call(obj) : val
      /* eslint-disable no-self-compare */
      if (newVal === value || (newVal !== newVal && value !== value)) {
        return
      }
      /* eslint-enable no-self-compare */
      if (process.env.NODE_ENV !== 'production' && customSetter) {
        customSetter()
      }
      // #7981: for accessor properties without setter
      if (getter && !setter) return
      if (setter) {
        setter.call(obj, newVal)
      } else {
        val = newVal
      }
      childOb = !shallow && observe(newVal)
      dep.notify()
    }
  })
```
这里需要注意的是 dep其实会被存入在 闭包中。在触发getter或者setter是进行获取dep.
从而实现一个属性对应一个dep.而一个dep可以对应多个watcher.毕竟一个属性可以被多处依赖。比如 renderWatcher，LazyWatcher,userWatcher等。
如果属性存在变动，则会让dep对象的subs中的所有watcher都进行更新。
``` javascript
// dep.js
notify () {
  // stabilize the subscriber list first
  const subs = this.subs.slice()
  if (process.env.NODE_ENV !== 'production' && !config.async) {
    // subs aren't sorted in scheduler if not running async
    // we need to sort them now to make sure they fire in correct
    // order
    subs.sort((a, b) => a.id - b.id)
  }
  for (let i = 0, l = subs.length; i < l; i++) {
    subs[i].update()
  }
}
```
``` javascript
// wathcer.js
update () {
  /* istanbul ignore else */
  if (this.lazy) {
    this.dirty = true
  } else if (this.sync) {
    this.run()
  } else {
    queueWatcher(this)
  }
}
```

这里判定如果是lazy watcher 也就是计算属性wathcer，那么将dirty设置为true,从而实现缓存的概念。也就是不再重复计算computed属性。
如果设置了sync,则直接执行，用于userWatcher.
除了上述情况，则将该watcher压入到更新队列，然后执行调用 nextTick 方法进行更新。
通俗来说就是在下一个微任务时序进行更新。

``` javascript
export function queueWatcher (watcher: Watcher) {
  const id = watcher.id
  if (has[id] == null) {
    has[id] = true
    if (!flushing) {
      queue.push(watcher)
    } else {
      // if already flushing, splice the watcher based on its id
      // if already past its id, it will be run next immediately.
      let i = queue.length - 1
      while (i > index && queue[i].id > watcher.id) {
        i--
      }
      queue.splice(i + 1, 0, watcher)
    }
    // queue the flush
    if (!waiting) {
      waiting = true

      if (process.env.NODE_ENV !== 'production' && !config.async) {
        flushSchedulerQueue()
        return
      }
      nextTick(flushSchedulerQueue)
    }
  }
}
```
#### 关于 queueWatcher 的流程。
先进行判断watcher.id是否已经被缓存过，如果没有的话，则将其压入到队列中。
然后判断是否在waiting中，如果没有在waiting中，则立马设置现在waiting标志符为true，并调用nextTick。
这里设置nextTick只是设置在下一个时序的时候调用一下flushSchedulerQueue,
而在flushSchedulerQueue中，会将queue中的队列进行依次执行。

#### waiting
需要注意的是，当第一次waiting设置为true之后，后续如果还有其他的watcher 被压入到队列中，下面的代码并不会被执行。
``` javascript
if (!waiting) {
  waiting = true

  if (process.env.NODE_ENV !== 'production' && !config.async) {
    flushSchedulerQueue()
    return
  }
  nextTick(flushSchedulerQueue)
}
```
直到flushSchedulerQueue方法被执行完成之后，waiting被才会被重置回来。

#### flushing
关于flushing字段，用于在watcher需要被更新时，判断当前是否正在进行批量更新操作。
如果没有的话，则直接压入queue队列即可。
如果正在进行更新操作，那么需要根据目前的watcher的id压入到queue队列中.
具体逻辑则是根据其id的大小，压入到queue中最后一个wathcer的id大于当前watcher的id的前面。
``` javascript
let i = queue.length - 1
while (i > index && queue[i].id > watcher.id) {
  i--
}
queue.splice(i + 1, 0, watcher)
```

这里的 index其实是正在执行 watcher在队列中的index.默认情况 index为 0。
``` javascript
i > index && queue[i].id > watcher.id
```
表示必须是在 已经执行过的wathcer的后面，并且是 watcher.id 刚号第一次大于queue.id的位置。如果不存在这样的位置，则直接在最后的位置加上当前的watcher.