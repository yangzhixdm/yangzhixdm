---
title: Vue源码-Vue响应式绑定过程
date: 2019-08-18 14:17:53
tags: Vue
description: 关于Vue的响应式绑定过程细探。
---

### defineReactive
绑定数据响应式， 对于每个属性创建一个Dep对象实例。并且observe元素判定子元素是否需要observe,如果需要则递归进行响应式
``` javascript
export function defineReactive (
  obj: Object,
  key: string,
  val: any,
  customSetter?: ?Function,
  shallow?: boolean
) {
  console.log('defineReactive: 定义响应式')
  // 创建Dep 属性收集器
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
  console.log("defineReactive key:", key)

  let childOb = !shallow && observe(val)
  // 这里只是定义，但是不一定会被调用
  // 如果是内部属性，则没有Dep.target,只会获取属性
  Object.defineProperty(obj, key, {
    enumerable: true,
    configurable: true,
    get: function reactiveGetter () {
      const value = getter ? getter.call(obj) : val
      // 此处的get只有Watcher调用get方法才会被调用
      // 其他内部的方法则不会被调用
      if (Dep.target) {
      dep.depend() // 添加依赖
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