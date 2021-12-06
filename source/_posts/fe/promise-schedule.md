---
title: Promise 请求限流
date: 2021-11-25 17:41:32
tags:
description: 使用 Promise 进行前端限流, 控制并发。
---

### 简单版本

``` javascript
class Scheduler {
  constructor (limit){ 
    this.limit = limit
    this.task = []
    this.count = 0
  }

  add (order, time, fn) {
    this.task.push(() => {
      return new Promise((resolve, reject) => {
        setTimeout(() => {
          resolve(fn)
        }, time)
      })
    })
  }

  run () {
    for (let i = 0; i < this.limit; i++) {
      this.next()
    }
  }

  next () {
    if (!this.task.length || this.count === this.limit) {
      return
    }
    this.count++
    const task = this.task.shift()
    task().then((fn) => {
      this.count --
      fn()
      this.next()
    })
  }
}


const scheduler = new Scheduler(2)

scheduler.add(1, 1000, () => {
  console.log(1)
})
scheduler.add(2, 500, () => {
  console.log(2)
})
scheduler.add(3, 300, () => {
  console.log(3)
})
scheduler.add(4, 400, () => {
  console.log(4)
})

scheduler.run()
```
output:
``` bash
2
3
1
4
```

这里只是使用了 `time` 数字来表示请求时间。但是如果这里的 不是请求时间，而是真实的请求，需要根据请求具体的返回情况来判定是否添加下一个请求进入队列该如何进行处理呢？

### Promisify 版本
等着，哈哈哈