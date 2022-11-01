---
title: seajs源码流程分析
date: 2018-10-25
tags: source-code
---

#### 有言在先
虽然现在前端已经是webpack，gulp，rollup，ES6 等加载器，但在那个前端加载器一无所有的年代，requirejs，seajs也曾站在巅峰过。最近有时间，来阅读一下曾经的CMD模块加载器的源代码，感受一下大神的思路。

#### 要点
>1. 并行加载，但是顺序执行，这一点和浏览器的机制一样，其实也相当于是模拟浏览器的机制。
>2. define 触发执行
>3. _entry用来保存入口模块，用来触发factory callback，以及检测remain的值。
>4. m.fetch用来拉取文件
>5. addOnload添加onload监听
>6. onload事件竟然是在代码执行之后才触发
>7. 异步加载的js , load方法在脚本执行之后触发。
>8. define之法执行时，解析依赖。
>9. _remain用来计数，剩下需要加载的模块
>10. 虽然文件是并行加载，但是factory确实同步执行，factory会被保存起来，直到所有模块加载完成，然后逐一触发。
>11. 执行方法是在exec中进行

#### 入口
``` javascript
Module.use = function (ids, callback, uri) {
  var mod = Module.get(uri, isArray(ids) ? ids : [ids])
  
  //_enrty入口模块引用，use方法中将自身模块加入到入口中
  mod._entry.push(mod)
  mod.history = {}
  mod.remain = 1

  //callback方法
  mod.callback = function() {
    var exports = []
    var uris = mod.resolve()

    for (var i = 0, len = uris.length; i < len; i++) {
      exports[i] = cachedMods[uris[i]].exec()
    }

    if (callback) {
      callback.apply(global, exports)
    }

    delete mod.callback
    delete mod.history
    delete mod.remain
    delete mod._entry
  }
  
  //加载模块
  mod.load()
}
```

##### load方法
``` javascript
Module.prototype.load = function() {
  var mod = this

  // If the module is being loaded, just wait it onload call
  if (mod.status >= STATUS.LOADING) {
    return
  }

  mod.status = STATUS.LOADING

  // Emit `load` event for plugins such as combo plugin
  var uris = mod.resolve()
  emit("load", uris)

  //获取依赖模块对象
  for (var i = 0, len = uris.length; i < len; i++) {
    mod.deps[mod.dependencies[i]] = Module.get(uris[i])
  }

  // Pass entry to it's dependencies
  //对依赖的模块进行遍历，设置对应的入口_entry值
  //用来保存入口模块引用，方便模块加载完成之后factory方法的调用
  mod.pass()

  // If module has entries not be passed, call onload
  if (mod._entry.length) {
    mod.onload()
    return
  }

  // Begin parallel loading
  var requestCache = {}
  var m

  for (i = 0; i < len; i++) {
    m = cachedMods[uris[i]]

    if (m.status < STATUS.FETCHING) {
      m.fetch(requestCache)
    }
    else if (m.status === STATUS.SAVED) {
      m.load()
    }
  }

  // Send all requests at last to avoid cache bug in IE6-9. Issues#808
  for (var requestUri in requestCache) {
    if (requestCache.hasOwnProperty(requestUri)) {
      requestCache[requestUri]()
    }
  }
}
```

##### pass方法
``` javascript
Module.prototype.pass = function() {
  var mod = this

  var len = mod.dependencies.length

  for (var i = 0; i < mod._entry.length; i++) {
    var entry = mod._entry[i]
    var count = 0
    for (var j = 0; j < len; j++) {
      var m = mod.deps[mod.dependencies[j]]
      // If the module is unload and unused in the entry, pass entry to it
      if (m.status < STATUS.LOADED && !entry.history.hasOwnProperty(m.uri)) {
        entry.history[m.uri] = true
        count++
        m._entry.push(entry)
        if(m.status === STATUS.LOADING) {
          m.pass()
        }
      }
    }
    // If has passed the entry to it's dependencies, modify the entry's count and del it in the module
    if (count > 0) {
      // remain的初始值就是1，表示默认就会有一个未加载的模块，所有需要减1
      entry.remain += count - 1
      // 如果有未加载的依赖项，则移除掉入口模块的entry
      mod._entry.shift() //同上，由于当前模块已经加载，移除模块的_entry
      i-- //元素被移除一个，所以标记退一格
    }
  }
}
```
pass方法拉取依赖模块，如果依赖模块没有被加载的话，那么就给依赖模块加上_entry入口模块。
并且如果当前模块已经被加载，且还有未加载的依赖项的话，那么当前模块就会移除掉_entry。
那么可以得出，如果当前模块被加载，且没有要加载的依赖项，当前模块的_entry值就不会为0，因为在use方法中一开始就被压入了自身模块。

