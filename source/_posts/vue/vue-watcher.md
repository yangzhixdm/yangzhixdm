---
title: Vue 中 Watcher 的一些想法
date: 2019-08-20 14:22:55
tags: Vue
description: Vue 源码篇中对于Watcher的一些想法。
---

#### 将data返回的对象绑定 `__ob__`  属性

data中每一个属性都会在 `defineReactive` 的时候，在闭包中绑定一个Dep实例，该实例主要用于在subs属性中存储对应引用的Watcher实例。
首先该data返回的对象（注意是data返回的对象）会被 `observe()` 方法给它自身新创建一个 `__ob__`属性，`__ob__` 属性其实是 `Observer` 的实例。
Observer类中，主要保存了两个属性：`id`, `dep`,这里dep也就是 Dep的实例。
``` javascript
export class Observer {
  constructor (value: any) {
    this.value = value
    this.dep = new Dep()
    this.vmCount = 0
    def(value, '__ob__', this)
    if (Array.isArray(value)) {
      if (hasProto) {
        protoAugment(value, arrayMethods)
      } else {
        copyAugment(value, arrayMethods, arrayKeys)
      }
      this.observeArray(value)
    } else {
      this.walk(value)
    }
  }

  /**
   * Walk through all properties and convert them into
   * getter/setters. This method should only be called when
   * value type is Object.
   */
  walk (obj: Object) {
    const keys = Object.keys(obj)
    for (let i = 0; i < keys.length; i++) {
      defineReactive(obj, keys[i])
    }
  }

  /**
   * Observe a list of Array items.
   */
  observeArray (items: Array<any>) {
    for (let i = 0, l = items.length; i < l; i++) {
      observe(items[i])
    }
  }
}
```

#### 遍历属性
接下来如果不是对象的话，那么就开始walk遍历属性，逐一绑定 `响应式`。
这里的核心逻辑，在该属性被访问时（一般是在render的时候，如果存在依赖，那么就会被访问），在getter方法中会保留dep的依赖，那么dep将会被封闭在闭包中。
通过调用 `dep.depend()` 将当前Dep.target进行添加到dep的subs中。（这里Dep.target就是正在被解析的 Watcher).

> 这里还会进行判断，当前属性是否是一个对象。主要通过 `observe` 方法进行判断。因为如果给元素是一个对象，那么 `observe` 方法就会返回一个 Observer 实例对象，否则 返回 void。
> 如果属性为对象，那么在 getter中需要对 属性进行重新处理。
> 上面说到如果 属性是一个对象，就会返回一个 Observer 实例，赋值给 childOb ，并在getter方法中进行引用，那么 childOb 就也被封闭在闭包中了。
> 然后将 当前的 Dep.target 添加大 childOb.dep的 subs中保存。

``` javascript
export function defineReactive (
  obj: Object,
  key: string,
  val: any,
  customSetter?: ?Function,
  shallow?: boolean
) {
  const dep = new Dep()

  const property = Object.getOwnPropertyDescriptor(obj, key)
  if (property && property.configurable === false) {
    return
  }

  // cater for pre-defined getter/setters
  const getter = property && property.get
  const setter = property && property.set
  if ((!getter || setter) && arguments.length === 2) {
    val = obj[key]
  }

  let childOb = !shallow && observe(val)
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
}
```
#### 绑定关系

这里需要注意的是 `dep.depend` 方法不只是将 watcher 绑定到了 dep的subs中。其实还做了另外一件事情。
`dep.depend` 方法其实还调用了 watcher的 addDeps方法。也即是将当前的这个dep引用加入到了watcher的deps队列中，当然这里面做了一层去重操作。
这里的作用是在日后我们需要通过watcher的监听进行更新的时候，可以轻易的找到对应的dep然后进行更新就可以了，另外上面所提到的 childOb也是可以在watcher的deps中进行找到，然后进行更新。
如果没有这一层关系的话，那么上面的dep或者childDep就没有办法被引用了。
``` javascript
addDep (dep: Dep) {
  const id = dep.id
  if (!this.newDepIds.has(id)) {
    this.newDepIds.add(id)
    this.newDeps.push(dep)
    if (!this.depIds.has(id)) {
      dep.addSub(this)
    }
  }
}
```

这里可能需要再强调一点，dep和watcher的关系，他们之前其实是一个多对多的关系。
- 1 一个属性对应一个 dep，如果存在对象属性，则使用childOb，重新创建一个dep.
- 2 一个dep的subs中可以包含多个 watcher，如果这个属性被多次引用的话,简单一点：`属性可能被renderWatcher引用，也可能被computedWatcher引用，也可能被userWatcher进行监听`。
- 3 一个watcher的deps中可以包含多个dep。比如renderWatcher中会引用多个属性，那么在renderWatcher中的deps中就会保存多个dep（导致的问题就是任意一个属性的变化都会导致重新render）。一个computed属性依赖多个data属性，那么就触发data属性的getter的时候，就会将依赖的属性的dep放入到当前的Dap.tareget(computedWatcher)的dep中，导致watcher.deps中会存在多个dep.

{% asset_image 2021-01-11_235255.png %}

#### 更新
关于更新可以看下一篇 [更新文档](http://localhost:4000/2019/08/20/vue/vue-update/)。

#### 总结

关于Vue中 watcher和 dep的关系，以及 watcher的批量更新机制，思考了很长一段时间，最终绘制成一张图，如下：

{% asset_image 20210111758443315.png %}
