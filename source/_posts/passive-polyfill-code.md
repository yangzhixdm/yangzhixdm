---
title: passive polyfill 代码
date: 2017-07-12 20:30:07
tags:
description: passive polyfill 代码实现逻辑。
---

看到了一段对Google的 Passive event listeners 特性进行 polyfill 的代码，用的太巧妙了，值得细读，简直帅到没朋友

``` javascript
// Test via a getter in the options object to see 
// if the passive property is accessed
var supportsPassive = false;
try {
  var opts = Object.defineProperty({}, 'passive', {
    get: function() {
      supportsPassive = true;
    }
  });
  window.addEventListener("test", null, opts);
} catch (e) {}

// Use our detect's results. 
// passive applied if supported, capture will be false either way.
elem.addEventListener(
  'touchstart',
  fn,
  supportsPassive ? { passive: true } : false
); 

//mdn object.defineProperty
https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/Object/defineProperty
```