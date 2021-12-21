---
title: compose机制
date: 2021-12-15 10:38:46
tags: node
description: 关于Koa中对 compose 的实现以及对 compose 的简单实现。
---

### 有言在先
Koa中 koa-compose 主要用于对 middleware也就是中间件进行 compose处理，然后在请求过程中进行调用，实现洋葱模型。

### Koa-compose

``` javascript
function compose (middleware) {
  if (!Array.isArray(middleware)) throw new TypeError('Middleware stack must be an array!')
  for (const fn of middleware) {
    if (typeof fn !== 'function') throw new TypeError('Middleware must be composed of functions!')
  }

  /**
   * @param {Object} context
   * @return {Promise}
   * @api public
   */

  return function (context, next) {
    // last called middleware #
    let index = -1
    return dispatch(0)
    function dispatch (i) {
      if (i <= index) return Promise.reject(new Error('next() called multiple times'))
      index = i
      let fn = middleware[i]
      if (i === middleware.length) fn = next
      if (!fn) return Promise.resolve()
      try {
        return Promise.resolve(fn(context, dispatch.bind(null, i + 1)));
      } catch (err) {
        return Promise.reject(err)
      }
    }
  }
}
```
### Core
核心代码:

``` javascript
Promise.resolve(fn(context, dispatch.bind(null, i + 1)));
```

1 将 结果 进行 Promise.resovle;
2 利用bind进行递归调用，实现 compose;
3 此处的 middleware 执行顺序为`从左往右`执行；

``` javascript
const func = compose([function(context, next){
  console.log('1')
  next()
}, function(context, next) {
  console.log(2)
  console.log(next)
  next()
}])

func() // 1 2
```

当然这里也存在问题，比如 这里的中间件是不支持参数传递的，如果要传递参数，就得把参数挂在 `ctx` 上。

### redux-compose
``` javascript
export default function compose(...funcs) {
  if (funcs.length === 0) {
    return arg => arg
  }

  if (funcs.length === 1) {
    return funcs[0]
  }

  return funcs.reduce((a, b) => (...args) => a(b(...args)))
}
```

### one more things
另外想多说一点，在调用reduce方式的时候，其实如果：
1 当 数组的长度为 1 时候，并且 `initValue` 没有被指定，那么就会直接返回数组的第一个 `value`

``` javascript
[() => { console.log('test')}].reduce(() => { return 'callback'}) // 会直接返回 () => { console.log('test')}
```

2 但是如果 initValue 已经被指定，那么就会调用 callbackn
``` javascript
[() => { console.log('test')}].reduce(() => { return 'callback'}, 1) // 执行callbackfn, 并返回 'callback'
```

具体可以参考 ecma 对 reduce 的解释:  [Ecma-script Reduce](https://262.ecma-international.org/6.0/#sec-array.prototype.reduce)

### 所以
上面 redux对 compose的方法实现中，对于 `funs.length === 1` 的处理，其实是可以直接去掉的。