---
title: Sizzle.find [ 源码分析 ]
date: 2014-08-28 15:03:04
tags:
description: jQuery中Sizzle引擎的find的方法分析。
---
``` javascript
Sizzle.find = function( expr, context, isXML ) {
	var set, i, len, match, type, left;
	//expr是否为空
	if ( !expr ) {
		return [];
	}

	for ( i = 0, len = Expr.order.length; i < len; i++ ) {
		type = Expr.order[i];
		//根据Expr.order,即Sizzle.selectors.order 的四种类型：ID,CLASS,NAME,TAG
		//与leftMatch确定 expr 的类型
		if ( (match = Expr.leftMatch[ type ].exec( expr )) ) {
			//left 官方说是为了确定之后的表达式是否有被转义，
			//人个认为这里的left基本没有意义
			left = match[1];
			match.splice( 1, 1 );

			if ( left.substr( left.length - 1 ) !== "\\" ) {
				//对match[1] 中的进行再次转换,如 \: 转义为 : ，\. 转为 .
				match[1] = (match[1] || "").replace( rBackslash, "" );
				//针对不同的type调用不同的find方法,返回查询结果，对应的元素集合
				set = Expr.find[ type ]( match, context, isXML );
				//查询结果不为空，那么删除掉已经查询过的表达式
				if ( set != null ) {
					expr = expr.replace( Expr.match[ type ], "" );
					break;
				}
			}
		}
	}
	//如果结果为空，那么采用如下方式寻找
	if ( !set ) {
		set = typeof context.getElementsByTagName !== "undefined" ?
			context.getElementsByTagName( "*" ) :
			[];
	}
	//返回临时结果
	return { set: set, expr: expr };
};
```

