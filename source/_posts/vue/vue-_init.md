---
title: Vue源码-Vue-init方法的执行过程
date: 2019-08-18 12:08:30
tags: Vue
description: Vue init 过程。
---

#### resolveConstructorOptions
``` javascript
if (options && options._isComponent) {
  // optimize internal component instantiation
  // since dynamic options merging is pretty slow, and none of the
  // internal component options needs special treatment.
  initInternalComponent(vm, options)
} else {
  vm.$options = mergeOptions(
    resolveConstructorOptions(vm.constructor),
    options || {},
    vm
  )
}
```
获取Ctor中默认的options属性，components, directives, filters, _base(Vue)
``` javascript
export function resolveConstructorOptions (Ctor: Class<Component>) {
  console.log('处理(resolve)构造器的options,构造器默认自带的options属性')
  let options = Ctor.options  // 默认的options 
  if (Ctor.super) {
    console.log('Ctor存在super')
    const superOptions = resolveConstructorOptions(Ctor.super)
    const cachedSuperOptions = Ctor.superOptions
    if (superOptions !== cachedSuperOptions) {
      // super option changed,
      // need to resolve new options.
      Ctor.superOptions = superOptions
      // check if there are any late-modified/attached options (#4976)
      const modifiedOptions = resolveModifiedOptions(Ctor)
      // update base extend options
      if (modifiedOptions) {
        extend(Ctor.extendOptions, modifiedOptions)
      }
      options = Ctor.options = mergeOptions(superOptions, Ctor.extendOptions)
      if (options.name) {
        options.components[options.name] = Ctor
      }
    }
  }
  return options
}
```
mergeOptions,首先调用resolveConstructorOptions,处理构造方法options
将Vue默认构造器的options,赋值给当前的options属性，并且判定super属性

#### mergeOptions
合并options，并且本地化（normalize) props,inject,directives
``` javascript
normalizeProps(child, vm)
normalizeInject(child, vm)
normalizeDirectives(child)
```
合并属性，并且合并Field字段策略
``` javascript
export function mergeOptions () {
  // ...
  const options = {}
  let key
  for (key in parent) {
    mergeField(key)
  }
  for (key in child) {
    if (!hasOwn(parent, key)) {
      mergeField(key)
    }
  }
  function mergeField (key) {
    const strat = strats[key] || defaultStrat
    console.log('调用strat策略方法')
    options[key] = strat(parent[key], child[key], vm, key)
  }
  return options
}
```
#### strats

strats中定义属性的不同调用方式。 Vue内部对不同的字段设定了不同的策略。比如对于data方法的策略：
``` javascript
strats.data = function (
  parentVal: any,
  childVal: any,
  vm?: Component
): ?Function {
  if (!vm) {
    if (childVal && typeof childVal !== 'function') {
      process.env.NODE_ENV !== 'production' && warn(
        'The "data" option should be a function ' +
        'that returns a per-instance value in component ' +
        'definitions.',
        vm
      )

      return parentVal
    }
    return mergeDataOrFn(parentVal, childVal)
  }

  return mergeDataOrFn(parentVal, childVal, vm)
}
```

watch 的strats策略

