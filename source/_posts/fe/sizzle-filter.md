---
title: Sizzle.filter [ 源码分析 ]
date: 2014-08-27 21:04:46
tags:
description: jQuery中Sizzle引擎的filter的方法分析。
---
最近一直在研究Sizzle选择器,对于其中的原理确实不得不佩服!

Sizzle中的filter方法，主要负责块表达式过滤元素集合，在方法内部会调用Sizzle.selector.fitler方法执行过滤操作。

Sizzle.filter主要分5个关键步骤：

1 使用LeftMatch确定表达式类型。

2 调用Sizzle.selectors.preFilter预过虑函数，执行过滤前的修正。

3 调用Sizzle.selectors.filter[ type ] 中对应的过滤函数，执行过滤操作，如果返回false,刚将元素集合中的对应位置的元素替换为false;

4 删除表达式中已过滤的部分

5 重复1 - 4 步骤

源码如下：
``` javascript
Sizzle.filter = function( expr, set, inplace, not ) {
	//expr 块表达式
	//set 待过滤的元素集合
	//inplace 是否替换，如果为true,那么当set中的元素与expr不匹配时，刚将不匹配的元素替换为false,
	//否则，构造新的数组，只保留匹配的元素
	//not: 如果为true,则去掉匹配元素，保留不匹配元素，如果为false,则保留匹配元素，去掉不匹配元素
	var match, anyFound,
		type, found, item, filter, left,
		i, pass,
		old = expr,
		result = [],
		curLoop = set,
		isXMLFilter = set && set[0] && Sizzle.isXML( set[0] );
	//while 循环，无限循环用expr过滤set元素，直到expr为空，因为在最后会将已经匹配过的部分删除，所以expr会越来越短
	while ( expr && set.length ) {
		for ( type in Expr.filter ) {
			//这里用来确定表达式类型，根据leftMatch正则
			//这里是match正则，LeftMatch只是要该正则加上了前缀和后缀
			/*
			match: {
				ID: /#((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,
				CLASS: /\.((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,
				NAME: /\[name=['"]*((?:[\w\u00c0-\uFFFF\-]|\\.)+)['"]*\]/,
				ATTR: /\[\s*((?:[\w\u00c0-\uFFFF\-]|\\.)+)\s*(?:(\S?=)\s*(?:(['"])(.*?)\3|(#?(?:[\w\u00c0-\uFFFF\-]|\\.)*)|)|)\s*\]/,
				TAG: /^((?:[\w\u00c0-\uFFFF\*\-]|\\.)+)/,
				CHILD: /:(only|nth|last|first)-child(?:\(\s*(even|odd|(?:[+\-]?\d+|(?:[+\-]?\d*)?n\s*(?:[+\-]\s*\d+)?))\s*\))?/,
				POS: /:(nth|eq|gt|lt|first|last|even|odd)(?:\((\d*)\))?(?=[^\-]|$)/,
				PSEUDO: /:((?:[\w\u00c0-\uFFFF\-]|\\.)+)(?:\((['"]?)((?:\([^\)]+\)|[^\(\)]*)+)\2\))?/
			},
			*/

			if ( (match = Expr.leftMatch[ type ].exec( expr )) != null && match[2] ) {
				filter = Expr.filter[ type ];
				//确定前缀
				left = match[1];

				anyFound = false;

				match.splice(1,1);
				//这里官方说是匹配 \\ 如果是 \\ 那么表示后面的元素都是被转义，刚不需要再去过滤，
				//不过个人觉得这个地方似乎无效的  left.subster(left.length - 1) 
				//这个字符的长度只会是1 怎么会和 \\　相等呢
				if ( left.substr( left.length - 1 ) === "\\" ) {
					continue;
				}
				//重置result为空数组，用于缩小候选集，result用来存放通过过滤的元素
				if ( curLoop === result ) {
					result = [];
				}

				if ( Expr.preFilter[ type ] ) {
					//这里会进行预过滤操作，主要对表达式进行修改正，如转义\\,取出空格等
					//针对不同和类型，会有不同的方式，详情可以见preFilter方法
					match = Expr.preFilter[ type ]( match, curLoop, inplace, result, not, isXMLFilter );
					/*match:
						1 true: 继续需要执行过滤，尚不到执行过虑函数的时候，直接执行continue,不再去调用filter方法。如pos,child
						2 false: 已经执行了过滤，缩小了候选集如：CLASS
						3 字符串: 修正之后的过滤参数，后面会继续调用对应的过虑函数
					*/

					if ( !match ) {
						anyFound = found = true;

					} else if ( match === true ) {
						continue;
					}
				}
				//返回的是修正的过滤参数，刚继续执行
				if ( match ) {
					//遍历候选集中的元素
					for ( i = 0; (item = curLoop[i]) != null; i++ ) {
						//元素存在
						if ( item ) {
							//对其进行过滤，返回结果
							found = filter( item, match, i, curLoop );
							//这里与　not 进行　异或　操作
							pass = not ^ found;
							//为替换模式，并且found 不为空
							if ( inplace && found != null ) {
								//如果通过，那么为true
								if ( pass ) {
									anyFound = true;
								} else {
									//否则将元素替换为false
									curLoop[i] = false;
								}

							} else if ( pass ) {
								//这要需要注意的是既然pass为真了，　那么就不需要判断了found,因为pass为found与not异或的结果
								//如果通过了，并且为非替换模式，那么将元素放入到新的数组result中
								result.push( item );
								anyFound = true;
							}
						}
					}
				}
				//found不为undefined
				if ( found !== undefined ) {
					if ( !inplace ) {
						//非替换模式，将结果复制给curLoop,
						curLoop = result;
					}
					//删除已经过滤过了的部分表达式
					expr = expr.replace( Expr.match[ type ], "" );
					//不匹配，返回空
					if ( !anyFound ) {
						return [];
					}
					//这里的break,说明已经进行过一次过虑，已经找到对应的type,可以选择跳出当前type的循环，
					break;
				}
			}
		}

		// 这里主要是匹配，如果最后过滤的表达式没有变化，那么认为过滤表达式有问题
		if ( expr === old ) {
			if ( anyFound == null ) {
				Sizzle.error( expr );

			} else {
				break;
			}
		}
		//备份expr,继续循环
		old = expr;
	}
	//返回过滤的结果
	return curLoop;
};
```