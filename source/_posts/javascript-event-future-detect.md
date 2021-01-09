---
title: javascript事件特性检测代替浏览器嗅探
date: 2014-07-31 23:10:17
tags:
---
参考文章：http://perfectionkills.com/detecting-event-support-without-browser-sniffing/

事件特性检测：

1 标准浏览器,包括ie，判断指定的元素是否存在 "on" + event 属性.

2 在firefox 下，上述办法不适用，解决办法为：

创建div对象，给div元素添加指定的属性，属性名为 "on" + event,之后判断添加属性的类型，如果browser 支持添加的事件，type of 之后应该为 "function"．
``` javascript
function isEventSupported(eventName, element) {

  element = element || document.createElement(TAGNAMES[eventName] || 'div');
  eventName = 'on' + eventName;
  
  var isSupported = (eventName in element);
  
  if (!isSupported) {
  // if it has no `setAttribute` (i.e. doesn't implement Node interface), try generic element
    if (!element.setAttribute) {
      element = document.createElement('div');
    }
    if (element.setAttribute && element.removeAttribute) {
      element.setAttribute(eventName, 'return;');
      isSupported = typeof element[eventName] == 'function';

    // if property was created, "remove it" (by setting value to `undefined`)
      if (typeof element[eventName] != 'undefined') {
        element[eventName] = undefined;
      }

      element.removeAttribute(eventName);
    }
  }
  element = null;
  return isSupported;
}
```

