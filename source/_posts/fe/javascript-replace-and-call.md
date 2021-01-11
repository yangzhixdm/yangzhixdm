---
title: javascript中call与replace方法!
date: 2014-07-31 22:57:26
tags:
description: javascript细节。
---
### call方法

call 方法最大特点就是可以改变当前所调用的方法所在的作用域

在作为对象的调用过程中，很多时候需要用到this,
而在方法的调用中，this的值很多时候都是变动的
那么如果需要在下一个被调用的方法中的this值和调用的方法的this一样，换言之，
刚是要让他们处在同一个作用域中，怎么办呢

那么就可以采用call方法，当然也可以使用apply,也可以使用new
``` javascript
var node = {
  init: function(){
      console.log("init");
      console.log(this);
  }
};
function test (){
  console.log(this);
  this.init();
}
test.call(node);
```
### apply方法

apply的一些其他巧妙用法
细心的人可能已经察觉到,在我调用apply方法的时候,第一个参数是对象(this), 第二个参数是一个数组集合, 在调用Person的时候,他需要的不是一个数组,但是为什么他给我一个数组我仍然可以将数组解析为一个一个的参数,这个就是apply的一个巧妙的用处,可以将一个数组默认的转换为一个参数列表([param1,param2,param3] 转换为 param1,param2,param3) 这个如果让我们用程序来实现将数组的每一个项,来装换为参数的列表,可能都得费一会功夫,借助apply的这点特性,所以就有了以下高效率的方法:
a) Math.max 可以实现得到数组中最大的一项
因为Math.max 参数里面不支持Math.max([param1,param2]) 也就是数组
但是它支持Math.max(param1,param2,param3…),所以可以根据刚才apply的那个特点来解决 var max=Math.max.apply(null,array),这样轻易的可以得到一个数组中最大的一项(apply会将一个数组装换为一个参数接一个参数的传递给方法)
这块在调用的时候第一个参数给了一个null,这个是因为没有对象去调用这个方法,我只需要用这个方法帮我运算,得到返回的结果就行,.所以直接传递了一个null过去
b) Math.min  可以实现得到数组中最小的一项
同样和 max是一个思想 var min=Math.min.apply(null,array);
c)Array.prototype.push 可以实现两个数组合并
同样push方法没有提供push一个数组,但是它提供了push(param1,param,…paramN) 所以同样也可以通过apply来装换一下这个数组,即:
``` javascript
var arr1 = new Array("1","2","3"),
  arr2 = new Array("4","5","6");  
  Array.prototype.push.apply(arr1,arr2);  
```
也可以这样理解,arr1调用了push方法,参数是通过apply将数组装换为参数列表的集合.

