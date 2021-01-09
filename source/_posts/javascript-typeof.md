---
title: javascript中typeof唯一一个应用
date: 2014-08-01 00:57:26
tags:
---
对于javascript中typeof唯一一个应用，就是用来判断变量是否被定声明，或者初始化。
``` javascript
if(typeof val === "undefined"){
	//do something
}
```

以下为typeof对不同的类型的变量的值：
``` javascript
var o = ["foo",new String("foo"),1.2,new Number(1.2),true,new Boolean(true),new  Date(),new Error(),[1,2,3],new Array(1,2,3),new Function(""),/abc/g,new RegExp("meow"),{},new Object(),undefined,null];

for(var i = 0 ; i < o.length ; i ++){
    console.log("type of [ o" + i + "] : " + typeof o[i] );
}
var name = new Number(1.2);
console.log(typeof name);

/*
JavaScript 类型表格

Value               Class      Type
-----------华丽的分割线--------------

"foo"               String     string
new String("foo")   String     object
1.2                 Number     number
new Number(1.2)     Number     object
true                Boolean    boolean
new Boolean(true)   Boolean    object
new Date()          Date       object
new Error()         Error      object
[1,2,3]             Array      object
new Array(1, 2, 3)  Array      object
new Function("")    Function   function
/abc/g              RegExp     object (function in Nitro/V8)
new RegExp("meow")  RegExp     object (function in Nitro/V8)
{}                  Object     object
new Object()        Object     object

-----------华丽的分割线--------------
```





