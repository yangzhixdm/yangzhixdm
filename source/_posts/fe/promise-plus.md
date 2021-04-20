---
title: Promise A+ 规范实现
date: 2021-04-20 19:57:32
tags:
description: 最近总结了一下 Promise的一些细节问题，没事最实现了一下 Promise A+ 规范。
---

##### 关于Promise 中then 挂载的任务，为交替执行。
``` javascript
Promise.resolve().then(() => {
    console.log(0);
}).then(() => {
    console.log(1)
})

Promise.resolve().then(() => {
    console.log(2);
}).then(() => {
    console.log(3);
})
```

输出结果为 ` 0 2 1 3`.

原因是因为每次 `then` 方法都是返回了一个新的 `Promise`，那么其实每次resolve的时候只是将当前的 Promise的 `onFulfilled` 推入到微任务队列中。
当前一次的 Promise的 onFulfilled 执行时，才通过 `resolvePromise` 开始执行下一个新的 `Promise`的resolve.

那么如果同时存在多个 Promise的话，那么首次推出到 微任务队列则是同时进入的，就相当于是 一批一批 的进行处理。
但是如果onFulfilled 中的返回值为一个新的`Promise`的话，那么这里就需要涉及到`Promise`的展开问题。但是整体的外层的 `onFulfilled`的执行顺序还是保持不变。

##### Promise 展开问题
``` javascript
Promise.resolve().then(() => {
    console.log(0);
  return new Promise((resolve) => {
    resolve()
  }).then(() => {
    console.log(1)
  }).then(() => {
    console.log(2)
    return new Promise((resolve) => {
      resolve()
    }).then(() => {
      console.log('3')
    })
  })
}).then(() => {
    console.log(4)
})
```
输出结果为  `0 1 2 3 4`

但是上面这个代码如果把 `return` 关键字去掉的话，结果又有点变化:
``` javascript
Promise.resolve().then(() => {
    console.log(0);
  new Promise((resolve) => {
    resolve()
  }).then(() => {
    console.log(1)
  }).then(() => {
    console.log(2)
    return new Promise((resolve) => {
      resolve()
    }).then(() => {
      console.log('3')
    })
  })
}).then(() => {
    console.log(4)
})
```
``` javascript
Promise.resolve().then(() => {
    console.log(0);
  new Promise((resolve) => {
    resolve()
  }).then(() => {
    console.log(1)
  }).then(() => {
    console.log(2)
    new Promise((resolve) => {
      resolve()
    }).then(() => {
      console.log('3')
    })
  })
}).then(() => {
    console.log(4)
})
```
输出结果为 `0 1 4 2 3`

##### PromiseA+ 规范实现
``` javascript
/**
 * construtor
 */
function Promise (executor) {
  this.state = 'pending'
  this.value = null
  this.reason = null
  
  this.onFulfilledCallbacks = []
  this.onRejectedCallbacks = []

  try {
    doResolve(executor, this)
  }  catch (e) {
    this.reject(e)
  }
}

function doResolve(executor, promise) {
  executor(function(value) {
    promise.resolve(value)
  }, function (reason) {
    promise.reject(reason)
  })
}

Promise.prototype.resolve = function (value) {
  if (this.state === 'pending') {
    this.state = 'fulfilled'
    this.value = value
    while (this.onFulfilledCallbacks.length) {
      this.onFulfilledCallbacks.shift()(value)
    }
  }
}

Promise.prototype.reject = function (reason) {
  if (this.state === 'pending') {
    this.state = 'rejected'
    this.reason = reason
    while (this.onRejectedCallbacks.length) {
      this.onRejectedCallbacks.shift()(reason)
    }
  }
}

function resolvePromise(promise, x, resolve, reject) {
  // 如果相等了，说明return的是自己，抛出类型错误并返回
  if (promise === x) {
    return reject(new TypeError('The promise and the return value are the same'));
  }

  if (typeof x === 'object' || typeof x === 'function') {
    // x 为 null 直接返回，走后面的逻辑会报错
    if (x === null) {
      return resolve(x);
    }

    let then;
    try {
      // 把 x.then 赋值给 then 
      then = x.then;
    } catch (error) {
      // 如果取 x.then 的值时抛出错误 error ，则以 error 为据因拒绝 promise
      return reject(error);
    }

    // 如果 then 是函数
    if (typeof then === 'function') {
      let called = false;
      try {
        then.call(
          x, // this 指向 x
          // 如果 resolvePromise 以值 y 为参数被调用，则运行 [[Resolve]](promise, y)
          y => {
            // 如果 resolvePromise 和 rejectPromise 均被调用，
            // 或者被同一参数调用了多次，则优先采用首次调用并忽略剩下的调用
            // 实现这条需要前面加一个变量 called
            if (called) return;
            called = true;
            resolvePromise(promise, y, resolve, reject);
          },
          // 如果 rejectPromise 以据因 r 为参数被调用，则以据因 r 拒绝 promise
          r => {
            if (called) return;
            called = true;
            reject(r);
          });
      } catch (error) {
        // 如果调用 then 方法抛出了异常 error：
        // 如果 resolvePromise 或 rejectPromise 已经被调用，直接返回
        if (called) return;

        // 否则以 error 为据因拒绝 promise
        reject(error);
      }
    } else {
      // 如果 then 不是函数，以 x 为参数执行 promise
      resolve(x);
    }
  } else {
    // 如果 x 不为对象或者函数，以 x 为参数执行 promise
    resolve(x);
  }
}

Promise.prototype.then = function (onFulfilled, onRejected) {

  const realOnFulfilled = typeof onFulfilled === 'function' ? onFulfilled : value => value;
  const realOnRejected = typeof onRejected === 'function' ? onRejected : reason => { throw reason };
  
  const promise2 = new Promise((resolve, reject) => {
    if (this.state == 'fulfilled') {
      setTimeout(() => {
        try {
          const x = realOnFulfilled(this.value)
          resolvePromise(promise2, x, resolve, reject)
        } catch(error) {
          reject(error)
        }
      },0)
    } else if(this.state === 'rejected'){
      setTimeout(() => {
        try {
          const x = realOnRejected(this.reason)
          resolvePromise(promise2, x, resolve, reject)
        } catch (error) {
          reject(error)
        }
      }, 0)
    } else  if(this.state === 'pending'){
      this.onFulfilledCallbacks.push(() => {
        setTimeout(() => {
          try {
            const x = realOnFulfilled(this.value)
            resolvePromise(promise2, x, resolve, reject)
          } catch (error) {
            reject(error)
          }
        }, 0)
      })
      this.onRejectedCallbacks.push(() => {
        setTimeout(() => {
          try {
            const x = realOnRejected(this.reason)
            resolvePromise(promise2, x, resolve, reject)
          } catch (error) {
            reject(error)
          }
        }, 0)
      })
    }
  })

  return promise2
}

Promise.prototype.catch = function (onRejected) {
  return this.then(null, onRejected)
}

/**
 * Promise.resolve
 */
Promise.resolve = function (value) {
  if (value && typeof value === 'object' && typeof value.constructor === Promise) {
    return value
  }

  return new Promise((resolve) => {
    resolve(value)
  })
}

Promise.reject = function (value) {
  return new Promise((resolve, reject) => {
    reject(value)
  })
}

Promise.deferred = function () {
  var result = {}
  result.promise = new Promise((resolve, reject) => {
    result.resolve = resolve
    result.reject = reject
  })

  return result
}
 
module.exports = Promise
```