``` javascript
strats.watch = function (
  parentVal: ?Object,
  childVal: ?Object,
  vm?: Component,
  key: string
): ?Object {
  // work around Firefox's Object.prototype.watch...
  if (parentVal === nativeWatch) parentVal = undefined
  if (childVal === nativeWatch) childVal = undefined
  /* istanbul ignore if */
  if (!childVal) return Object.create(parentVal || null)
  if (process.env.NODE_ENV !== 'production') {
    assertObjectType(key, childVal, vm)
  }
  if (!parentVal) return childVal
  const ret = {}
  extend(ret, parentVal)
  for (const key in childVal) {
    let parent = ret[key]
    const child = childVal[key]
    if (parent && !Array.isArray(parent)) {
      parent = [parent]
    }
    ret[key] = parent
      ? parent.concat(child)
      : Array.isArray(child) ? child : [child]
  }
  return ret
}

/**
 * Other object hashes.
 */
strats.props =
strats.methods =
strats.inject =
strats.computed = function (
  parentVal: ?Object,
  childVal: ?Object,
  vm?: Component,
  key: string
): ?Object {
  if (childVal && process.env.NODE_ENV !== 'production') {
    assertObjectType(key, childVal, vm)
  }
  if (!parentVal) return childVal
  const ret = Object.create(null)
  extend(ret, parentVal)
  if (childVal) extend(ret, childVal)
  return ret
}
strats.provide = mergeDataOrFn
```
对于data, props，以及watch都有不同的策略方案。
data方案调用 mergeDataOrFn 方法
``` javascript
export function mergeDataOrFn (
  parentVal: any,
  childVal: any,
  vm?: Component
): ?Function {
  console.log('mergeDataOrFn: 合并data或者function')
  if (!vm) {
    // in a Vue.extend merge, both should be functions
    if (!childVal) {
      return parentVal
    }
    if (!parentVal) {
      return childVal
    }
    // when parentVal & childVal are both present,
    // we need to return a function that returns the
    // merged result of both functions... no need to
    // check if parentVal is a function here because
    // it has to be a function to pass previous merges.
    return function mergedDataFn () {
      return mergeData(
        typeof childVal === 'function' ? childVal.call(this, this) : childVal,
        typeof parentVal === 'function' ? parentVal.call(this, this) : parentVal
      )
    }
  } else {
    return function mergedInstanceDataFn () {
      // instance merge
      const instanceData = typeof childVal === 'function'
        ? childVal.call(vm, vm)
        : childVal
      const defaultData = typeof parentVal === 'function'
        ? parentVal.call(vm, vm)
        : parentVal
      if (instanceData) {
        return mergeData(instanceData, defaultData)
      } else {
        return defaultData
      }
    }
  }
}
```

``` javascript
function mergeData (to: Object, from: ?Object): Object {
  console.log("mergeData: 合并data, 主要用于mixin以及extend会涉及到data数据的合并")
  if (!from) return to
  let key, toVal, fromVal

  const keys = hasSymbol
    ? Reflect.ownKeys(from)
    : Object.keys(from)

  for (let i = 0; i < keys.length; i++) {
    key = keys[i]
    // in case the object is already observed...
    if (key === '__ob__') continue
    toVal = to[key]
    fromVal = from[key]
    if (!hasOwn(to, key)) {
      set(to, key, fromVal)
    } else if (
      toVal !== fromVal &&
      isPlainObject(toVal) &&
      isPlainObject(fromVal)
    ) {
      mergeData(toVal, fromVal)
    }
  }
  return to
}
```

#### initProxy
设置Prxoy,设置代理属性

#### initLifecycle
初始化生命周期属性
``` javascript
export function initLifecycle (vm: Component) {
  const options = vm.$options
  console.log('initLifecycle生命周期')

  // locate first non-abstract parent
  let parent = options.parent
  if (parent && !options.abstract) {
    while (parent.$options.abstract && parent.$parent) {
      parent = parent.$parent
    }
    parent.$children.push(vm)
  }

  vm.$parent = parent
  vm.$root = parent ? parent.$root : vm

  vm.$children = []
  vm.$refs = {}

  vm._watcher = null
  vm._inactive = null
  vm._directInactive = false
  vm._isMounted = false
  vm._isDestroyed = false
  vm._isBeingDestroyed = false
  console.log('初始化设置一堆属性,$children,$refs,__watcher, _inactive,_directInactive, _isMounted,_isDestroyed,_isBeingDestroyed')
}
```

