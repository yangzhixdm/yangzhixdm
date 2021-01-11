---
title: requireJs入门
date: 2014-03-28 10:33:22
tags:
description: requireJs 学习。
---
也是久闻AMD大名。但一直没有时间去弄这些东西。

终于找了一些时间去了解一下：

关于js的模块设计就不用多说了，自然好处多多，很大程度上去优化结构代码，减少http请求次数，提升性能。

去网上找来了requireJs的api，感觉不错。http://makingmobile.org/docs/tools/requirejs-api-zh/

1.加载requireJ文件

在页面加入:
``` html
<script type="text/javascript"  data-main="app/app.js" src="require/require.js"></script>
```
这里的data-main表示你要加载的入口文件，一般很多人会取一个叫main的名字。这里习惯取了app。

这里的data-main实际上的一个模块id，可以省去js后缀，requireJS加载会自动补上。

2.配置入口文件，也就是这里的app.js
``` javascript
requirejs.config({
    baseUrl: 'js',
    paths: {
        app: 'app'
    }
});

require(["jquery","show","cat"],function($,show,cat){
    alert($("#tt").val());
    alert(show);
});
```

这里的baseUrl表示页面当前所在的目录。如baseUrl:'js'，即为当前页面同级的js目录.

paths定义一些路径，同样相对的也是当前引入requireJs的目录。

cat.js
``` javascript
define(['jquery'],function($) {
	alert($);
    return {
        num: "111",
        size: "123"
    }
});
```
show.js
``` javascript
define(['jquery'],function() {
    return {
        color: "black",
        size: "123"
    }
});
```
这里的define表示定于模块，可以给三个参数，

第一个参数表示模块ID,

第二个参数表示依赖的模块id,

第三个参数一般为执行方法，可以将依赖的模块以形参的方式传入进去。

如果只有两个参数，那么模块名将默认为文件名。

这里需要注意的是jquery在1.7.2之后的版本才支持自动AMD，所以如果用以前的版本需要手动define一下.