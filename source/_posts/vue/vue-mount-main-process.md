---
title: vue组件挂载主流程
date: 2019-03-19
tags: Vue
---

### vm.$mount

在vue 进行init之后，将会进入挂载阶段，也就是在created生命周期之后。

``` javascript
Vue.prototype._init = function (options) {

  //some code ...

  initLifecycle(vm);
  initEvents(vm);
  initRender(vm);
  callHook(vm, 'beforeCreate');
  initInjections(vm); // resolve injections before data/props
  initState(vm);
  initProvide(vm); // resolve provide after data/props
  callHook(vm, 'created');

  //some code ...
  if (vm.$options.el) {
    vm.$mount(vm.$options.el);
  }
}
```
``` javascript
Vue.prototype.$mount = function (el, hydrating) {
 el = el && inBrowser ? query(el) : undefined;
  return mountComponent(this, el, hydrating);
};
```

### mountComponent
2.1 首先触发beforeMount周期钩子函数
2.2 生成updataComponent方法
``` javascript
updateComponent = function updateComponent() {
  vm._update(vm._render(), hydrating);
};
```
2.3 创建一个Wather监听器
``` javascript
new Watcher(vm, updateComponent, noop, null, true /* isRenderWatcher */);
```
####  function Watcher
updateComponent 方法被赋值给Wather中的getter方法，且生成的Wather为renderWather，然后赋值给vm的wather属性，并push进wathers数组中，进行记录。
然后判定lazy属性，如果非lazy，则调用wather的get方法。

#### Watcher.prototype.get
1 当前watcher压入target数组
2 调用getter方法，也就是上面的mountComponent方法。
3 弹出target
4 清除Dep
``` javascript
 Watcher.prototype.get = function get() {
    pushTarget(this);
    var value;
    var vm = this.vm;
    try {
      value = this.getter.call(vm, vm);
    } catch (e) {
      if (this.user) {
        handleError(e, vm, "getter for watcher \"" + this.expression + "\"");
      } else {
        throw e;
      }
    } finally {
      // "touch" every property so they are all tracked as
      // dependencies for deep watching
      if (this.deep) {
        traverse(value);
      }
      popTarget();
      this.cleanupDeps();
    }
    return value;
  };
```

2.4 回溯调用mounted周期钩子函数，注意这里会涉及到回溯。

### updataComponent
调用_update方法进行更新。
``` javascript
updateComponent = function updateComponent() {
  vm._update(vm._render(), hydrating);
};
```

### Vue.prototype._render
> 调用render方法，生成vnode，并且指定 parent

### _update
如果已经 mounted过了，则触发beforeUpdate周期。
否则开始进行__patch__.
``` javascript
Vue.prototype._update = function (vnode, hydrating) {
  var vm = this;
  if (vm._isMounted) {
    callHook(vm, 'beforeUpdate');
  }
  //some code ...
  if (!prevVnode) {
  // initial render
  vm.$el = vm.__patch__(vm.$el, vnode, hydrating, false , vm.$options._parentElm, vm.$options._refElm);
  vm.$options._parentElm = vm.$options._refElm = null;
  } else {
    vm.$el = vm.__patch__(prevVnode, vnode);
  }
  //some code ...
};
```
### `__patch__`
1 如果新旧节点都为vnode,则进行patchNode，这一部分用来用来对比vnode的差异。
2 如果新节点isRealElement（为元素节点），则直接生成一个空的vnode，并且根据元素信息调用createEle，生成新的节点。
``` javascript
oldVnode = emptyNodeAt(oldVnode);
```
### createElm
在createEle中会进行创建Component.
``` javascript
  vnode.isRootInsert = !nested; // for transition enter check
  if (createComponent(vnode, insertedVnodeQueue, parentElm, refElm)) {
    return;
  }
```
### 8 createComponent
这个方法就是根据vnode来创建组件，首先会检测一下vnode.data中有没有hook和init方法，
然后调用init方法，进行创建组建，并传入vnode.

### componentVNodeHooks中的init
``` javascript
init: function init(vnode, hydrating, parentElm, refElm) {
  if (vnode.componentInstance && !vnode.componentInstance._isDestroyed && vnode.data.keepAlive) {
    // kept-alive components, treat as a patch
    var mountedNode = vnode; // work around flow
        componentVNodeHooks.prepatch(mountedNode, mountedNode);
    } else {
      var child = vnode.componentInstance = createComponentInstanceForVnode(vnode, activeInstance, parentElm, refElm);//创建组件
      child.$mount(hydrating ? vnode.elm : undefined, hydrating);//开始挂载
    }
  }
```

### createComponentInstanceForVnode
根据根据vnode创建Component。
``` javascript
return new vnode.componentOptions.Ctor(options);
```
而这个 vnode.componentOptions.Ctor 就是Vue的构造方法，就是下面这个东西。
``` javascript
function VueComponent(options) {
  this._init(options);
};
```
> 创建完成之后，开始挂载生成的子组件，也就是回到了第一步，开始进入递归操作，知道子组件触发了mounted之后，父组件才回溯触发mounted生命周期。

### createChildren
然后进行创建子节点
``` javascript
function createElm(vnode, insertedVnodeQueue, parentElm, refElm, nested, ownerArray, index) {
  //some code ...
  vnode.isRootInsert = !nested; // for transition enter check
    if (createComponent(vnode, insertedVnodeQueue, parentElm, refElm)) {
      return;
    }
  //some code ...
  {
    createChildren(vnode, children, insertedVnodeQueue);
    if (isDef(data)) {
      invokeCreateHooks(vnode, insertedVnodeQueue);
    }
    insert(parentElm, vnode.elm, refElm);
  }
}
```

``` javascript
function createChildren(vnode, children, insertedVnodeQueue) {
  if (Array.isArray(children)) {
    {
      checkDuplicateKeys(children);
    }
    for (var i = 0; i < children.length; ++i) {
      createElm(children[i], insertedVnodeQueue, vnode.elm, null, true, children, i);
    }
  } else if (isPrimitive(vnode.text)) {
    nodeOps.appendChild(vnode.elm, nodeOps.createTextNode(String(vnode.text)));
  }
}
```
重复调用createEle方法进行创建组件，不停的递归调用下去。
然后触发相应的钩子函数invokeCreateHooks，然后执行insert操作。

