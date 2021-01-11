---
title: javascript事件绑定
date: 2014-07-31 23:08:11
tags:
description: 事件绑定。
---
1  事件绑定

标准浏览器同时支持冒泡和捕捉两种方式。
``` javascript
var node = document.getElementById("input-id");
var divNode = document.getElementById("div-id");
node.addEventListener("click",function(){
  alert("input-id");
},false);
divNode.addEventListener("click",function(){
  alert("div-id");
},false);
```

IE 只支持冒泡方式
需要注意的是,IE 绑定事件需要加上on前缀
``` javascript
var node = document.getElementById("ie-input-id");
var divNode = document.getElementById("ie-div-id");
node.attachEvent("onclick",function(){
  alert("ie-input-id");
});
divNode.attachEvent("onclick",function(){
  alert("ie-div-id");
});
```
没办法，在IE的大市场份额下, IE 最终赢得了主动权
于是有了下面的解决方法：
``` javascript
var E = {
  on:function(el,type,fn){
    el.addEventListener?
      el.addEventListener(type,fn,false):el.attachEvent("on" + type,function(){
        fn.call(el,window.event);
        //这里使用call主要是为了保证作用域this的正确性
      })
  }
};

function handler(e){
  //alert(e.getAttribute("alt"));
  alert(this == window);
  alert(e.srcElement);
}
E.on(document.getElementById("call-id"),"click",handler);
```



