---
title: js中array的sort()方法
date: 2014-02-20 10:15:28
tags:
description: 排序问题。
---
或许你一直在用javascript中的array的sort.

或许你一直相信它会给你正确的结果。

至少我曾经也是这样认为的，直到有一天，我看到了如下的代码 ：
``` javascript
[5,10,1].sort();
```
或许结果有点出人意料。结果如下：
``` javascript
[1,10,5]
```
仔细深究之后，发现原来默认的sort方法并不是按照整形数据来排序，而是用的字符串匹配方式。

换言之就是10 中的这个 1 导致了上面代码的错误。

当然，解决方法有很多，可以给sort方法中传入回调函数。
``` javascript
[5,10,1].sort(function(x,y){
        if(x>y) {return 1;
        }else{
            return -1
        }
    }
);
```
这样就能得到你预期的结果了。

偶然发现，以记录之，防止忘记。