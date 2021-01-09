---
title: vue源码阅读
date: 2019-08-3 10:57:52
tags: Vue
description: Vue 源码阅读方式。
---
### vue 源码阅读准备
1 下载vue源代码
https://github.com/vuejs/vue

2 开始安装依赖

``` javascript
  npm i
```

3 开启sourcemap模式
在package中dev命令下添加 --sourcemap
``` javascript
  "dev": "rollup -w -c scripts/config.js --environment TARGET:web-full-dev --sourcemap",
```

4 安装webpack-dev-server
添加start命令
``` javasctipt
  "start": "webpack-dev-server --config webpack.config.js",
```

5 修改example中页面的引用路径
``` html
<script src="../../dist/vue.js"></script>
```
6 启动访问demo
访问example页面例子即可
