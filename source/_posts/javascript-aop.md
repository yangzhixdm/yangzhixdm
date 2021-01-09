---
title: javascript中面向切面编程[Aop]
date: 2014-10-22 15:37:23
tags:
---
对于java中的aop，早有耳闻，所谓面向切面编程，不过一直都以为只是在后台才会用到。

从做前端开始就没有这方面的需求，这一次对于一些东西做新的想法，发现有些东西还是可以去考虑考虑的。

有一个公用的代码，可能在很多地方都会被用到，那么现在要做的就是，需要这个方法跑起来之前走一些东西，在这个方法跑完之后，还在处理一些东西。

那么问题就来了：

1 直接改之前封装好的东西，Ok,可以没问题，但是这样做了之后，别处用到这个东西的地方就直接崩溃了！

2 那么如果采用直接复制一份代码，修改单个这一处的问题，ok，没有问题，但是这不也就是代表着代码冗余越来越多。

这个时候想起了，在java里面有一个东西，面向切面，添加切点，一切似乎就可以正常的跑起来了。是的，就这么干.....

如下：

由于是工具类，无需实例化，刚直接可以采用{}对象方式：
``` javascript
aop = window.aop || {};

aop = {
	doBefore:function(ins,methodName,callback){
		if(typeof ins[methodName] != "undefined"){
			var method = ins[methodName];	
		}
		ins[methodName] = function(){
			callback.apply(ins,arguments)
			return method.apply(ins,arguments);
		}
	},
	doAfter:function(ins,methodName,callback){
		if(typeof ins[methodName] != "undefined"){
			var method = ins[methodName];
		}
		ins[methodName] = function(){
			var ret = method.apply(ins,arguments),
				callret = callback.apply(ins,arguments);
			return typeof callret == "undefined" ? ret : callret;
		}
	}
}
```

html 中测试如下：
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<script type="text/javascript" src="aop.js"></script>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<title>aop javascript</title>
</head>
<body>
	<script type="text/javascript">
		//json对象
		var obj = {
			run:function(){
				console.log("run");
			},
			init:function(name){
				console.log("arguments : " + name);
				return "info";
			}
		};


		aop.before(obj,"run",function(){
			console.log("run before");
		});
		aop.after(obj,"run",function(){
			console.log("run after");
		});
		console.log("<!-----json对象--------->");
		obj.run();

		var F = function(){};
		F.prototype = {
			run:function(){
				console.log("prototype run");
			}
		};
		//创建对象
		console.log("<!-----创建对象--------->");
		var f = new F();

		aop.before(f,"run",function(){
			console.log("prototype run before");
		});
		aop.after(f,"run",function(){
			console.log("prototype run after");
		});
		f.run();

		//参数问题
		console.log("<!-----参数问题--------->");
		aop.before(obj,"init",function(name){
			console.log("arguments before :" + name);
		});
		aop.after(obj,"init",function(name){
			console.log("arguments after :" + name);
			return "next";
		});

		console.log(obj.init("user"));

	</script>
</body>
</html>
```

这里多种情况，包括参数的传递问题，皆有考虑!

偶有所得，以记录之，谨防忘记!