---
title: 关于跨域的问题
date: 2018-09-06 11:12:52
tags:
description: 跨域问题已经方案思考。
---

#### 跨域问题
由于浏览器安全限制，同源策略。在web中一个页面（资源）想要获取另一个页面（资源）上的数据，就要求两个页面(资源）来自同一个源。而源则是由协议，主机名，端口号组合而成的。所以一个页面要获取另一个页面的数据，就需要相同协议，相同主机名，相同端口才能进行访问。如果出现不同源的请求，则称为跨域请求。

#### 跨域方案
##### JSONP
> 利用script标签的 src属性加载资源没有同源策略的限制。

> 通过页面创建script标签来资源，而该资源会返回一段script代码，并传入数据，前端加载完成后会直接执行。前端可通过传递一个回调标记，后端使用该标记进行包裹要传入的数据，前端可以直接执行已定义好的对应方法。如：

``` js
var node = createElement('<script></script>');
node.src =  "http://www.xdmlib.com/demo/jsonp.php?callback=handler";

node.onload = function(){
  console.log('success');
}

function handler = function(data){
  console.log(data);
}
```
php 返回结果

``` js
handler({retcode: 0,retmsg: 'ok',data: []}
```

`需要注意的问题`

- 1 jsonp 中只能执行get请求
- 2 jsonp请求有可能产生csrf问题，需要做好验证，需要做好referer校验，以及token机制
- 3 handler参数做好过滤，防止xss

##### CORS
服务端header进行判断源（origin）是否为允许访问的域名，cors会先发送预检请求判定，预检请求一般为option类型，并且判断请求是否为简单请求或是复杂请求。

##### domain + iframe
> 多个子域之间之间iframe通间，可以使用document.domain设置为相同的域名， 来进行通信。

> 浏览器中不同域的框架之间是不能进行js的交互操作的，不同的框架之间（父子或同辈），是能够获取到彼此的window对象的，但是你拿不到window下面的任何属性和方法。这个时候，你想要进行在不同的iframe之间进行交互的话，那么可以使用docuemt.domain设置成相同的域名即可。如:

> git.xdmlib.com 目录下有一个页面a.html要访问 demo.xdmlib.com下面的b.html

> \<iframe src="http://demo.xdmlib.com/b.html" id="iframe"\/>
在a.html和b.html页面中都设置如下代码即可：
``` js
document.domain = "xdmlib.com";
```
注意点

> 如果想要在 iframe中进行发ajax请求，一样会被拦截。如果需要进行发ajax请求，则需要使用一个的隐藏的iframe来做中间代理。原理就是让这个iframe载入一个与你想要通过ajax获取数据的目标页面处在相同的域的页面，所以这个iframe中的页面是可以正常使用ajax去获取你要的数据的，然后就是通过我们刚刚讲得修改document.domain的方法，让我们能通过js完全控制这个iframe，这样我们就可以让iframe去发送ajax请求，然后收到的数据我们也可以获得了

##### postmessage/window.name
上面提到浏览器可以拿到不同框架的window对象，但是确实取不到window的对象，但是postmessage另外。所以就可以考虑使用postmessage来进行跨域。但存在兼容问题。

window.name则是由于窗口载入的所有的页面共享window.name的值。

##### 服务器代理
前端直接请求代理服务器，需要的跨越资源可以让服务端采用反向代理方式进行拉取，从而可以绕过跨域问题。

#### 跨域为什么不能防止csrf
大多情况下，跨域请求并不是被浏览器拦截掉，而是可以正常发送到服务端，只是返回来的数据，浏览器由于同源策略会自行拦截掉，不允许访问。即使在跨域的情况下，请求依旧会到达服务器，服务端的操作一样会执行，只是返回的数据客户端会被拦截而已。

但是在某些浏览器下，浏览器会在请求访问之前拦截掉。

#### 参考
[js中几种实用的跨域方法原理详解](https://www.cnblogs.com/2050/p/3191744.html)