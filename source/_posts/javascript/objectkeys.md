---
title: Object.keys 和 Reflect.ownKeys 区别
date: 2022-11-24 23:42:00
description: Object.keys 和 Reflect.ownKeys 区别
---

#### Reflect.ownKeys(target)
返回所有的属性key，包括不可枚举类型，不包括继承的属性

``` javascript
let obj = {
    name: 'yangym',
    age: '20'
}

console.log(Reflect.ownKeys(obj))
```

> Reflect.ownKeys(obj) 和 Object.getOwnPropertyNames(target).concat(Object.getOwnPropertySymbols(target)) 等价

#### Object.keys()
Object.keys(obj) 返回遍历对象可枚举类型的属性，不包括继承的属性

``` javascript
let obj = {
    name: 'yangym',
    age: '20',
    func: function () {
        console.log('test')
    }
}

Object.defineProperty(obj, 'sorce', {
    value: 'programer',
    enumrable: false
})

console.log(Object.keys(obj))
```
#### 区别
- Reflect.ownKeys 返回所有的属性不管是不是可枚举
- Object.keys返 回可枚举的属性