---
title: javascript针对IE的文档加载IEContentLoaded
date: 2014-08-01 01:02:11
tags:
description: 哈哈哈，IE一直是开发人员的痛。
---
1. IEContendLoaded.js

它的原理是，在IE下，DOM的某些方法只有在DOM解析完成后才可以调用，doScroll就是这样一个方法，反过来当能调用doScroll的时候即是DOM解析完成之时。在jquery的源码中也能看到如此对IE的特殊处理。
``` javascript
/*
 *
 * IEContentLoaded.js
 *
 * Author: Diego Perini (diego.perini at gmail.com) NWBOX S.r.l.
 * Summary: DOMContentLoaded emulation for IE browsers
 * Updated: 05/10/2007
 * License: GPL
 * Version: TBD
 *
 * Copyright (C) 2007 Diego Perini & NWBOX S.r.l.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://javascript.nwbox.com/IEContentLoaded/GNU_GPL.txt.
 *
 */

// @w	window reference
// @fn	function reference
function IEContentLoaded (w, fn) {
	var d = w.document, done = false,
	// only fire once
	init = function () {
		if (!done) {
			done = true;
			fn();
		}
	};
	// polling for no errors
	(function () {
		try {
			// throws errors until after ondocumentready
			d.documentElement.doScroll('left');
		} catch (e) {
			setTimeout(arguments.callee, 50);
			return;
		}
		// no errors, fire
		init();
	})();
	// trying to always fire before onload
	d.onreadystatechange = function() {
		if (d.readyState == 'complete') {
			d.onreadystatechange = null;
			init();
		}
	};
}
```
测试页面：

``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>IEContentLoaded - TEST CASE</title>
<meta name="author" content="Diego Perini" />
<meta name="copyright" content="Copyright (c) 2007, Diego Perini" />

<!-- body background set to lime -->
<style type="text/css">
/*<![CDATA[*/

	body { background-color: lime; }

/*]]>*/
</style>

<!-- load iecontentloaded script -->
<script type="text/javascript" src="iecontentloaded.js">
</script>

<!-- body background set to green -->
<script type="text/javascript">
//<![CDATA[

	var $tt = (new Date).getTime();
	function timeElapsed(t) { return ((new Date()).getTime() - t); }

	// only start this for Internet Explorer

	if (document.attachEvent && !window.opera) {
		IEContentLoaded(window,
			function () {
				document.body.style.backgroundColor = 'green';
				window.status += ' * IEContentLoaded ' + timeElapsed($tt) + ' ms.';
				alert("fn run")
			}
		);
	}

//]]>
</script>
</head>
<body>

<h1>IEContentLoaded - TEST CASE</h1>

<p><big>Back to <a href="index.html">IEContentLoaded INDEX</a>.</big></p>

<p>The objective of this test is to evaluate the detection reliability of this method.</p>
<p>In this specific case we are interested in determining when all the nodes in the<br />
requested page have been loaded and parsed in the DOM and available to scripts.</p>
<p>The fact that images exists or not in the page shouldn't affect this test.</p>

<p>The test case executes these steps, in sequence, at different times:</p>

<ol>
 <li>INLINE STYLE is applied and the BODY background color is set to "lime"</li>
 <li>Ending BODY SCRIPT is executed and the background color is set to "red"</li>
 <li>IEContentLoaded function is fired and the background color is set to "green"</li>
</ol>

<p>The page should have a "green" background color after being loaded completely.<br />
If that is not the case then something went wrong in my script or in your browser.</p>

<hr />
<img src="run.jpg" alt="A test image for IEContentLoaded" />
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<!-- body background set to red -->
<script type="text/javascript">
//<![CDATA[

	document.body.style.backgroundColor = 'red';

//]]>
</script>

</body>
</html>
```
测试页面2:
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>IEContentLoaded - TEST CASE</title>
<meta name="author" content="Diego Perini" />
<meta name="copyright" content="Copyright (c) 2007, Diego Perini" />

<!-- body background set to lime -->
<style type="text/css">
/*<![CDATA[*/

	body { background-color: lime; }

/*]]>*/
</style>

<!-- load iecontentloaded script -->
<script type="text/javascript" src="iecontentloaded.js">
</script>

<!-- body background set to green -->
<script type="text/javascript">
//<![CDATA[

	var $tt = (new Date).getTime();
	function timeElapsed(t) { return ((new Date()).getTime() - t); }

	// only start this for Internet Explorer

	if (document.attachEvent && !window.opera) {
		IEContentLoaded(window,
			function () {
				document.body.style.backgroundColor = 'green';
				window.status += ' * IEContentLoaded ' + timeElapsed($tt) + ' ms.';
			}
		);
	}

//]]>
</script>
</head>
<body>

<h1>IEContentLoaded - TEST CASE</h1>

<p><big>Back to <a href="index.html">IEContentLoaded INDEX</a>.</big></p>

<p>The objective of this test is to evaluate the detection reliability of this method.</p>
<p>In this specific case we are interested in determining when all the nodes in the<br />
requested page have been loaded and parsed in the DOM and available to scripts.</p>
<p>The fact that images exists or not in the page shouldn't affect this test.</p>

<p>The test case executes these steps, in sequence, at different times:</p>

<ol>
 <li>INLINE STYLE is applied and the BODY background color is set to "lime"</li>
 <li>Ending BODY SCRIPT is executed and the background color is set to "red"</li>
 <li>IEContentLoaded function is fired and the background color is set to "green"</li>
</ol>

<p>The page should have a "green" background color after being loaded completely.<br />
If that is not the case then something went wrong in my script or in your browser.</p>

<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<h4>This filler ensure exceeding the default 4Kb output caching of Apache.</h4>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
<span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span><span>test test test</span>
    
<hr />

<!-- body background set to red -->
<script type="text/javascript">
//<![CDATA[

	document.body.style.backgroundColor = 'red';

//]]>
</script>

</body>
</html>

 
```