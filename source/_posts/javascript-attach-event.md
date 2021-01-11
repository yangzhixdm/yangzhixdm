---
title: attachEvent事件
date: 2014-04-21 18:35:37
tags:
description: 关于绑定事件的上下文问题。
---
最近在看本司徒大神的博客，发现其中很是有还多可以发掘的东西的!

如下:

``` html
<!doctype html>
<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<title>Document</title>
</head>
<body>
	<input type="button" value="call me" id="call-id" alt="1"/>
</body>
<script type="text/javascript">
	
	var E = {

		on:function(el,type,fn){
			el.addEventListener?
				el.addEventListener(type,fn,false):el.attachEvent("on" + type,function(){
					fn.call(el,window.event);
				})

				/*
					el.addEventListener(type,fn,false):el.attachEvent("on" + type,fn);
				*/
		}
	};
	
	function handler(e){
		//alert(e.getAttribute("alt"));
		alert(this == window);
		alert(e.srcElement);
	}
	E.on(document.getElementById("call-id"),"click",handler);
</script>
</html>
```
以上被注释的代码，想了很久没有想明白为什么他要采用一种全新的方式去调用，那就是用call ?

包括很多书上都是这样写的，都是直接执行。

于是开始各种搜索，测试之。

发现原来还是有区别的。

在这里，使call去调用：
``` javascript
fn.call(el,window.event);
```
1 第一点，它能够确保，事件对象作为第一个参数被传入，这里直接将window.event传入。（这一点，司徒大神已经说过了）

2 在某些浏览器下，ie678,如果直接调用，会出现 this == window 的情况，这关于这个问题，就不多说了，都是万恶的IE导致的。

那么如果采用call 可以保证this的值指向事件源对象。这对于兼容标准浏览器和IE我想是有一定的用处的。

偶有所得，以记录之，谨防忘记!

[ 后记 ]

这里的使用call 只不过是为了调整作用域，保证在被调用的方法中，this指向调用的对象。