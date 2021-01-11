---
title: Sizzle.selectors.relative [ 源码分析 ]
date: 2014-08-24 11:00:00 
tags:
description: jQuery中Sizzle引擎的selectors 分析。
---
1 jQuery 对象Sizzle.selectors.relative中存放了块间关系符和对应的块间关系过滤函数，称为“块间关系过滤函数集” 块间关系符共有4种，其含义和过滤方式如图所示。
![](/postimg/20140824102652300.png)

在函数Sizzle( selector, context, results, seed )从右向左进行过滤时，块间关系过滤函数被
调用，用于检查映射集checkSet中的元素是否匹配块间关系符左侧的块表达式。调用时的参
数格式为：
``` javascript
Sizzle.selectors.relative[ 块间关系符 cur  ](  映射集 checkSet,  左侧块表达式pop, 
contextXML );
```


块间关系过滤函数接受3个参数：
‰ ‰参数checkSet：映射集，对该元素集合执行过滤操作。
‰ ‰参数part：大多数情况下是块间关系符左侧的块表达式，该参数也可以是DOM元素。
‰ ‰参数isXML：布尔值，指示是否运行在一个XML文档中。
块间关系过滤函数实现的3个关键步骤如下：
1）遍历映射集checkSet。
2）按照块间关系符查找每个元素的兄弟元素、父元素或祖先元素。
3）检查找到的元素是否匹配参数part，并替换映射集checkSet中对应位置的元素。
a. 如果参数part是标签，则检查找到的元素其节点名称nodeName是否与之相等，
如果相等则替换为找到的元素，不相等则替换为false。
b. 如果参数part是DOM元素，则检查找到的元素是否与之相等，如果相等则替换
为true，不相等则替换为false。
c. 如果参数part是非标签字符串，则调用方法Sizzle.filter( selector, set, inplace, not )过滤。
也就是说，遍历结束后，映射集checkSet中的元素可能会是兄弟元素、父元素、
祖先元素、true或false。

1 块间关系符"+"匹配选择器"prev + next"，即匹配所有紧接在元素prev后的兄弟元素
next。例如，$("div + span")、$(".lastdiv + span")。对于从右向左的查找方式，则是检查元
素next之前的兄弟元素是否匹配块表达式prev。

