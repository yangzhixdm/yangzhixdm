---
title: getElementsByClassName与className和getAttribute!
date: 2014-08-30 18:03:34
tags:
---
对于javascript中的getElementsByClassName 在IE 6/7/8 不支持问题。
那么需要模拟出getElementsByClassName  需要采用className属性，这里就涉及到javascript中的getAttribute问题。
在ie 6/7 中，对于getAttribute存在Bug

需要采用className 获取如下：
``` javascript
var node = document.getElementById("test");
var name = node.className;
```
对于标准浏览器，则可以直接使用className 也可以使用 getAttritbute,结果一样:
``` javascript
node.getAttritbute("class");
```
但是如果是ie 6/7 ，则getAttribute()存在问题。

所以getElementsByClassName 可以使用如下方式:
``` javascript
getElementsByClazzName = document.getElementsClassName ? 
  function(name){
    return document.getElementsClassName(name);
  }:function(name){
    var nodes = document.getElementsByTagName("*"),
      result = [];
    for(var node in nodes){
      if(node.className && node.className.indexOf(name)){
        result.push(node);
      }	
    }
    return result;
  }
```
这里的实现方式比较简单。

1:常规属性建议使用：node.xxx。
2:自定义属性建议使用: node.getAttribute("xxxx")。
3:当获取的目标是 javascript 里的关键字时建议使用node.getAttribute("xxx")，如label中的for。
4:当获取的目标是保留字,如：class,请使用className代替。