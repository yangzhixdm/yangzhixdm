---
title: jQuery Event add [ 源码分析 ]
date: 2014-08-28 14:05:19
tags:
description: jQuery源码分析-事件add
---
``` javascript
/*
 * Helper functions for managing events -- not part of the public interface.
 * Props to Dean Edwards' addEvent library for many of the ideas.
 */
jQuery.event = {

	add: function( elem, types, handler, data, selector ) {

		var elemData, eventHandle, events,
			t, tns, type, namespaces, handleObj,
			handleObjIn, quick, handlers, special;

		// Don't attach events to noData or text/comment nodes (allow plain objects tho)
		if ( elem.nodeType === 3 || elem.nodeType === 8 || !types || !handler || !(elemData = jQuery._data( elem )) ) {
			return;
		}

		// Caller can pass in an object of custom data in lieu of the handler
		// 这里在之前一直不明白为什么要这么做，原因就是这里的handler 可以是一个function,即我们平时所说的绑定的事件方法
		// 同时也可以是一个事件对象，也就是下面所说的handleObj,那么如果是在jQuery的内部是可以传递一个事件对象过来的
		if ( handler.handler ) {
			handleObjIn = handler;
			handler = handleObjIn.handler;
			selector = handleObjIn.selector;
		}

		// Make sure that the handler has a unique ID, used to find/remove it later
		// 这里分配guid
		if ( !handler.guid ) {
			handler.guid = jQuery.guid++;
		}

		// Init the element's event structure and main handler, if this is the first
		// 从缓存系统中，从缓存事件对象中获取events数组
		events = elemData.events;
		if ( !events ) {
			elemData.events = events = {};
		}
		//主监听函数，唯一一个绑定到dom元素的方法，会调用dispatch分配事件
		eventHandle = elemData.handle;
		if ( !eventHandle ) {
			elemData.handle = eventHandle = function( e ) {
				// Discard the second event of a jQuery.event.trigger() and
				// when an event is called after a page has unloaded
				//这里主要防止trigger 手动触发时的二次冒泡。这里在trigger中，会存在模拟冒泡的事件，
				//主要针对不能冒泡的事件如：focus 就只能模拟冒泡事件
				//如果已经冒泡过了，那么在这里就不需要再次执行了。
				return typeof jQuery !== "undefined" && (!e || jQuery.event.triggered !== e.type) ?
					jQuery.event.dispatch.apply( eventHandle.elem, arguments ) :
					undefined;
			};
			// Add elem as a property of the handle fn to prevent a memory leak with IE non-native events
			// 这里主要针对ie的内存问题
			eventHandle.elem = elem;
		}

		// Handle multiple events separated by a space
		// jQuery(...).bind("mouseover mouseout", fn);
		// 切割多个事件组合
		types = jQuery.trim( hoverHack(types) ).split( " " );
		for ( t = 0; t < types.length; t++ ) {
			//使用正则切割命名空间
			//	rtypenamespace = /^([^\.]*)?(?:\.(.+))?$/,
			//	如: click.namespace ,hover.namespace
			//	==> ["click.namespace", "click", "namespace", index: 0, input: "click.namespace"] 
			tns = rtypenamespace.exec( types[t] ) || [];
			type = tns[1];
			namespaces = ( tns[2] || "" ).split( "." ).sort();

			// If event changes its type, use the special event handlers for the changed type
			// 这里主要对事件进行修复，看是否在special中存有定义，如果有，刚使用special中的事件类型替换之
			// 主要是某些原生事件，对浏览器存在兼容问题，所以需要替换
			special = jQuery.event.special[ type ] || {};

			// If selector defined, determine special event api type, otherwise given type
			// 这里的selector为代理事件，如果存在selector，那么就存在冒泡事件
			// 对于delegateType主要是对原生不能冒泡的事件进行替换如：focus => focusin
			// 那么如果原生事件对于冒泡没有问题，那么检测绑定类型是否需要修复：如： mouseover ==> mouseenter 
			type = ( selector ? special.delegateType : special.bindType ) || type;

			// Update special based on newly reset type
			special = jQuery.event.special[ type ] || {};

			// handleObj is passed to all event handlers
			handleObj = jQuery.extend({
				type: type,
				origType: tns[1],
				data: data,
				handler: handler,
				guid: handler.guid,
				selector: selector,
				quick: selector && quickParse( selector ),
				namespace: namespaces.join(".")
			}, handleObjIn );

			// Init the event handler queue if we're the first
			handlers = events[ type ];
			if ( !handlers ) {
				handlers = events[ type ] = [];
				handlers.delegateCount = 0;

				// Only use addEventListener/attachEvent if the special events handler returns false
				if ( !special.setup || special.setup.call( elem, data, namespaces, eventHandle ) === false ) {
					// Bind the global event handler to the element
					if ( elem.addEventListener ) {
						elem.addEventListener( type, eventHandle, false );

					} else if ( elem.attachEvent ) {
						elem.attachEvent( "on" + type, eventHandle );
					}
				}
			}

			if ( special.add ) {
				special.add.call( elem, handleObj );

				if ( !handleObj.handler.guid ) {
					handleObj.handler.guid = handler.guid;
				}
			}

			// Add to the element's handler list, delegates in front
			if ( selector ) {
				handlers.splice( handlers.delegateCount++, 0, handleObj );
			} else {
				handlers.push( handleObj );
			}

			// Keep track of which events have ever been used, for event optimization
			jQuery.event.global[ type ] = true;
		}

		// Nullify elem to prevent memory leaks in IE
		elem = null;
	}
}
```