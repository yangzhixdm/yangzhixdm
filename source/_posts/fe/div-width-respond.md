---
title: div宽度自适应
date: 2014-03-24 14:07:07
tags:
---
偶尔朋友得一题目，关于div的宽度自适应.

原文出处:http://chaoqunz.blog.163.com/blog/static/6154877720096308422202/

感谢原创作者!

1.宽度自适应的条件
自适应宽度的层不要指定宽度和float属性
自适应宽度的层物理的位置放在最下面
自适应的div中不能出现“width=100%”，否则在ie6中会出现错位现象，ie7，ie8，ff3，chrome都正常。
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">   
<html xmlns="http://www.w3.org/1999/xhtml">   
<head>   
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"  />   
 <title>DIV自适应宽度</title>   
 <style type="text/css">   
 .L { width:100px; background-color:blue; float:left;}  
 .R { width:100px; background-color:blue; float:right;}  
 .M { background-color:red; }  
  </style>   
   </head>   
  <body>   
  <div style="width:500px;"> 
     <div class="L">left:100px</div> 
     <div class="R">right:100px</div> 
      <div class="M">自适应宽度</div> 
   </div> 
  </body>   
</html> 
```
2.div的默认（不写）宽度是100%，但不撑大父框架，写出width=100%是如果再使用了margin或padding就会将父框架撑大。 table默认宽度为内容的长度，与div不同，因此最好不要混着用，否者这个宽度自适应的方法在ie6下就有问题了。但高版本的浏览器都没问题，是不是 说以后大家都会忽略这两者之间的差异呢？

如果父框架设定了固定宽度，那么子div的宽度设为width=100%，即使设定了margin,padding或border都不会撑大父框架。 

