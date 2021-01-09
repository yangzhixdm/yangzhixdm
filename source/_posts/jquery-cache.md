---
title: jQuery缓存系统的前世今生
date: 2014-08-01 22:52:07
tags:
---
### 第一代事件系统
Eean wdwrad 大神的addEvent.js.当然最原始的jQuery事件系统也是照抄过来的.
``` javascript
function addEvent(element, type, handler) {
	// assign each event handler a unique ID
	if (!handler.$$guid) handler.$$guid = addEvent.guid++;
	// create a hash table of event types for the element
	if (!element.events) element.events = {};
	// create a hash table of event handlers for each element/event pair
	var handlers = element.events[type];
	if (!handlers) {
		handlers = element.events[type] = {};
		// store the existing event handler (if there is one)
		if (element["on" + type]) {
			handlers[0] = element["on" + type];
		}
	}
	// store the event handler in the hash table
	handlers[handler.$$guid] = handler;
	// assign a global event handler to do all the work
	element["on" + type] = handleEvent;
};
// a counter used to create unique IDs
addEvent.guid = 1;

function removeEvent(element, type, handler) {
	// delete the event handler from the hash table
	if (element.events && element.events[type]) {
		delete element.events[type][handler.$$guid];
	}
};

function handleEvent(event) {
	// grab the event object (IE uses a global event object)
	event = event || window.event;
	// get a reference to the hash table of event handlers
	var handlers = this.events[event.type];
	// execute each event handler
	for (var i in handlers) {
		this.$$handleEvent = handlers[i];
		this.$$handleEvent(event);
	}
};
```
当然，这由于是最初的版本，自然还没有引入缓存系统。由于是将回调事件handler直接绑定在元素上，可能引发循环引用，导致内存泄漏。

也有在iframe中事件对象不对的问题。

在ED大神的博客的第54条回复中，直接引发了jquery的缓存机制的产生，建议给元素分配一个UUID,所有的回调都放到一个对象中去存储。

看到司徒大神的一段对于oninput事件的处理：
``` javascript
if (W3C) { //先执行W3C
    element.addEventListener("input", updateVModel)
    data.rollback = function() {
      element.removeEventListener("input", updateVModel)
    }
  } else {
    removeFn = function(e) {
      if (e.propertyName === "value") {
        updateVModel()
      }
    }
    element.attachEvent("onpropertychange", removeFn)
    data.rollback = function() {
      element.detachEvent("onpropertychange", removeFn)
    }
  }

  if (DOC.documentMode === 9) { // IE9 无法在切剪中同步VM
    var selectionchange = function(e) {
      if (e.type === "focus") {
        DOC.addEventListener("selectionchange", updateVModel)
      } else {
        DOC.removeEventListener("selectionchange", updateVModel)
      }
    }
    element.addEventListener("focus", selectionchange)
    element.addEventListener("blur", selectionchange)
    var rollback = data.rollback
    data.rollback = function() {
      rollback()
      element.removeEventListener("focus", selectionchange)
      element.removeEventListener("blur", selectionchange)
    }
  }
}
```
想了很久，不知道此处的data何用，现在看到，不过是用来做数据缓存的，用来保存元素对象的rollback回调，而不用去直接绑定在元素自身。

防止循环引用的产生。