所以在load方法会对_entry的长度进行判断，经历过pass方法之后，当前模块的_entry长度不为0 ，则当前模块以及依赖模块已经加载ok。

``` javascript
// If module has entries not be passed, call onload
if (mod._entry.length) {
  mod.onload()
  return
}
```
如果存在依赖没有加载完，那么就会拉取依赖模块.
``` javascript
for (i = 0; i < len; i++) {
  m = cachedMods[uris[i]]

  if (m.status < STATUS.FETCHING) {
    m.fetch(requestCache)
  }
  else if (m.status === STATUS.SAVED) {
    m.load()
  }
}

// Send all requests at last to avoid cache bug in IE6-9. Issues#808
for (var requestUri in requestCache) {
  if (requestCache.hasOwnProperty(requestUri)) {
    requestCache[requestUri]()
  }
}
```

##### fetch方法
``` javascript
// 拉取模块
Module.prototype.fetch = function(requestCache) {
  var mod = this
  var uri = mod.uri

  mod.status = STATUS.FETCHING

  // Emit `fetch` event for plugins such as combo plugin
  var emitData = { uri: uri }
  emit("fetch", emitData) //触发fetch事件，通常为保留的plugin事件接口，参考切面编程
  var requestUri = emitData.requestUri || uri

  // Empty uri or a non-CMD module
  if (!requestUri || fetchedList.hasOwnProperty(requestUri)) {
    mod.load()
    return
  }

  if (fetchingList.hasOwnProperty(requestUri)) {
    callbackList[requestUri].push(mod)
    return
  }

  fetchingList[requestUri] = true
  callbackList[requestUri] = [mod]

  // Emit `request` event for plugins such as text plugin
  // 生成一个emitData
  emit("request", emitData = {
    uri: uri,
    requestUri: requestUri,
    onRequest: onRequest,
    charset: isFunction(data.charset) ? data.charset(requestUri) : data.charset,
    crossorigin: isFunction(data.crossorigin) ? data.crossorigin(requestUri) : data.crossorigin
  })
  
  // 将sendRequest 方法封装给对应的requestCache对象中
  // sendRequest方法实际是调用了seajs的request方法
  // { 'a.js' : sendRequest }
  if (!emitData.requested) {
    requestCache ?
      requestCache[emitData.requestUri] = sendRequest :
      sendRequest()
  }

    // 上面给每一个模块一个方法的原因，应该是由于emitData的值不一样，多个方法，可以将数据存储在闭包内，这样保证了数据的正确性。
  //将onRequest作为callback传入
  function sendRequest() {
    seajs.request(emitData.requestUri, emitData.onRequest, emitData.charset, emitData.crossorigin)
  }
  
  ....

```

##### request方法
``` javascript
//是否支持webworker
if (isWebWorker) {
  function requestFromWebWorker(url, callback, charset, crossorigin) {
    // Load with importScripts
    var error
    try {
      importScripts(url)
    } catch (e) {
      error = e
    }
    callback(error)
  }
  // For Developers
  seajs.request = requestFromWebWorker
}
else {
  var doc = document
  var head = doc.head || doc.getElementsByTagName("head")[0] || doc.documentElement
  var baseElement = head.getElementsByTagName("base")[0]

  var currentlyAddingScript

  function request(url, callback, charset, crossorigin) {
    var node = doc.createElement("script")

    if (charset) {
      node.charset = charset
    }

    if (!isUndefined(crossorigin)) {
      node.setAttribute("crossorigin", crossorigin)
    }
    //添加onload方法，将callback传入，其实就是上面说的onRequest方法
    addOnload(node, callback, url)

    node.async = true
    node.src = url

    // For some cache cases in IE 6-8, the script executes IMMEDIATELY after
    // the end of the insert execution, so use `currentlyAddingScript` to
    // hold current node, for deriving url in `define` call
    currentlyAddingScript = node

    // ref: #185 & http://dev.jquery.com/ticket/2709
    baseElement ?
        head.insertBefore(node, baseElement) :
        head.appendChild(node)

    currentlyAddingScript = null
  }

  function addOnload(node, callback, url) {
    var supportOnload = "onload" in node

    if (supportOnload) {
      // 指定load事件回调
      node.onload = onload
      node.onerror = function() {
        emit("error", { uri: url, node: node })
        onload(true)
      }
    }
    else {
      // 指定load事件回调
      node.onreadystatechange = function() {
        if (/loaded|complete/.test(node.readyState)) {
          onload()
        }
      }
    }

    function onload(error) {
      // Ensure only run once and handle memory leak in IE
      node.onload = node.onerror = node.onreadystatechange = null

      // Remove the script to reduce memory leak
      if (!data.debug) {
        head.removeChild(node)
      }

      // Dereference the node
      node = null
      //此处的callback 即为上面传入的回调方法，也就是onRequest方法
      callback(error)
    }
  }

  // For Developers
  seajs.request = request
}
```

