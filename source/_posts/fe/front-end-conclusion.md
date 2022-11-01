---
title: 前端技术总结
date: 2019-05-08 19:30:00
tags:
description: 前端技术总结
---


文章主要是用来记录一下一些常见问题，权当是复习了。
##1 请用原生JS实现对象的深度拷贝。
其实这个题目还是挺基础的，主要考虑当objt是对象object或者数组时递归调用即可，当然这里还有一个小问题需要注意的就是对于`纯对象`的考量，这里直接用`typeof parent[i] ==='object'`可能会存在一定的问题。
具体代码如下：
```javascript
function extendsDeep(parent,child){
	var i,
		toStr = Object.prototype.toString,
		astr = "[object Array]";
	child = child || {};

	for(i in parent){
		if(parent.hasOwnProperty(i)){
			if(typeof parent[i] === 'object'){
				child[i] =(toStr.call(parent[i]) === astr) ? [] : {};
				extendsDeep(parent[i],child[i]);
			}else{
				child[i] = parent[i];
			}
		}
	}
}
```
## 2 请用原生代码实现简易事件系统。
这里主要涉及浏览器的事件系统，包括事件冒泡，事件捕获，目标事件，以及对不同浏览器的事件兼容性问题处理，最后就是this的指向处理。
```javascript
//add event
funciton addEvent(elem,type,fn){
	if(elem.addEventListener){
	    elem.addEventListener(type,fn,false);//标准浏览器默认事件为冒泡
	}else if(elem.attachEvent){
	    elem.attachEvent('on'+type,function(){
		    fn.call(elem);
	    });
    }else{
		elem['on'+type] = fn;
	}
}

//remove event
function removeEvent(elem,type,fn){
	if(elem.removeEventListener){
		elem.removeEventListener(type,fn,false);
	}else if{
		elem.detachEvent('on'+type,fn);
	}
}
```
## 3 请用原生JS代码实现事件代理
```javascript
addEvent: function(elem,selector,type,fn){
		//判断是否为代理
		if(arguments.length == 3){
			var	fn = type, 
				type = selector;
				selector = "";//代理置空
		}

		if(elem.addEventListener){
			elem.addEventListener(type,function(event){
				//是否存在代理
				if(selector != ""){
					var event = event || window.event,
						target = event.target;
						if("." + target.className == selector){
							fn.apply(elem,arguments);
						}
				}else{
					fn.apply(elem,arguments);
				}
			},false);
		}else if(elem.attachEvent){
			elem.attachEvent("on" + type,function(event){
				//是否存在代理
				if(selector != ""){
					var event = event || window.event,
						target = event.target || event.srcElement;
					if("." + target.className == selector){
						fn.apply(elem,arguments);
					}
				}else{
					fn.apply(elem,arguments);
				}
			});
		}
	}
```

## 4 请用原生JS实现对象继承
对象继承的方式有许多种，主要包括默认模式，借用构造方法模式，借用与设置模式，共享原型模式,临时构造方法模式；
当然这里可能只要你能答出2到3种就已经差不多了； 
###1 `默认模式`
其实默认模式应该是最简单的，需要继承父类的方法，直接简单粗暴的new一个父类的实例出来就搞定了。应该由于js中prototype机制的关系，new出来的实例中会有一个内部属性`__proto__` 指向prototype对象，那么将子类的prototype对象直接赋值为new出来的实例，一切就搞定了。
```javascript
function extend(Chid,Parent){
	Child.prototype = new Parent(); 
}
```
当然他的缺点也很明显：
1 `子类同时继承了两个对象的属性，即添加到this的属性与原型链上的属性`;
2 `默认模式并不支持将参数传递到子构造函数中，而子构造函数又将参数传递到父构造函数中`;
考虑以下情况：
```javascript
function Parent(name){
	this.name = name || 'parent';
}
Parent.prototype.say = function(){
	console.log(this.name);
}

function Child(name){
}

extend(Child,Parent);

var child = new Child('child');
//由于这里'child'参数并不会传递到Parent构造方方法中
//虽然子构造方法可以将参数传递到父构造方方法，
//但是如果那样的话，在每次需要一个新的子对象时都必须重新执行这种继承机制，
//而且这种机制效率低下，原因在于最终会反复的重新创建父对象;
s.say();	// parent	
```
### 2 `借用构造方法模式`
```javascript
function Child(a,b,c,d){
	Parent.appliy(this,arguments);
}
```
当然这种模式的缺点也很明显：
只能继承this中的属性，而在prototype中的属性则不能继承；
PS:需要至于与上面默认模式的区别，1中的默认模式是继承的this中属性是对父级属性的引用，而2中借用构造方法则是对父类中this属性的拷贝，毕竟都已经在子类的构造方法中单独运行了。所以如果修改通过1继承而来的子类属性，父类的是属性会一样被修改，而2则不会;

