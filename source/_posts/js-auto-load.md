---
title: 关于jQuery中的自加载
date: 2014-03-04 11:29:21
tags:
---
估计很多人都会用到jQuery中的each方法。

那就来看一看jQuery都干了些什么。
### each
找到jquery中的each源码：
``` javascript
each: function( object, callback, args ) {
  var name, i = 0,
    length = object.length,
    isObj = length === undefined || jQuery.isFunction( object );
    
  if ( args ) {	
    if ( isObj ) {	
      for ( name in object ) {	
        if ( callback.apply( object[ name ], args ) === false ) {
          break;
        }
      }
    } else {
      for ( ; i < length; ) {
        if ( callback.apply( object[ i++ ], args ) === false ) {
          break;
        }
      }
    }

  // A special, fast, case for the most common use of each
  } else {
    if ( isObj ) {
      for ( name in object ) {
        if ( callback.call( object[ name ], name, object[ name ] ) === false ) {
          break;
        }
      }
    } else {
      for ( ; i < length; ) {
        if ( callback.call( object[ i ], i, object[ i++ ] ) === false ) {
          break;
        }
      }
    }
  }

  return object;
},
```

这段代码还是比较简单的，应该没什么问题.

为了测试这个东西，我写了下面一段js;
``` javascript
$.each($(".demo"),function(){
  console.log("show");
});
```

然后再加一段html:
``` html
<a href="" class="demo"></a>
<a href="" class="demo"></a>
<a href="" class="demo"></a>
```
然后拿着jQuery的源码来调试，以为能得到正确的结果。可是很遗憾。
![](/postimg/20140304111339109.png)

可以看到，这里的object竟然不是我所想要的那三个html对象，竟然是js内置的8种数据类型。

之后在jquery的源码中加了一段代码:
``` javascript
console.log(Object.prototype.toString(object));	
console.log(length);
```
之后的输入如下：
![](/postimg/20140304111927062.png)

也就是说在文档加载的时候，jQuery会用each遍历dom对象,即使没有使用
``` javascript
$(function(){
});
```

当遍历指定对象之后，依旧会继续冒泡遍历父级元素。

### 根据jQuery的源码使用回调函数参数
``` javascript
$.each($(".demo"),function(a,b){
  console.log(a + "" + $(a).attr("class"));
})
```
从源码可以看到:
``` javascript
callback.call( object[ i ], i, object[ i++ ] ) 
```
最后都将当前对象通过call方法传递给了回调函数，那么就可以像上面一样去使用当前对象中的属性。当然也可以直接用this来调用。

如果说复杂些的东西，比如，这里我是将$(".demo")作为object传递进去给$.each()

如里某些时候不是传递的jQuery或者html对象。而是一个Object或者array。

而在array中又存在很多其它的object或者方法。

这样就能弄出更多的效果。

### 采用call或者apply实现回调模式.

从上面的代码可以看到：


``` javascript
callback.call(obj,args)
callback.apply([obj],args)
```
之类的代码，这里只需要传递callback函数，就能实现自己调用，这对于提高代码的复用程度用处不小。

但是也存在某些缺点，比如代码可读性降低，耦合程度增加等。

偶有所得，以记录之，以防忘记！
