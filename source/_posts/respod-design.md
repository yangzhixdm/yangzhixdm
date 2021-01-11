---
title: 响应式设计入门
date: 2014-04-03 13:18:55
tags:
description: 响应式的开发的简单demo.
---
html:
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta name="viewport" content="initial-scale=1.0,width=device-width" />
	<title>And the winner isn't</title>
	<link href="css/main-pecent.css" rel="stylesheet" type="text/css" />
</head>
<body>
	<div id="wrapper">
		<!-- the header and navigation -->
		<div id="header">
			<div id="navigation">
				<ul>
					<li><a href="#"><img src="images/01.jpg" alt="image"/></a></li>
					<li><a href="#">navigation2</a></li>
					<li><a href="#">navigation3</a></li>
					<li><a href="#">navigation4</a></li>
					<li><a href="#">navigation5</a></li>
					<li><a href="#">navigation6</a></li>
					<li><a href="#">navigation7</a></li>
					<li><a href="#">navigation8</a></li>
					<li><a href="#">navigation9</a></li>
				</ul>
			</div>
		</div>
		<!-- the content -->
		<div id="content">
			<p><img src="images/01.jpg" alt="imgage"/></p>
		</div>
		<!-- the sidebar -->
		<div id="sidebar">
			<p><img src="images/01.jpg" alt="imgage"/>dfdfdf</p>
		</div>
		<!-- the footer -->
		<div id="footer">
			<p>Here is the footer</p>
		</div>
	</div>
</body>
</html>
```

css:
``` css
#wrapper {
	margin-right: auto;
	margin-left: auto;
	width: 96%; /* 最外层 DIV */
}
#header {
	margin-right: 10px;
	margin-left: 10px;
	width: 97.9166667%; /* 940 ÷ 960 */
}

#navigation {
	padding-bottom: 25px;
	margin-top: 26px;
	margin-left: -1.0416667%; /* 10 ÷ 960 */
	padding-right: 1.0416667%; /* 10 ÷ 960 */
	padding-left: 1.0416667%; /* 10 ÷ 960 */
	width: 97.9166667%; /* 940 ÷ 960 */
	background-repeat: repeat-x;
	background-image: url(../img/atwiNavBg.png);
	border-bottom-color: #bfbfbf;
	border-bottom-style: double; border-bottom-width: 4px;
}
#navigation ul li {
	display: inline-block;
	margin-right: 2.6595745%; /* 25 ÷ 940 */
}

#navigation ul li a {
	height: 42px;
	line-height: 42px;
	text-decoration: none;
	text-transform: uppercase;
	font-family: Arial, "Lucida Grande", Verdana, sans-serif;
	font-size: 1.6875em;
	color: black;
}
img{
	width: 10%;
}
#navigation ul li a img{
	
}

#content {
	margin-top: 58px;
	margin-right: 1.43266475%;
	float: right;
	width: 72.7083333%; /* 698 ÷ 960 */
}
#sidebar {
	border-right-color: #e8e8e8;
	border-right-style: solid;
	border-right-width: 2px;
	margin-top: 58px;
	margin-right: 0.458715596%;
	margin-left: 0.458715596%;
	float: left;
	width: 22.7083333%; /* 218 ÷ 960 */
}
#footer {
	float: left;
	margin-top: 20px;
	margin-right: 10px;
	margin-left: 10px;
	clear: both;
	width: 97.9166667%; /* 940 ÷ 960 */
}
```