#### initEvents
初始化events,此处会设置listener属性
``` javascript
export function initEvents (vm: Component) {
  console.log('初始化事件')
  vm._events = Object.create(null)
  vm._hasHookEvent = false
  // init parent attached events
  console.log('根据_parentListeners设置listeners')
  const listeners = vm.$options._parentListeners
  if (listeners) {
    updateComponentListeners(vm, listeners)
  }
}
```
#### initRender
初始化_c和$createElement方法，_c方法主要用于内部调用，包括内部编译生成的代码会使用_c方法，而$createElemnt主要用于外部调用，供开发者调用，然后设置$attrs和$listeners
``` javascript
export function initRender (vm: Component) {
  console.log('initRender')
  console.log('设置vm._vnode, _staticTrees')
  vm._vnode = null // the root of the child tree
  vm._staticTrees = null // v-once cached trees
  const options = vm.$options
  const parentVnode = vm.$vnode = options._parentVnode // the placeholder node in parent tree
  // parentNode渲染上下文
  const renderContext = parentVnode && parentVnode.context
  // 设置插槽resolveSlots(options._renderChildren, renderContext)
  vm.$slots = resolveSlots(options._renderChildren, renderContext)
  vm.$scopedSlots = emptyObject
  // 绑定createElement方法到this实例，以便我们可以获取到恰当的上下文在内部，参数顺序：tag, data, children, normalizationType, alwaysNormalize,私有内部使用用来渲染从模版编译过来的functions
  vm._c = (a, b, c, d) => createElement(vm, a, b, c, d, false)
  // 标准化createElement方法，用来对外公开，被使用在用户自己写的render方法中
  vm.$createElement = (a, b, c, d) => createElement(vm, a, b, c, d, true)
  // $attr和 $listeners被暴露出来用于更加方便的高阶组件创建，他们需要被加入响应式，以便与高阶组件使用他们总是被更新
  const parentData = parentVnode && parentVnode.data

  // 设定$attrs和$listeners
  /* istanbul ignore else */
  if (process.env.NODE_ENV !== 'production') {
    defineReactive(vm, '$attrs', parentData && parentData.attrs || emptyObject, () => {
      !isUpdatingChildComponent && warn(`$attrs is readonly.`, vm)
    }, true)
    defineReactive(vm, '$listeners', options._parentListeners || emptyObject, () => {
      !isUpdatingChildComponent && warn(`$listeners is readonly.`, vm)
    }, true)
  } else {
    defineReactive(vm, '$attrs', parentData && parentData.attrs || emptyObject, null, true)
    defineReactive(vm, '$listeners', options._parentListeners || emptyObject, null, true)
  }
}
```
然后触发beforeCreate生命周期
#### initInjections
初始化inject
#### initState
初始化data，绑定watcher,并且初始化props，methods，data，以及computed属性，以及watch属性，如果存在data就调用initData，进行初始化，否则就observe一个空data
``` javascript
export function initState (vm: Component) {
  vm._watchers = []
  const opts = vm.$options
  // ininitMethods: 初始化props
  if (opts.props) initProps(vm, opts.props)
  // ininitMethods: 初始化methods
  if (opts.methods) initMethods(vm, opts.methods)
  // ininitData: 初始化Data
  if (opts.data) {
    initData(vm)
  } else {
    observe(vm._data = {}, true /* asRootData */)
  }
  // initComputed: 计算属性
  if (opts.computed) initComputed(vm, opts.computed)
  // inWatch: Watch属性初始化
  if (opts.watch && opts.watch !== nativeWatch) {
    initWatch(vm, opts.watch)
  }
}
```
##### initProps
将props属性纳入到响应式属性，并且将props属性代理到vm的_props属性中
``` javascript
defineReactive(props, key, value, () => {
  if (!isRoot && !isUpdatingChildComponent) {
    warn(
      `Avoid mutating a prop directly since the value will be ` +
      `overwritten whenever the parent component re-renders. ` +
      `Instead, use a data or computed property based on the prop's ` +
      `value. Prop being mutated: "${key}"`,
      vm
    )
  }
})
// ...