接下来，浏览器会通过异步加载的方式进行加载文件，文件加载之后会先执行模块，这里对模块进行了一定的规范，必须用define进行定义，这样文件执行时，会优先执行define方法。
``` javascript
Module.define = function (id, deps, factory) {
  //前面调整了一下参数
  var argsLen = arguments.length

  // define(factory)
  if (argsLen === 1) {
    factory = id
    id = undefined
  }
  else if (argsLen === 2) {
    factory = deps

    // define(deps, factory)
    if (isArray(id)) {
      deps = id
      id = undefined
    }
    // define(id, factory)
    else {
      deps = undefined
    }
  }

  // Parse dependencies according to the module factory code
  // 对定义的模块中的依赖进行解析，主要用来解析require字段
  if (!isArray(deps) && isFunction(factory)) {
    deps = typeof parseDependencies === "undefined" ? [] : parseDependencies(factory.toString())
  }

  var meta = {
    id: id,
    uri: Module.resolve(id),
    deps: deps,
    factory: factory
  }
  // 这里需要考虑一个点，因为如果是匿名模块的话，当前模块就不存在id，那么就需要用到 
  // getCurrentScript方法获取当前文件的uri作为存储标记，因为这个id值在一开始就没传递。如
  // 文件 a.js : define(function(){})
  // Try to derive uri in IE6-9 for anonymous modules
  if (!isWebWorker && !meta.uri && doc.attachEvent && typeof getCurrentScript !== "undefined") {
    var script = getCurrentScript()

    if (script) {
      meta.uri = script.src
    }

    // NOTE: If the id-deriving methods above is failed, then falls back
    // to use onload event to get the uri
  }

  // Emit `define` event, used in nocache plugin, seajs node version etc
  emit("define", meta)
 
  //当前模块已经加载完成，对模块进行保存
  meta.uri ? Module.save(meta.uri, meta) :
    // Save information for "saving" work in the script onload event
    anonymousMeta = meta
}
```

define方法执行完毕之后，就会进行触发load方法，也就是开始执行上面提到的onRequest回调。

``` javascript
  // 模块触发load方法是被调用
  function onRequest(error) {
    delete fetchingList[requestUri]
    fetchedList[requestUri] = true

    // Save meta data of anonymous module
    if (anonymousMeta) {
      Module.save(uri, anonymousMeta)
      anonymousMeta = null
    }

    // Call callbacks
   //获取该模块的回调方法，由于同一个模块可能被多处引用，所以此处是一个数组，正常情况下此处应该只有一个值
    var m, mods = callbackList[requestUri]  
    delete callbackList[requestUri]
    while ((m = mods.shift())) {
      // When 404 occurs, the params error will be true
      if(error === true) {
        m.error()
      }
      else {
        m.load() 
       // 没有错误信息则执行 load方法，表示当前模块已经加载完毕，可以进行安装，
       // 然后又开始遍历其是否存在依赖模块
      }
    }
  }
}
```
接下来，如果存在依赖模块则会继续进行拉取，如果没有则会触发以下条件
``` javascript
if (mod._entry.length) {
  mod.onload()
  return
}
```
执行其factory方法。

``` javascript
Module.prototype.onload = function() {
  var mod = this
  mod.status = STATUS.LOADED //设定状态

  // When sometimes cached in IE, exec will occur before onload, make sure len is an number
  for (var i = 0, len = (mod._entry || []).length; i < len; i++) {
    var entry = mod._entry[i] //遍历其关联的入口模块
    if (--entry.remain === 0) { //判定其remain 剩余需要加载的依赖模块是否安装完毕
      entry.callback() //一一执行入口模块的callback，也就是其factory方法
    }
  }

  delete mod._entry
}
```
那么依赖模块的factory方法是怎么执行的呢？
其实也在其入口模块的callback方法执行的时候进行遍历执行的。可以查看入口中的callback定义。
``` javascript
Module.use = function (ids, callback, uri) {
  
  ...

  mod.callback = function() {
    var exports = []
    var uris = mod.resolve()

    for (var i = 0, len = uris.length; i < len; i++) {
      exports[i] = cachedMods[uris[i]].exec() //遍历依赖模块并且进行执行其方法
    }

    if (callback) {
      callback.apply(global, exports)
    }

    delete mod.callback
    delete mod.history
    delete mod.remain
    delete mod._entry
  }

  mod.load()
}
```
这里要注意的是cachedMods对象，这是一个全局对象，而每次调用Module.get的时候，如果不存在模块对象，就会创建一个对象，并保存在cachedMods中。