``` javascript
var Expr = Sizzle.selectors = {
	relative: {
		"+": function(checkSet, part){
			//检查参数是否为字符串
			var isPartStr = typeof part === "string",
			//指示参数part是否为标签字符串
			isTag = isPartStr && !rNonWord.test( part ),
			//isPartStrNotTag：指示参数part是否是非标签字符串。
			isPartStrNotTag = isPartStr && !isTag;

			if ( isTag ) {
			part = part.toLowerCase();
			}
			/*
			遍历映射集checkSet，查找每个元素的前一个兄弟元素，并替换映
			射集checkSet中对应位置的元素，有以下3个逻辑分支：
			1 如果未找到兄弟元素，则替换为false。
			2 如果找到了兄弟元素，并且参数part是标签，则检查兄弟元素的节点名称nodeName
			是否与之相等，如果相等则替换为兄弟元素，不相等则替换为false。
			3 如果找到了兄弟元素，并且参数part是DOM元素，则检查二者是否相等，如果相等
			则替换为true，不相等则替换为false。
			因此，在遍历结束后，映射集checkSet中的元素可能会是兄弟元素、true或false。
						*/
			for ( var i = 0, l = checkSet.length, elem; i < l; i++ ) {
				if ( (elem = checkSet[i]) ) {
					/*在遍历兄弟元素的同时过滤掉非元素节点，并且只要取到一个兄弟元素就
退出while循环。*/
					while ( (elem = elem.previousSibling) && elem.nodeType !== 1 ) {

					}

		 			checkSet[i] = isPartStrNotTag || elem && elem.node
					Name.toLowerCase() === part ?
					elem || false :
					elem === part; 
				}
			}
			/*
			如果参数part是非标签字符串，则调用方法Sizzle.filter( selector, 
			set, inplace, not )过滤映射集checkSet。对于参数part是标签和DOM元素的情况，在前面遍
			历映射集checkSet时已经处理过了。
			*/
			if ( isPartStrNotTag ) {
				Sizzle.filter( part, checkSet, true );
			}
		},
	},
};
```
2 块间关系符">"用于选择器"parent > child"，即匹配父元素parent下的子元素child。
例如，$("div + span")、$(".lastdiv + span")。对于从右向左的查找方式，则是检查子元素
child的父元素是否匹配块表达式parent。
``` javascript
var Expr = Sizzle.selectors = {
	relative: {
		">": function( checkSet, part ) {
			var elem,
			isPartStr = typeof part === "string",
			i = 0,
			l = checkSet.length;
			/*
			如果参数part是标签，则遍历映射集checkSet，查找每个元素的
			父元素，并检查父元素的节点名称nodeName是否与参数part相等，如果相等则替换映射集
			checkSet中对应位置的元素为父元素，不相等则替换为false。 
			*/
			if ( isPartStr && !rNonWord.test( part ) ) {
				part = part.toLowerCase();

				for ( ; i < l; i++ ) {
					elem = checkSet[i];

					if ( elem ) {
						var parent = elem.parentNode;
						checkSet[i] = parent.nodeName.toLowerCase() === part ? 
						parent : false;
					}
				}
			} else {
			/*
			如果参数part不是标签，则可能是非标签字符串或DOM元素，同
			样遍历映射集checkSet，查找每个元素的父元素，并替换映射集checkSet中对应位置的元
			素，在这个过程中有以下2个逻辑分支：
			1 如果参数part是非标签字符串，则在遍历映射集checkSet的过程中，替换映射集
			checkSet中对应位置的元素为父元素，遍历结束后调用方法Sizzle.filter( selector, set, 
			inplace, not )过滤映射集checkSet。
			2 如果参数part是元素，则在遍历映射集checkSet时，检查每个元素的父元素是否与
			之相等，如果相等则替换映射集checkSet中对应位置的元素为true，不相等则替换为
			false。
			因此，在遍历结束后，映射集checkSet中的元素可能会是父亲元素、true或false。
			*/
				for ( ; i < l; i++ ) {
					elem = checkSet[i];

					if ( elem ) {
						checkSet[i] = isPartStr ?
						elem.parentNode :
						elem.parentNode === part;
					}
				}

				if ( isPartStr ) {
					Sizzle.filter( part, checkSet, true );
				}
			}
		},
	},
};
```
3 块间关系符""用于选择器"ancestor descendant"，即匹配祖先元素ancestor的所有后代
元素descendant。例如，$("div button")、$("div .btn")。对于从右向左的查找方式，则是检
查后代元素descendant的祖先元素是否匹配块表达式ancestor。
``` javascript
var Expr = Sizzle.selectors = {
	relative: {
		"": function(checkSet, part, isXML){
			var nodeCheck,
			doneName = done++,
			checkFn = dirCheck;
			/*
			1 如果参数part是非标签字符串或DOM元素，则调用函数dirCheck()过滤映射集
			checkSet。
			2 如果参数part是标签，则调用函数dirNodeCheck()过滤映射集checkSet。
			调用函数dirCheck()和dirNodeCheck()时的参数格式为：
			checkFn( 方向 "parentNode/previousSibling", 块表达式 part, 缓存计数器 doneName, 映
			射集 checkSet, nodeCheck, isXML )
			函数dirCheck()和dirNodeCheck()会遍历映射集checkSet，查找每个元素的祖先元素，
			并检查是否有祖先元素匹配参数part，同时替换映射集checkSet中对应位置的元素。
			*/
			if ( typeof part === "string" && !rNonWord.test( part ) ) {
				part = part.toLowerCase();
				nodeCheck = part;
				checkFn = dirNodeCheck;
			}

			checkFn( "parentNode", part, doneName, checkSet, nodeCheck, isXML );
		},
	},
};
```
4 块间关系符"～"用于选择器"prev～siblings"，即匹配元素prev之后的所有兄弟元
素siblings。例如，$('div～p')。对于从右向左的查找方式，则是检查元素siblings之前的
兄弟元素是否匹配块表达式prev。
Sizzle.selectors.relative["～"]( checkSet, part )的源码实现与Sizzle.selectors.relative[""]
( checkSet, part )几乎一样，两者的区别仅仅在于调用函数dirCheck()和dirNodeCheck()时第
一个参数的值不同，前者是"previousSibling"，后者则是"parentNode"。
相关代码如下所示：
``` javascript
var Expr = Sizzle.selectors = {
	relative: {
		"~": function( checkSet, part, isXML ) {
			var nodeCheck,
			doneName = done++,
			checkFn = dirCheck;

			if ( typeof part === "string" && !rNonWord.test( part ) ) {
				part = part.toLowerCase();
				nodeCheck = part;
				checkFn = dirNodeCheck;
			}

			checkFn( "previousSibling", part, doneName, checkSet, nodeCheck, 
			isXML );
		}
	},
};
```