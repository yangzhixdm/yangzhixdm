---
title: 计算屏幕中的各种高度
date: 2014-03-28 21:25:35
tags:
---
这几天对于js控制元素在浏览器端的高度问题，

有点麻烦，于是决定深入学习一番，经过一番努力测试，得如下结果：
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
  <meta http-equiv="Content-Type" content="text/html;charset=UTF-8" />
  <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script> 
  <title></title>
</head>
<body>
	<div style="height:800px;width:500px;border:10px solid red;" id="div1">
		<div style="margin-top:100px;height:800px;width:500px;border:10px solid blue;" id="div2">
		<div style="margin-top:100px;height:100px;width:500px;border:10px solid green;position:fixed;bottom:100px;" id="div3">
			<div ></div>
		</div>
	</div>

	<script type="text/javascript">
	$(window).scroll(function(){
		console.log("window-height:" + $(window).height());
		console.log("div1-height:" + $("#div1").height());
		console.log("div2-height:" + $("#div2").height());
		console.log("div3-height:" + $("#div3").height());
		console.log("div1到document顶部距离:div1-offset-height:" + $("#div1").offset().top);
		console.log("div2到document顶部距离:div2-offset-height:" + $("#div2").offset().top);
		console.log("div3到document顶部距离:div3-offset-height:" + $("#div3").offset().top);
		console.log("document被压缩的距:document-scrollTop:" + $(document).scrollTop());
		console.log("div3距离浏览器边框底部高度:" + ($(window).height() - $("#div3").height() - ($("#div3").offset().top- $(document).scrollTop())));
		console.log("div3距离浏览器边框底部真实高度(不包括div的上下border宽度):" + ($(window).height() - $("#div3").outerHeight() - ($("#div3").offset().top- $(document).scrollTop())));

	});
	</script>
</body>
</html>
```
结合前两天对scrollTop与offset的理解，终于对于浏览器中各种高度作出了一个总结。

哈哈，妈妈再也不用担心浏览器的高度问题了。

偶有所得，以记录之，谨防忘记。