### 3 `借用与设置模式`
其实就是将上面的2种模式综合一起。如下：
```javascript
function Child(a,b,c,d){
	Parent.apply(this,arguments);
}

Child.prototype = new Parent();
```
既可以解决上面1中子构造方法的传参问题，也可以解决2中的prototype属性没法继承问题；
但是新的问题是，这里面调用了`2次`父类的构造方法；

### 4 `共享原型模式`
所谓共享，就是将prototype进行复用；
```javascript
funciton extend(Child,Parent){
	Child.prototype = Parent.prototype;
}
```
当然它的问题也很明显，子类与父类的耦合太重，如果修改了子类的prototype，那么父类也很一样受到影响；

### 5 `临时构造方法模式`
都说大音希声，其实到最后最终的解决方案，也很简单。
从上面来看，共享原型的方式会存在耦合，那么我们解掉耦合不就over了么？
解决方案是，用一个临时的方法 F 的prototype来保存父类的prototype对象，而子类的prototype对象则选择指向临时方法F 的实例，那么这样如果你修改了子类的prototype对象，那么受到印象的也只是F的实例而已，不会影响到父类方法的prototype;
所以解决方案如下：
```javascript
function extend(Child,Parent){
	var F = function(){}
	F.prototype = Parent.prototype();
	Child.prototype = new F();
}
```
### 6 `终极解决方案`
在5的基础中进一步优化，重置constructor的指向与优化如下：
```javascript
var extend = (function(){
	var F = function(){};//单例，只需要申明一次即可
	return function(Child,Parent){
		F.prototype = Parent.prototype;
		Child.prototype = new F();
		Child.prototype.constructor = Child;
		Child.super = Parent;
	}
}());
```
## 5  关于json与jsonp的区别
json是一种数据交互的数据格式，jsonp为一种数据交互的跨域手段。
1 由于`跨域`由于同源策略的存在导致ajax在端口，域名，协议不同的情况下，无法完成请求；
2 但是发现页面在加载资源的时候img,script等，并不存在些等情况，唯一不同的是，加载资源是属于`GET`请求；
3 那么jsonp请应运而生了，我们可以动态的创建节点，然后动态加载资源，后端只需要把相应的数据以json格式返回回来即可，前端自行进行渲染；
具体实现如下:
服务端有js文件jsonp.js
返回数据格式如下:
```javascript
//这里的showName 就是前端传递过来的callback参数值
showName({
    "code": "CA1998",
    "price": 1780,
    "tickets": 5
});
```
前端调用文件
```javascript
	
	var showName = function(data){
		console.log(data);
	}
	
	var script = document.createElement("script");
    script.type = "text/javascript";
    script.src = "http://www.xx.com/jsonp.js?callback=showName";//通过此处将要回调的方法名传递给后端
    document.getElementsByTagName('head')[0].appendChild(script);
```
ps: ajax与jsonp都可以用来作异步操作。但ajax和jsonp其实本质上是不同的东西。ajax的核心是通过XmlHttpRequest获取非本页内容，而jsonp的核心则是动态添加\<script\>标签来调用服务器提供的js脚本。 

## 6 水平垂直居中问题
垂直居中分为定宽和不定宽；
```css
/*已知高度和宽度的水平垂直居中*/
#body-div{
    position:relative;
}
#body-div-container{
    width:100px;
    height:100px;
    position:absolute;
    top:50%;
    left:50%;
    margin:-50px 0 0 -50px;
}
```
```css
/*未知高度和宽度的水平垂直居中*/
#body-div{
    position:relative;
}
##body-div-container{
    position:absolute;
    margin:auto;
    top:0;
    right:0;
    bottom:0;
    left:0;
}
```
## 7 闭包
关于闭包，确实涉及的东西太多太多，那么最重要的两个问题：
1 闭包是怎么形成的
2 闭包的作用与优缺点
关于第一个闭包是怎么形成的，这里又涉及到`词法作用域`，`作用域链`，`执行上下文`等概念。

### 7.1 词法作用域是什么呢？
词法作用域就是`定义`在词法阶段的作用域。换句话说，`词法作用域`是由你在写代码时将变量和块作用域写在哪里来决定的，因此当词法分析器处理代码会保持`作用域`不变（大部分情况是这样的）。
考虑以下代码：
```javascript
function foo(a){
	var b = a * 2;
	function bar(c){
		 console.log(a,b,c);
	}
	bar(b*3);
}
foo(2);//2,4,12
```

