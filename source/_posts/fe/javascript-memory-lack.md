---
title: 内存泄露以及循环引用解析
date: 2015-07-11 16:43:07
tags:
description: 对于内存的一些简单理解吧。
---
内存泄露

在javascript中，我们很少去关注内存的管理。我们创建变量，使用变量，浏览器关注这些底层的细节都显得很正常。

但是当应用程序变得越来越复杂并且ajax化之后，或者用户在一个页面停留过久，我们可能需要去注意一些问题，如一个浏览器花费了1G以上的内存，并且在不断的增加。这些问题常常都是因为内存泄露引起。

Javascript 内存泄露

这个javascript内存管理的核心概念就是具不具有可达性的概念。

1 一个明显的对象集合将会被认为是可达的：这些对象是被知道的像roots一样。

包括那些所有的对象在堆栈中个地方被引用（包括，所有的局部变量，正在被调用的方法的中的参数），以及任何的全局变量。

2 对象保存在内存中，他们是可以到达的从roots 对象，通过一个引用货者一个引用链。

这里有一个GC 垃圾回收器在浏览器中，用来清楚没有用的对象在内存中。

垃圾回收example
``` javascript
function Menu(title) {
  this.title = title
  this.elem = document.getElementById('id')
}

var menu = new Menu('My Menu')

document.body.innerHTML = ''  // (1)

menu = new Menu('His menu') // (2)
```
来看一下内存结构：
{% asset_image 20150711154107210.png %}

在step(1) 中，Body.innerHTML 被清除掉，所以它的子节点也会被删除，因为他们不再被关联。

但是这个元素#id 是一个例外，他是是被 menu.elem 关联着，所以该对象会一直存在内存中，当然 ，如果你检查他的parentNode, 将会得到一个null值。

注意：个别的Dom元素 可以会保存在内存中即使他们的parent 被移除了。

在step(2) 中，引用window.menu 被定义，所以之前的 menu因为不再被关联，它将会自动被移除通过浏览器的GC。
{% asset_image 20150711154646812.png %}

循环引用集合

闭包经常会导致循环引用，例如：
``` javascript
function setHandler() {

  var elem = document.getElementById('id')

  elem.onclick = function() {
    // ...
  }
}
```
在这里，这个DOM 元素直接引用匿名function通过onclick。并且这个function引用了elem元素通过外部的词法环境。
{% asset_image 20150711155007031.png %}

( 这里多说一点，关于[[Scope]]是function的内部属性，在创建function的时候，会将外部函数的词法环境加入到[[Scope]]中，这里涉及到javascript的作用域问题。)

这样的内存结构一样会出现即使这个处理函数内部没有任何的代码。特别的一些方法如addEventListener/attachEvent 也会在内部创建一个引用。

在这个处理函数中通常进行清除，当这个elem死亡的时候。


``` javascript
function cleanUp() {
  var elem = document.getElementById('id')
  elem.parentNode.removeChild(elem)
}
```


调用clearUp删除元素从Dom 中。这里依旧存在一个引用，LexialEnvironment.elem ，但是这里没有了嵌套的functions，所以 LexialEnvironment 是可以回收的。

在这之后，elem 变成没有关联的并且和他的handlers一起被回收。



内存泄露

内存泄露主要发生当一些浏览器由于一些问题不能够移除没有用的对象从内存中。

这发生可能是由于一些原因，如浏览器的Bugs，浏览器的扩展问题，或多或少，我们自己的代码错误。



IE 8 以下 DOM-JS 内存泄露

IE8 之前的浏览器不能对DOM和javascript之间的循环引用进行清理。这个问题相对更加的严重在ie6 windows xp sp3 之前的版本

因为内存没法释放在页面卸载之前。



所以 setHandler 泄露在ie 8 之前的浏览器，elem 和这些闭包没办法清除。


``` javascript
function setHandler() {
  var elem = document.getElementById('id')
  elem.onclick = function() { /* ... */ }
}
```



不仅仅是DOM 元素，包括XMLHttpRequest 或者其它COM 对象，都会存在此现象。

在IE下用来打破循环引用的方法：
{% asset_image 20150711160712981.png %}

我们定义了elem = null,所以这个处理函数不再关联到DOM 元素，这个循环自然打破。



XmlHttpRequest 内存管理和泄露

下面的代码在i9以下浏览器内存泄露：
``` javascript
var xhr = new XMLHttpRequest() // or ActiveX in older IE

xhr.open('GET', '/server.url', true)

xhr.onreadystatechange = function() {
  if(xhr.readyState == 4 && xhr.status == 200) {            
    // ...
  }
}

xhr.send(null)
```
看一下内存结构：
{% asset_image 20150711160935482.png %}

