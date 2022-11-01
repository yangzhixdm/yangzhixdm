---
title: JavaScript限流
date: 2020-12-03
tags:
---

#### 有言在先
朋友偶然给了一道题目讨论一下，关于使用 js 进行请求限制流量的问题，似乎也是经常面试被问题到的问题。

#### 实现

虽然实现有点难看，但是总算是实现了，而且对与promise和async/await也有一些新的理解。
- 1 async 方法会返回一个Promise
- 2 await 会对Promise进行暂停，如果resolve没有被执行的话，也就是promise需要决议，才会进行下一步（即使这个决议在别处）。
- 3 Promise的then方法如果返回的是一个Promise，那么Promise会被展开
- 4 async 方法如果没有返回值，那么自动返回一个新的Promise，如果有promise返回，那么就会被展开，返回指定的Promise

``` javascript
// js 限流器
class Scheduler {
  
  constructor () {
    this.count = 0
    this.waiting = []
  }

  async add (fn) {
    // 超过
    let promise
    if (this.count >= 2) {
      promise = new Promise(resolve => {
        fn.resolve = resolve
        this.waiting.push(fn)
      })
    } else {
      promise = this.run(fn)
    }
    return promise
  }

  run (fn) {
    this.count ++
    let promise = fn().then(() => {
      this.count --
      console.log('len:', this.waiting.length, `ms: ${new Date().getTime() - start}`)
      if(this.waiting.length > 0) {
        const task = this.waiting.shift()
        // const taksPromise = task()
        //taksPromise.then(() => {
          this.run(task).then(() => {
            task.resolve()
          })
        // })
      }
    })
    return promise
  }
}

const scheduler = new Scheduler()

const timeout = (time) => {
  return new Promise(r => {
    console.log('setTimeout', time)
    setTimeout(r, time)
  })
}

const addTask = (time, order) => {
  scheduler.add(() => {
    return timeout(time)
  }).then(() => {
    console.log(order, `ms: ${new Date().getTime() - start}`)
  })
}

let start = new Date().getTime()

addTask(1000, 1)
addTask(500, 2)
addTask(300, 3)
addTask(400, 4)
```
