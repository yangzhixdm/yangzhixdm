---
title: Promisify机制。
date: 2021-11-25 15:19:54
tags: node
description: 其实这也不是什么新鲜的东西，简单点说就是把一个异步的玩意变成同步的东西。基本目的就是后续可以和async await 进行配合然后实现 同步的效果。
---

### 啥玩意
其实这也不是什么新鲜的东西，简单点说就是把一个异步的玩意变成同步的东西。基本目的就是后续可以和async await 进行配合然后实现 同步的效果。

### 为啥
类似读取文件这种玩意，你要是直接写就得像下面这样：
``` javascript
const fs = require('fs')
fs.readFile('./text.js', (err, data) => {
  console.log(this)
})
```
说实话，这种 回调 的写法真的是挺痛苦的，但是呢，又不想想用同步的readFileSync, 毕竟太好性能，再说了，也不是任何时候都有同步的方法给你用的。

如果要同步，可能就要自己去操作了。手动的方式,那就整个promise呗。

``` javascript
const fs = require('fs')
const readFileAsyncCustom = function () => {
  return new Promise((reslove, reject) => {
    fs.readFile('./text.js', (err, data) => {
      if (err) {
        reject(err)
      } else {
        resolve(data)
      }
    })
  })
}
```
这样下次用的时候就可以直接用 `await` 了，美滋滋。
``` javascript
async () => {
  const data = await readFileAsyncCustom
}
```

但是问题是，这特么也太麻烦了吧，下次再来一个方法，我都要再写一次？ 也太恶心了吧。

### 怎么干
好吧，那竟然就是封装个 `Promise` 而已，那就整个通用的吧，管你是啥，都用一个方法给封装起来。
``` javascript
const promisify = function (fn) {
  return function () {
    const args = arguments
    const context = this
    return new Promise((resolve, reject) => {
      fn.apply(context, [...args,(err, data) => {
        if (err) {
          reject(err)
        } else {
          resolve(data)
        }
      }])
    })
  }
}
```
把你的方法 传入 近来，封装一个 高阶 函数，该函数会返回一个 Promise 方法， 并且在 传入的fn 执行完成之后才 进行 `决议`(resolve or reject).
东西有了，怎么用？

``` javascript
(async () => {
  const promisifyReadFile = promisify(fs.readFile)
  const file = await promisifyReadFile('./text.js')
  console.log(file.toString())
})()
```
美滋滋。

see you again