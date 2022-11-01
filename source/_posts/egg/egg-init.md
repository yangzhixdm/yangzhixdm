---
title: Eggjs 学习1
date: 2021-01-12 17:20:30
tags: 记忆中的事
description: 最近开始学习Eggjs。
---

#### 安装
直接使用ts版本，根据官方提供的模版进行生成项目。
``` bash
npm init egg --type=ts
```

#### postmain模拟请求
postmain中模拟请求，会存在csrf限制，需要进行绕过。
首先在postman的tests添加如下代码：
```
var csrf_token = postman.getResponseCookie("csrfToken").value
postman.clearGlobalVariable("csrfToken");
postman.setGlobalVariable("csrfToken", csrf_token);
```

然后再postman中添加 `x-csrf-token` 头即可。
{% asset_image postmain-csrf.png %}


#### egg 自动生成typings文件
新的方式：
```
# page
"egg": {
    "declarations": true
  },
```