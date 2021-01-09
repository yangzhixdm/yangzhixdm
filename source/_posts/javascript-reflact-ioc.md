---
title: 反射与依赖注入
date: 2015-05-30 09:25:32
tags:
---
对于javascript中的反射的理解，一直都是认为，利用数组对回调函数进行保存，之后在适当的时刻利用call或是apply 方法，对回调进行调用即可，一般如下操作：

首先定义两个方法：
``` javascript
var service = function() {
    return { name: 'Service' };
}
var router = function() {
    return { name: 'Router' };
}
```
我们有另一个函数需要用到这两个模块。
``` javascript
var doSomething = function(other) {
    var s = service();
    var r = router();
};
```

当然，我们希望能够能够使用依赖注入的方式去做，把该控制权交给计算机去做，而不是人为的去调用如下：
``` javascript
var doSomething = injector.resolve('router,,service', function(a, b, c) {
    expect(a().name).to.be('Router');
    expect(b).to.be('Other');
    expect(c().name).to.be('Service');
});
doSomething("Other");
```

那么我们可以造出一个反射方法如下：
``` javascript
var injector ={
  dependencies: {},
  register: function(key, value) {
      this.dependencies[key] = value;
  },
  resolve:function(deps, func, scope) {
      var args = [];
    for(var i=0; i<deps.length, d=deps[i]; i++) {
      if(this.dependencies[d]) {
        args.push(this.dependencies[d]);
      } else {
        throw new Error('Can\'t resolve ' + d);
      }
    }
    return function() {
      func.apply(scope || {}, args.concat(Array.prototype.slice.call(arguments, 0)));
    }
  }
	};
```
如上代码,dependencies 用来保存回调函数集合，resolve用来调用。

这也算是一个比较成熟ok的想法。

但是依旧存在几点问题：

1 resolve 在进行调用时，deps参数列表顺序必须保持一致。
2 这一点有点牵强，但是也算。在调用时，需要再一次的输入形参，而不能直接调用。

那么为了解决以上问题，给出以下解决方案：
``` javascript
var injector ={
  dependencies: {},
  register: function(key, value) {
    this.dependencies[key] = value;
  },
  resolve: function() {
    var func, deps, scope, args = [], self = this;
    if(typeof arguments[0] === 'string') {
      func = arguments[1];
      deps = arguments[0].replace(/ /g, '').split(',');
      scope = arguments[2] || {};
    } else {
      func = arguments[0];
      deps = func.toString().match(/^function\s*[^\(]*\(\s*([^\)]*)\)/m)[1].replace(/ /g, '').split(',');
      scope = arguments[1] || {};
    }
    return function() {
      var a = Array.prototype.slice.call(arguments, 0);
      for(var i=0; i<deps.length; i++) {
        var d = deps[i];
        args.push(self.dependencies[d] && d != '' ? self.dependencies[d] : a.shift());
      }
      func.apply(scope || {}, args);
    }
  }
};
```

利用正则对代码进行解析，解析出function 列表参数，再进行一一自动匹配传值，那么即可解决，顺序必须一直问题，当然这也是当然最热mvvm框架AngularJs采取的方式。
调用方式可以如下：
``` javascript
injector.resolve(['service,,router', function(service, router) {

}]);
```
你可能注意到在第一个参数后面有两个逗号——注意

这不是笔误。空值实际上代表“Other”参数（占位符）。这显示了我们是如何控制参数顺序的。
最后还有一种方式，直接注入scope ，也就是直接注入作用域，那么作用域被注入，也就不存在上述的传参顺序问题
因为不在需要传递参数，直接可以从作用域中访问到。
``` javascript
 var injector = {
  dependencies: {},
  register: function(key, value) {
    this.dependencies[key] = value;
  },
  resolve: function(deps, func, scope) {
    var args = [];
    scope = scope || {};
    for(var i=0; i<deps.length, d=deps[i]; i++) {
        if(this.dependencies[d]) {
            scope[d] = this.dependencies[d];
        } else {
            throw new Error('Can\'t resolve ' + d);
        }
    }
    return function() {
        func.apply(scope || {}, Array.prototype.slice.call(arguments, 0));
    }        
  }
}

var doSomething = injector.resolve(['service', 'router'], function(other) {
  expect(this.service().name).to.be('Service');
  expect(this.router().name).to.be('Router');
  expect(other).to.be('Other');
});
doSomething("Other");
```
javascript反射依赖注入新的理解，记录之，谨防忘记.