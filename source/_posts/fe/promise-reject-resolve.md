---
title: Promise中的resolve与reject回调问题
date: 2018-10-06
tags: Promise
---
##### Promise
``` js
var fn = function(num) {
    return new Promise(function(resolve, reject) {
        console.log(resolve.toString());
        if (typeof num == 'number') {
            resolve(num);
        } else {
            reject('TypeError');
        }
    })
}

fn(1).then(
    function(){
        console.log('111')
    }
);
```

之前一直以为 上面的resolve方法就是下面then中传递的方法

``` js
    function(){
        console.log('111')
    }
```
但是根据上面的打印输出的是 [native code]，其实这里内部已经编译好的方法。
根据猜想，这里应该是内部实现的一个异步方法，方法内部去异步调用你then方法中传递的回调方法。
如果你没传递的话，那么就是一个空函数。
那么这样逻辑就能够理清楚了。

then方法会返回一个新的Promise对象是什么情况呢？
> then在执行之后，会将需要执行的回调方法推到队列任务中，并返回一个新的 Promise，这样就可以在后面进行链式调用。同时新的Promise 和之前的Promise的状态是没有关系的。