这个异步xmlHttpRequest对象一直被浏览器追踪，因为有一个内部的引用关联到它。

当这个请求结束之后，这个引用就会被删除，所以xhr 变成不可关联对象。但是ie9以下的浏览器不是这么做的。

幸运的是，要修复这个Bug很简单，我们需要删除这个xhr 从这个闭包中并且使用它用this在这个处理函数中。如下：
``` javascript
var xhr = new XMLHttpRequest()
  
xhr.open('GET', 'jquery.js', true)
  
xhr.onreadystatechange = function() {
  if(this.readyState == 4 && this.status == 200) {            
    document.getElementById('test').innerHTML++
  }
}
   
xhr.send(null)
xhr = null
}, 50)
```
{% asset_image 20150711161343489.png %}
这样就没有了循环引用。

setInterval/setTimeout

在使用setTimeout/setInterval 也会存在内部引用并且被追踪知道结束，然后clear up.

对于setInterval 这个结束发生在 clearInterval中，这个可能会导致内存泄露当这个方法实际什么也 没做，但是这个interval却没有被清除。

内存泄露的大小

内存泄露的数据结构的size可能不大。

但是这个闭包会导致外部函数的所有的变量遗留下来，当这个内部函数是活动的时候。

所以，你可以想象，你创建了一个function,而且其中一个变量包含了一个大的字符串。
``` javascript
function f() {
  var data = "Large piece of data, probably received from server"

  /* do something using data */

  function inner() {
    // ...
  }

  return inner
}
```

While the function inner function stays in memory, then the LexicalEnvironment with a large variable inside will hang in memory until the inner function is alive.

事实上，这可能没有泄露，许多的fucntions 可能会被创建因为一些合理的原因。比如，对于每一个请求，并不清干净，因为他们是一些处理函数或者其它什么。

如果这个data 仅仅被使用在外部函数，我们可以使它作废在外部方法中。
``` javascript
function f() {
  var data = "Large piece of data, probably received from server"

  /* do something using data */

  function inner() {
    // ...
  }

  data = null
  return inner
}
```

现在。这个data 依旧保留在内存中作为一个词法环境的一个属性，不过它不再需要去占用太多的空间。

jQuery 内存泄露和避免方式

jQuery 使用 $.data 去避免ie 6 7 内存泄露。不幸运的是，它导致了一些新的 jQuery 特殊的内存泄露。

这个核心原理关于$.data是，任何的javascript实体被限制去读取一个元素使用如下的方式
``` javascript
// works on this site cause it's using jQuery

$(document.body).data('prop', 'val') // set
alert( $(document.body).data('prop') ) // get

jQuery $(elem).data(prop,val) 按照如下步骤：
```
1 元素获取一个唯一的标记如果它不存在的话：

elem[ jQuery.expando ] = id = ++jQuery.uuid  // from jQuery source

2 data 被设置到一个特殊的对象 jQuery.cache:
``` javascript
jQuery.cache[id]['prop'] = val
```
当这个date从一个元素中被读取：

1 这个元素的唯一标示会被返回：id = elem[jQuery.expando]

2 这个data 会被读取从jQuery.cache[id]

jQuery设置这个api的目的就是为了让DOM元素不再直接引用Javascript元素。它使用了一个数量，但是很安全。

这个data 保存在jQuery.cache中。内部事件处理函数同样使用$.data API。

同时也造成了另一方面的影响，一个元素不能被移除从DOM中使用 本地的调用。

如下代码造成了内存泄露在所有的浏览器中：

``` javascript
$('<div/>')
  .html(new Array(1000).join('text')) // div with a text, maybe AJAX-loaded
  .click(function() { })
  .appendTo('#data')

document.getElementById('data').innerHTML = ''
```
这个泄露的发生因为elem 被removeed 通过清除 parent 的innerHTML .但是这个data依旧保存在jQuery.cache中。

更重要的是，这个事件处理函数引用elem,所以这个事件处理函数和elem保留在内存中和整个闭包。

一个简单的泄露例子
``` javascript
function go() {
  $('<div/>')
    .html(new Array(1000).join('text')) 
    .click(function() { })
}
```
这个例子的问题在于，这个元素被创建了，但是没有使用。所以在这个函数定义之后，这个引用就消失了， 但是这个jQuery.cache中依旧是存在的。

----------------------------------------------------------------------------

原文地址：http://javascript.info/tutorial/memory-leaks