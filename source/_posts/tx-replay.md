---
title: 腾讯前端回复效果
date: 2014-04-01 13:18:30
tags:
---
平日喜欢关注一些新闻，经常会浏览腾讯网！

但是很少看到评论的回复。与是仔细寻找之，发现原来是通过一个页面的弹出遮罩来显示的。

与是仔细研究一翻，自己模拟出来一个，感觉还行，兼容ie 8/chrome/firefox.

思路：主页面和遮罩是两个不同的页面，遮罩通过iframe 利用js加载进来。 

代码如下：

index.html
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script> 
	<title>append-iframe</title>
	<style type="text/css">
	html,body{height:100%;padding:0px;margin:0px;}
	</style>
</head>
<body>
	<div><input type="button" value="show" id="show-id"/></div>
	<div><input type="button" value="test" id="test-id"/></div>
	<script type="text/javascript">
	$("#show-id").on("click",function(){
		$(top.document.body).append('<iframe src="subiframe.html" id="np-pop-iframe" allowtransparency="true" frameborder="0" scrolling="no" style="width: 100%; z-index: 9999; position: fixed; top: 0px; left: 0px; border: none; overflow: hidden; height: 100%;" data-nick="" data-pic="" data-id="" parentid=""></iframe>');
	});
	$("#test-id").on("click",function(){
		alert("test");
	});
	</script>
</body>
</html>
```

subiframe.html:
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script> 
	<title>Document</title>
	<style type="text/css">
	html,body{height:100%;padding:0px;margin:0px;}
	.np-popframe-bg-opacity{
		filter: alpha(opacity=50);
		-moz-opacity: 0.5;
		opacity: 0.5;
	}
	.wrapper{
		position: absolute;
		border:1px solid red;
		width:800px;
		height: 800px;
		background: #fff;
		top:1000px;
		left:500px;

	}
	</style>
</head>
<body class="iframe-body" style="background-image: none; width: 100%; height: 100%; background-position: initial initial; background-repeat: initial initial;">
	<div class="np-popframe-bg-opacity" style="height: 100%; width:100%; background-color:#000;"></div>
	<div class="np-popframe-bg" id="np-popframe-bg-id" style="height: 100%; width:100%; position:absolute; top:0px; left:0px;"></div>
	<div class="wrapper" id="wrapper-id"></div>
</body>
<script type="text/javascript">
	(function(){
		$("#wrapper-id").animate({top:"100px"},"slow");
		$("#np-popframe-bg-id").on("click",function(){
			//$(window.parent.document.getElementById("np-pop-iframe")).remove();

			$("#np-pop-iframe",window.parent.document).remove();
		});
	})(jQuery);
</script>
</html>
```
这里需要用一到滤镜。让加载来的遮罩保持透明.

1 np-popframe-bg-id 的作用主要是显示评论框的外部区域。当鼠标点击该区域的时候，移除iframe.

2 div 需要设置100%的高度，需要在前面加上 html,body{height:100%;}的css，否则无效

3 还有一点就是绝对定位的absolute.

偶有所得，以记录之，谨防忘记！