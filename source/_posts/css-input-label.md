---
title: CSS深入之label与input对齐
date: 2014-02-18 11:49:47
tags:
---
我想很多人都会碰到label与input 对齐的问题。

这个东西本身不难，但是要做到与IE这个东西兼容确实有点头疼。

参考各大门户网站的前端源码。 得一方法，以记录之：

html确实很简单：

``` html
<div class="login-box" id="login-id">
  <form id="login-form-id" class="login-form">
    <p>
      <label>帐号</label>
      <input name="username" value="name" type="text" class="normal"/>
      <small>输入用户帐号.</small>
    </p>
    <p>
      <label>密码</label>
      <input name="username" value="name" type="text" class="normal"/>
      <small>输入用户密码.</small>
    </p>
    <p>
      <input type="button" value="login"/>
      <input type="button" value="cancel"/>
    </p>
  </form>
</div>
```

CSS如下:
``` css
label{
	display: inline-block;
	padding: 0 10px;
	vertical-align: middle;
}
input{
	outline: none;
	border: 1px solid rgb(216, 216, 216);
	padding: 2px 10px 2px 10px;
}
input[type="text"]{
	height: 14px;
	line-height: 14px;
	border-radius: 5px;
	padding:10px 10px;
	vertical-align: middle;
	color:#666;
}
input[type="button"]{
	padding: 0px 10px;
	height:40px;
}


.login-box p{
	vertical-align:middle;
	padding:10px 10px;
}
```
那么这里最主要的问题就是
input中采用了paddng的方式。

用padding将input撑开，那么这时候input的高度就只有14px;然后再设置line-height为14px;

再加上vertical-align垂直居中，就能将input中的文字垂直居中。

需要将input与label垂直对齐，只需要将他们的父节点的vertical-align设置为middel.

再将height与line-height的高度设置为一致，当然必须要比input的实际高度高或者是相等。

这里说的是实际高度。

这里记录一下CSS盒子模型的高度问题:



