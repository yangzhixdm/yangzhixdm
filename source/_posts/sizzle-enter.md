---
title: sizzle入口源码分析
date: 2014-08-28 14:46:50
tags:
description: sizzle入口源码分析
---
``` javascript
var Sizzle = function( selector, context, results, seed ) {
	//context 默认为document,可以人为指定
	results = results || [];
	context = context || document;

	var origContext = context;
	//判断文档节点
	if ( context.nodeType !== 1 && context.nodeType !== 9 ) {
		return [];
	}
	//表达式是否为字符串
	if ( !selector || typeof selector !== "string" ) {
		return results;
	}
	//set: 种子集
	//checkSet: 种子集的副本
	//extra: 保存剩余的并联表达式，注意是并联表达式
	//ret: 初步返回的结果，数据类型为json，包含set与expr属性，set为种子集,expr为剩余的块表达式
	//cur: 块表达式关系符如：+ > ~ ，如果没有则默认为空格 " "
	//pop:弹出数组的最后一个变量
	//parts: 存储当前被分割的块表达式数组
	var m, set, checkSet, extra, ret, cur, pop, i,
		prune = true,
		contextXML = Sizzle.isXML( context ),
		parts = [],
		soFar = selector;

	// Reset the position of the chunker regexp (start from head)
	// 分割快表达式，对于碰到并联表达式，则暂时结束，将余下的并联表达式保存在extra中
	// 如#info .p,div.red > a
	do {
		chunker.exec( "" );
		m = chunker.exec( soFar );

		if ( m ) {
			soFar = m[3];

			parts.push( m[1] );
			//根据上面的chunker正则，如果存在并联表达式，那么exec之后，m[2]的值为 ","
			if ( m[2] ) {
				//m[3]为剩余的并联表达式
				extra = m[3];
				break;
			}
		}
	} while ( m );
	//会被切割为如下:
	//parts ["#info",".p"] ,extra : div.red > a
	//判断是否存在位置伪类 origPOS,如 :frist,:last 等，如果存在位置伪类，则采取自左向右搜索
	if ( parts.length > 1 && origPOS.exec( selector ) ) {

		if ( parts.length === 2 && Expr.relative[ parts[0] ] ) {
			set = posProcess( parts[0] + parts[1], context, seed );

		} else {
			set = Expr.relative[ parts[0] ] ?
				[ context ] :
				Sizzle( parts.shift(), context );

			while ( parts.length ) {
				selector = parts.shift();

				if ( Expr.relative[ selector ] ) {
					selector += parts.shift();
				}

				set = posProcess( selector, set, seed );
			}
		}

	} else {
		// Take a shortcut and set the context if the root selector is an ID
		// (but not if it'll be faster if the inner selector is an ID)
		// 这里主要修改context,如果表达式的开头为ID类型，且最后一个表达式非ID类型
		// 所有的查询，使用到sizzle.selector.find的，只有最后一个节点，其它的节点都可以采用关系来决定
		// 之所以只有第一个为id的时候可以修改context,且最后一个不能为ID，因为getElementById只在document中存在
		// 在element中不存在该方法，如最后一个元素为ID,那么直接就会报错
		// 那么将context修改为ID所在节点，用来提高效率
		// 如：$("#info .p"); 那么自动修改为如下： $(".p",$("#info"));
		if ( !seed && parts.length > 1 && context.nodeType === 9 && !contextXML &&
				Expr.match.ID.test(parts[0]) && !Expr.match.ID.test(parts[parts.length - 1]) ) {
			//调用find方法，查询parts中的第一个元素，返回临时结果
			//如以上的#info .p 那么会直接查询#info ,返回结果
			ret = Sizzle.find( parts.shift(), context, contextXML );
			//然后修改context
			context = ret.expr ?
				Sizzle.filter( ret.expr, ret.set )[0] :
				ret.set[0];
		}

		if ( context ) {
			//因为是采用自右向左的方式搜索，那么先获取出数组的最后一个元素,调用parts.pop();
			//再调用find方法查询
			ret = seed ?
				{ expr: parts.pop(), set: makeArray(seed) } :
				Sizzle.find( parts.pop(), parts.length === 1 && (parts[0] === "~" || parts[0] === "+") && context.parentNode ? context.parentNode : context, contextXML );
			//对结果进行过滤
			set = ret.expr ?
				Sizzle.filter( ret.expr, ret.set ) :
				ret.set;

			if ( parts.length > 0 ) {
				checkSet = makeArray( set );

			} else {
				prune = false;
			}
			//继续操作块表达式中最后一个元素左边的部分，如果存在，调用关系正则判断
			//如 1 .div.red > p,那么上面已经查询过p了，那么现在parts中的元素为['div.red','>'];
			//   2 .div.red p,那么上面已经查询过p了，那么现在parts中的元素为['div.red'];
			while ( parts.length ) {
				//取出最后一个元素，即为'>' ,如果为第二种情况，那么直接就是 'div.red'
				cur = parts.pop();
				pop = cur;
				//是否存在关系符号，如果为空格
				if ( !Expr.relative[ cur ] ) {
					//查询是否存在cur类型，如为第二种情况，那么给cur赋值为"",这时的pop 则为div.red
					cur = "";
				} else {
					//取出上一级元素,如为第一种情况，需要重新取最后一个元素，为div.red
					pop = parts.pop();
				}
				//如果pop为空，那么默认为document
				if ( pop == null ) {
					pop = context;
				}
				//调用关系正则判断,pop为左边元素的表达式，checkSet为待过滤元素，cur 为关系符
				//这里要强调的一点是，可以有人会发现，这里是直接调用，没有返回值，
				//那么过滤之后的得到的结果怎么返回呢？
				//这里是采用了javascript函数对数组的传递是传值的方式，调用的函数中修改checkSet的值
				//那么在后面去使用checkSet的值，也会变化
				Expr.relative[ cur ]( checkSet, pop, contextXML );
			}

		} else {
			checkSet = parts = [];
		}
	}

	if ( !checkSet ) {
		checkSet = set;
	}

	if ( !checkSet ) {
		Sizzle.error( cur || selector );
	}

	if ( toString.call(checkSet) === "[object Array]" ) {
		if ( !prune ) {
			results.push.apply( results, checkSet );

		} else if ( context && context.nodeType === 1 ) {
			for ( i = 0; checkSet[i] != null; i++ ) {
				if ( checkSet[i] && (checkSet[i] === true || checkSet[i].nodeType === 1 && Sizzle.contains(context, checkSet[i])) ) {
					results.push( set[i] );
				}
			}

		} else {
			for ( i = 0; checkSet[i] != null; i++ ) {
				if ( checkSet[i] && checkSet[i].nodeType === 1 ) {
					results.push( set[i] );
				}
			}
		}

	} else {
		makeArray( checkSet, results );
	}

	if ( extra ) {
		//存在并联表达式，递归调用
		Sizzle( extra, origContext, results, seed );
		//对结果排序去重，合并
		Sizzle.uniqueSort( results );
	}

	return results;
};
```