接下来看一下 exec方法
``` javascript
Module.prototype.exec = function () {
  var mod = this

  // When module is executed, DO NOT execute it again. When module
  // is being executed, just return `module.exports` too, for avoiding
  // circularly calling
  if (mod.status >= STATUS.EXECUTING) {
    return mod.exports
  }

  mod.status = STATUS.EXECUTING

  if (mod._entry && !mod._entry.length) {
    delete mod._entry
  }

  //non-cmd module has no property factory and exports
  if (!mod.hasOwnProperty('factory')) {
    mod.non = true
    return
  }

  // Create require
  var uri = mod.uri

  function require(id) {
    var m = mod.deps[id] || Module.get(require.resolve(id))
    if (m.status == STATUS.ERROR) {
      throw new Error('module was broken: ' + m.uri)
    }
    return m.exec()
  }

  require.resolve = function(id) {
    return Module.resolve(id, uri)
  }

  require.async = function(ids, callback) {
    Module.use(ids, callback, uri + "_async_" + cid())
    return require
  }

  // Exec factory
  // 获取factory 方法
  var factory = mod.factory
  //判断是否是一个函数，是一个方法，则直接执行，
  // 并且把export，require等属性传递过去，修改this为exports
  var exports = isFunction(factory) ?
    factory.call(mod.exports = {}, require, mod.exports, mod) :
    factory

  if (exports === undefined) {
    exports = mod.exports
  }

  // Reduce memory leak
  delete mod.factory

  mod.exports = exports
  mod.status = STATUS.EXECUTED

  // Emit `exec` event
  emit("exec", mod)

  return mod.exports
}
```
到此 seajs就执行结束了。

#### 其它
##### Module.save方法

``` javascript
// Save meta data to cachedMods
Module.save = function(uri, meta) {
  var mod = Module.get(uri) //新建一个Module实例，并且存放在cachedMods中，然后设置其属性

  // Do NOT override already saved modules
  if (mod.status < STATUS.SAVED) {
    mod.id = meta.id || uri
    mod.dependencies = meta.deps || []
    mod.factory = meta.factory
    mod.status = STATUS.SAVED

    emit("save", mod)
  }
}
```
##### get
``` javacript
Module.get = function(uri, deps) {
  return cachedMods[uri] || (cachedMods[uri] = new Module(uri, deps))
}
```
##### require

``` javascript
// require方法 直接调用 onload方法，并执行
seajs.require = function(id) {
  var mod = Module.get(Module.resolve(id))
  if (mod.status < STATUS.EXECUTING) {
    mod.onload()
    mod.exec()
  }
  return mod.exports
}
```

##### define

define模块之中的 require与require.async方法
``` javascript
  function require(id) {
    var m = mod.deps[id] || Module.get(require.resolve(id))
    if (m.status == STATUS.ERROR) {
      throw new Error('module was broken: ' + m.uri)
    }
    return m.exec()
  }

  require.async = function(ids, callback) {
    Module.use(ids, callback, uri + "_async_" + cid())
    return require
  }
```
看以看到这里的require方法是直接使用模块，然后调用其exec方法，而require.async则是使用use方法，那么需要重新去异步加载，所以这也是为什么require是同步的，这require.async是异步加载的原因。

最后配上两张对于_entry理解的图
![_entry1](https://raw.githubusercontent.com/helloyangzhi/learn/master/_entry5.png)

等有时间再来整一张完整的流程图，希望有时间，哈哈哈.....

tips:
1 seajs 中define方法的第二个参数如果被传递，那么会加载该模块但不会执行该factory方法。
当然这也符合cmd的特性，虽然可以提前加载模块但不会立刻执行。
``` javascript
define(id, deps, function(){}
```
2 seajs 中加载依赖方式使用require
``` javascript
define(id, function() {
  require('./a.js'); 
  //源码中会对require进行解析，并且当前模块factory执行时，会调用对应的require方法，从而执行依赖模块的factory方法
})
```