### repalce方法
``` javascript
var reg = /([0-9]+$)|(([0-9]+\,)*([0-9]+\.)[0-9]$)/;
    var t = reg.exec("USD $100.00");
/*console.log(t);
console.log(reg.exec("USD $100.100"));
console.log(reg.exec("USD $100"));
console.log(reg.exec("USD $100.00"));
console.log(reg.exec("USD $0.01"));
console.log(reg.exec("USD $0.1"));*/
console.log(reg.exec("USD $100,555.1"));
var t = reg.exec("USD $100,555.1");
console.log(parseFloat(t[0]));


var number = "123123123.12";
number.replace(/([0-9]{0,3},)+\.[0-9]+/);
```
js replace 第二个参数为function()
参数最多个数根据第一个参数中的正则确定，换言之则是根据正则的分组数目确定.
第一个参数为匹配到的字符
第二个参数到倒数第三个参数为每一个分组所匹配到的字符
倒数第二个字符为匹配到的字符的起始位置
最后一个参数为被匹配的字符
``` javascript
/**
	 *	以下正则含有两个分组
	 * 
	 */
	String.prototype.capitalize = function(){
 
		return this.replace( /([a-z])([a-z])/g , function(m,a,b,p1,p2){
			return p1+p2.toUpperCase();
		});
	};
	String.prototype.capitalize2 = function(){
 
		return this.replace( /([a-z])([a-z])/g , function(m,a,p1,p2){
			return p1+p2.toUpperCase();
		});
	};
	String.prototype.capitalize3 = function(){
 
		return this.replace( /([a-z])([a-z])/g , function(m,p1,p2){
			return p1+p2.toUpperCase();
		});
	};
	String.prototype.capitalize4 = function(){
 
		return this.replace( /([a-z])([a-z])/g , function(m,p1){
			return p1.toUpperCase();
		});
	};
	String.prototype.capitalize5 = function(){
 
		return this.replace( /([a-z])([a-z])/g , function(m){
			return m;
		});
	};
	console.log("safsdafasdfasdfsd".capitalize());
	console.log("safsdafasdfasdfsd".capitalize2());
	console.log("safsdafasdfasdfsd".capitalize3());
	console.log("safsdafasdfasdfsd".capitalize4());
	console.log("safsdafasdfasdfsd".capitalize5());

	/**
	 *  以下正则没有分组
	 */
	String.prototype.capitalize6 = function(){
 
		return this.replace( /[a-z][a-z]/g , function(m,a,b,p1,p2){
			return m;
		});
	};

	String.prototype.capitalize7 = function(){
 
		return this.replace( /[a-z][a-z]/g , function(m,a,p1,p2){
			return m;
		});
	};
	String.prototype.capitalize8 = function(){
 
		return this.replace( /[a-z][a-z]/g , function(m,p1,p2){
			return m;
		});
	};
	String.prototype.capitalize9 = function(){
 
		return this.replace( /[a-z][a-z]/g , function(m,p1){
			return m;
		});
	};
	String.prototype.capitalize10 = function(){
 
		return this.replace( /[a-z][a-z]/g , function(m){
			return m;
		});

	};
	console.log("safsdafasdfasdfsd".capitalize6());
	console.log("safsdafasdfasdfsd".capitalize7());
	console.log("safsdafasdfasdfsd".capitalize8());
	console.log("safsdafasdfasdfsd".capitalize9());
	console.log("safsdafasdfasdfsd".capitalize10());
```
### js中的双叹号( !! )
经常看到这样的例子：
``` javascript
var a;var b = !!a;
```
a默认是undefined。!a是true，!!a则是false，所以b的值是false，而不再是undefined，也非其它值，主要是为后续判断提供便利。
!!一般用来将后面的表达式强制转换为布尔类型的数据（boolean），也就是只能是true或者false;
因为javascript是弱类型的语言（变量没有固定的数据类型）所以有时需要强制转换为相应的类型，类似的如:a=parseInt(“1234″)；a=”1234″-0 //转换为数字；b=1234+”” //转换为字符串；c=someObject.toString() //将对象转换为字符串
其中第1种、第4种为显式转换，2、3为隐式转换
布尔型的转换，javascript约定规则为
1) false、undefinded、null、0、”” 为 false
2) true、1、”somestring”、[Object] 为 true
对null与undefined等其他用隐式转换的值，用!操作符时都会产生true的结果，所以用两个感叹号的作用就在于将这些值转换为“等价”的布尔值；
``` javascript
function test(info,flag){
  console.log(!!flag);
}
test("dd","ddd");
test("dd","");
test("dd"," ");
test("dd");
test("dd",undefined);
test("dd",null);
```
### js中简易代码高亮效果，主要依靠正则表达式
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title></title>
<style type="text/css">
/*

*/
body{
	font-size:12px;
	line-height:1.8;
	font-family:'Courier New', Courier, monospace;
}
#area{
	width:320px;
	height:120px;
}
</style>
</head>
<body>

<textarea id="area">
body{
	font-size:12px;
	line-height:1.8;
	font-family:'微软雅黑';
}
/*
	web：http://www.dovapour.com.cn/
    mail：dovapour@gmail.com
  	QQ：362380046
*/
#area{
	width:320px;
	height:120px;
	line-height:1.5;
	font-family:"Courier New", Courier, monospace;
}
/*
ul{
	margin:0;
    padding:0;
    list-style:none;
}
table{
	border-collapse:collapse;
    border-spacing:0;
}
*/
</textarea>
<button id="btn">测试</button>
<div id="pre" style="color:#F0F;"></div>
<script type="text/javascript">
function $(id) {
	return document.getElementById(id);
};
$("btn").onclick=function(){
	var code = $("area").value;
	//处理注释：注释替换成 _数字_ 
	var startIdx = endIndex =-1;
	var at = 0;
	var commentList = [];
	while(true){
		startIndex = code.indexOf("/*",at);
		if(startIndex == -1)
			break;
		endIndex = code.indexOf("*/",startIndex);
		
		if(endIndex==-1)
			break;
		
		at = endIndex + 2;
		commentList.push(code.substring(startIndex,at));
		code = code.replace(commentList[commentList.length-1],"_"+(commentList.length-1)+"_");
	}
	
	//字符串
	code = code.replace(/(['"]).*\1/g,function(m){return "<span style=\"color:#060;\">"+m+"</span>"});
	
	//CSS样式值
	code = code.replace(/:(.+);/g,function(m,n){return ":<span style=\"color:#00F;\">"+n+"</span>;"});
	
	//CSS样式名称
	code=code.replace(/[{}]/g,function(m){
		if(m=="{"){
			return "{<span style=\"color:#006;\">";
		}else{
			return "</span>}";
		}
	});
	
	//注释
	code=code.replace(/_(\d+)_/g,function(m,n){return "<span style=\"color:#999;\">"+commentList[n]+"</span>"});
	
	//处理\t
	code=code.replace(/^(\t+)/gm,function(m){
		return (new Array(m.length+1)).join("    ");									
	});
	
	//处理空格
	code=code.replace(/^( +)/gm, function(m) {
		return (new Array(m.length + 1)).join(" ");
	});
	
	//处理换行
	code=code.replace(/\r?\n/g,"<br />");
	
	$("pre").innerHTML=code;
}
</script>
</body>
</html>
```