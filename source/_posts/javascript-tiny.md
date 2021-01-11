---
title: 小东西!
date: 2014-02-19 16:40:35
tags:
description: 开发必备。
---
### jquery在线引用
``` javascript
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script>  
```

### 检测浏览器器型
``` javascript
if($.browser.msie) { 
			alert("本站已放弃对IE的支持,请换用别的浏览器,谢谢!");
			window.opener=null;window.open('','_self');window.close();
	} else if($.browser.safari) { 
	} else if($.browser.mozilla) { 
	} else if($.browser.opera) { 
}
```

### font
``` css
font-style font-variant font-weight font-size line-height font-family
font{12px/normal microsoft Yahei}
```

### background:transparent

此属性在ie8 会导致设置属性设置background失效.
在tr中是设置了此属性，之后想在改变background.在ie8中无效。

### ajax请求判断，判断ajax是否成功。
``` javascript
$(function(){
  $("#btn").on("click",function(){
    $("#btn-box-content").html("html");
    var timer = setTimeout(function(){
    $("#btn-box-content").html("");
      alert("请求超时!");
    },5000);
    $.post("http://shenzhen.8684.cn/city1.php?t=16/p2",{},function(){
      clearTimeout(timer);
    });
  });
});
```

### jquery end()
``` html
	<div>
		<ul class="nav-box">
			<li data="0">tab1</li>
			<li data="1">tab2</li>
		</ul>
		<div id="content-box">
			<div class="box-content">
				table1
			</div>
			<div class="box-content">
				table2
			</div>
		</div>
	</div>
```
``` javascript
$(function(){
	$(".nav-box li").on("click",function(){
		var index = $(this).attr("data");
		$("#content-box").find(".box-content").eq(index).siblings().removeClass("active").end().addClass("active");
	});
});
```
end()方法会结束当前最近的一搜索，并将匹配元素集还原为之前的状态。

大多数 jQuery 的遍历方法会操作一个 jQuery 对象实例，并生成一个匹配不同 DOM 元素集的新对象。

当发生这种情况时，应该会把新的元素集推入维持在对象中的堆栈内。每次成功的筛选方法调用都会把新元素推入堆栈中。如果我们需要老的元素集，可以使用 end() 从堆栈中弹出新集合。

### checkbox选中
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
	<title>Document</title>
	<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script>   
	<style>
	.pre{
			border:1px solid red;
	}
	</style>
</head>
<body>
	<div class="pre">
		<input type="checkbox"/>test
	</div>
	<script type="text/javascript">
	$(".pre").on("click",function(){
		var node = $(this).find("input");
		if(node.attr("checked") == "checked"){
			node.attr("checked",false);
		}else{
			node.attr("checked",true);
		}
	});
	</script>
</body>
</html>
```





