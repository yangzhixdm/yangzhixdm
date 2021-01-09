---
title: 突然想到的一个东西？测试一下
date: 2013-12-31 14:15:58
tags:
---
写段代码吧！作为是2013年的结束吧，也算是2014的开始！
关于jquery的click函数。也算是关于闭包的一点理解吧！
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8" />
	<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
  <title></title>
</head>
<body>
	<input type="button" value="test" id="in"/>
	<script type="text/javascript">
	(function($){
		var t = {
			show:function(){
				 var _root	= this,
					 _t	= 1;
				 $("#in").click(function(){
						 var i = 0;
						 i++;
						 _t++;
						 console.log(" _root: " + _root);
						 console.log("_t: " + _t);
						 console.log("i: " + i);
				 });
			}
		};
		t.show();
	})(jQuery);
	</script>
</body>
</html>
```
如果不想整天纠结各大浏览器的兼容性的问题的话,那么你的CSS该重置了！

css 重置样式:
``` css
/*css重置样式*/
html, body, div, span,   
h1, h2, h3, h4, h5, h6, p, blockquote, pre,  
a, abbr, acronym, address, big, cite, code,  
img, ins, kbd, q, s, samp,  
small, strike, strong,   
dl, dt, dd, ol, ul, li,  
fieldset, form, label, legend,  
table, caption, tbody, tfoot, thead, tr, th, td {  
    margin: 0;  
    padding: 0;  
    border: 0;  
    outline: 0;  
    font-size: 100%;  
    vertical-align: baselinebaseline;  
    background: transparent;  
}  
body {  
    line-height: 1;  
}  
ol, ul {  
    list-style: none;  
}  
blockquote, q {  
    quotes: none;  
}  
blockquote:before, blockquote:after,  
q:before, q:after {  
    content: '';  
    content: none;  
}  
  
table {  
    border-collapse: collapse;  
    border-spacing: 0;  
}
```