if (!(key in vm)) {
  proxy(vm, `_props`, key)
}
```

##### initMethods
initMethods首先检测是否重名，然后设定bind，给methods方法绑定this
``` javascript
vm[key] = typeof methods[key] !== 'function' ? noop : bind(methods[key], vm)
```
##### initData 
主要干了几件事
1 获取data对应的strats中的策略方法
2 根据策略获取对应的data
3 获取props以及methods，然后判定是否存在重复，然后data属性进行代理到vm的_data中
4 将data属性进行响应式
``` javascript
function initData (vm: Component) {
  let data = vm.$options.data
  console.log('此处的data获取到根据内部策略合并之后的data方法', data)
  data = vm._data = typeof data === 'function'
    ? getData(data, vm)
    : data || {}
  
  ...

  const keys = Object.keys(data)
  const props = vm.$options.props
  
  const methods = vm.$options.methods
  let i = keys.length
  debugger
  while (i--) {
    const key = keys[i]
    if (process.env.NODE_ENV !== 'production') {
      if (methods && hasOwn(methods, key)) {
        warn(
          `Method "${key}" has already been defined as a data property.`,
          vm
        )
      }
    }
    if (props && hasOwn(props, key)) {
      process.env.NODE_ENV !== 'production' && warn(
        `The data property "${key}" is already declared as a prop. ` +
        `Use prop default value instead.`,
        vm
      )
    } else if (!isReserved(key)) {
      console.log('initData: 设置data代理到vm._data')
      proxy(vm, `_data`, key)
    }
  }
  // observe data
  console.log('initData: observe(data)，纳入响应式')
  observe(data, true /* asRootData */)
}
```
tips: Vue会将prop以及watch中的属性合并到data中 ？ // 需要确定
##### initComputed
设置computed，并且为每个computed属性绑定一个watcher对象，然后对computed属性进行响应式，defineComputed中会利用缓存
``` javascript
function initComputed (vm: Component, computed: Object) {
  // $flow-disable-line
  const watchers = vm._computedWatchers = Object.create(null)
  // computed properties are just getters during SSR
  const isSSR = isServerRendering()

  for (const key in computed) {
    const userDef = computed[key]
    const getter = typeof userDef === 'function' ? userDef : userDef.get
    if (process.env.NODE_ENV !== 'production' && getter == null) {
      warn(
        `Getter is missing for computed property "${key}".`,
        vm
      )
    }

    if (!isSSR) {
      // create internal watcher for the computed property.
      console.log('为每个计算属性创建一个Watcher')
      watchers[key] = new Watcher(
        vm,
        getter || noop,
        noop,
        computedWatcherOptions
      )
    }

    // component-defined computed properties are already defined on the
    // component prototype. We only need to define computed properties defined
    // at instantiation here.
    if (!(key in vm)) {
      console.log('computed:', key)
      defineComputed(vm, key, userDef)
    } else if (process.env.NODE_ENV !== 'production') {
      if (key in vm.$data) {
        warn(`The computed property "${key}" is already defined in data.`, vm)
      } else if (vm.$options.props && key in vm.$options.props) {
        warn(`The computed property "${key}" is already defined as a prop.`, vm)
      }
    }
  }
}
```
##### initWatch
调用vm.$watch对watche属性进行监控，而在$watch 方法会在stateMixin初始化中进行赋值，具体就是会对传入的属性都创建一个Watcher对象进行监控，，并且还会对immediate属性进行判定。
``` javascript
function initWatch (vm: Component, watch: Object) {
  console.log('initWatch: Watch初始化')
  // 每个对象创建一个watcher
  for (const key in watch) {
    const handler = watch[key]
    if (Array.isArray(handler)) {
      for (let i = 0; i < handler.length; i++) {
        createWatcher(vm, key, handler[i])
      }
    } else {
      createWatcher(vm, key, handler)
    }
  }
}

function createWatcher (
  vm: Component,
  expOrFn: string | Function,
  handler: any,
  options?: Object
) {
  if (isPlainObject(handler)) {
    options = handler
    handler = handler.handler
  }
  if (typeof handler === 'string') {
    handler = vm[handler]
  }
  console.log('createWatcher: 调用$watch方法，监控属性')
  return vm.$watch(expOrFn, handler, options)
}
```

#### initProvide
判定是否存在provide, 然后进行赋值
``` javascript
export function initProvide (vm: Component) {
  const provide = vm.$options.provide
  if (provide) {
    vm._provided = typeof provide === 'function'
      ? provide.call(vm)
      : provide
  }
}
```
#### vm.$mount
加载组件