{% asset_image 20170511185923465.png %}

三级嵌套的词法作用域，具体如下：
(1) 包含着整个全局作用域，其中只有一个标识符：foo。
(2) 包含着foo所创建的作用域，其中有三个标识符： a、bar、b。
(3) 包含着bar所创建的作用域，其中只有一个标识符：c 。

无论函数在哪里被调用，也无论它如何被调用，它的词法作用域都只由函数被声明时所处于的位置决定。

`当函数可以记住并访问所在词法作用域时，就产生了闭包，即使函数是在当前词法作用域之外执行；`
```javascript
function foo(){
	var a = 2;
	function bar(){
		console.log(a);
	}
	return bar;
}

var baz = foo();
baz();//2 ---朋友，这就是闭包的效果
```
我们可以看到 bar这个函数在 定时时的词法作用域以外的地方被调用。闭包使得函数可以继续访问定义时的词法作用域。
那么就很容易会发现，上面bar被调用时，需要用到变量a。那么js引擎会被a所在词法作用域压缩到bar方法的作用域链中，即使foo已经调用完成，内存中也不会释放对变量a的引用。需要注意的是，这里压缩到作用域链中的词法作用域中只保有bar所引用的变量，没有引用的变量是不会被内存保留下来的。
## 8 性能优化
关于性能优化，真的能说的太多了。
常见情况：
1) `请求合并`
2) `CSS Sprite，启用base64`
3) `延迟加载`
4) `CSS放在页面上面，JS资源放在页面最下面`
5) `异步请求`
6) `CDN加速`
7) `反向代理`
8) `启用gizp压缩`
9) `合理设置缓存`
10) `合理选择图片格式（包括：jpg，png，gif，svg，webp，iconfont等）`
11) `减少cookie，使用离线存储`
12) `页面静态化`
13) `DNS预加载`
14) `域名拆分`
## 9 浏览器缓存机制
{% asset_image 20170524131403402.png %}
## 10 VUE双向绑定实现原理
1 `Vue双向绑定原理`：

{% asset_image 20170525115839966.png %}

核心代码劫持监听对象数据， 需要考虑的是Dep对象被封装在闭包，属性调用get方法，会将对应的watcher添加到Dep.subs中，属性调用set方法是，调用dep.notify方法，会调用watcher中的update方法：
``` javascript
function observe(data) {
    if (!data || typeof data !== 'object') {
        return;
    }
    // 取出所有属性遍历
    Object.keys(data).forEach(function(key) {
        defineReactive(data, key, data[key]);
    });
};
function defineReactive(data, key, val) {
    var dep = new Dep();
    observe(val); // 监听子属性

    Object.defineProperty(data, key, {
        // ... 省略
        set: function(newVal) {
            if (val === newVal) return;
            console.log('哈哈哈，监听到值变化了 ', val, ' --> ', newVal);
            val = newVal;
            dep.notify(); // 通知所有订阅者
        }
    });
}

function Dep() {
    this.subs = [];
}
Dep.prototype = {
    addSub: function(sub) {
        this.subs.push(sub);
    },
    notify: function() {
        this.subs.forEach(function(sub) {
            sub.update();
        });
    }
};
// Watcher.js
function Watcher(vm, exp, cb) {
    this.cb = cb;
    this.vm = vm;
    this.exp = exp;
    // 此处为了触发属性的getter，从而在dep添加自己，结合Observer更易理解
    this.value = this.get(); 
}
Watcher.prototype = {
    update: function() {
        this.run();    // 属性值变化收到通知
    },
    run: function() {
        var value = this.get(); // 取到最新值
        var oldVal = this.value;
        if (value !== oldVal) {
            this.value = value;
            this.cb.call(this.vm, value, oldVal); // 执行Compile中绑定的回调，更新视图
        }
    },
    get: function() {
        Dep.target = this;    // 将当前订阅者指向自己
        var value = this.vm[exp];    // 触发getter，添加自己到属性订阅器中
        Dep.target = null;    // 添加完毕，重置
        return value;
    }
};
```
2 `Vue模版绑定渲染方案`

{% asset_image 20170525115850779.png %}
## 11 XSS与CSRF请求
## 12 http请求
## 13 html缓存
## 14 变量提升问题
## 15 ES6

引用:
https://segmentfault.com/a/1190000006599500