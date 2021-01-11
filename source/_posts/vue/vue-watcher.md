---
title: Vue 中 Watcher 的一些想法
date: 2019-08-20 14:22:55
tags: Vue
description: Vue 源码篇中对于Watcher的一些想法。
---
关于Vue中 watcher和 dep的关系，以及 watcher的批量更新机制，思考了一段时间，最终绘制成一张图，如下：

![Watcher绑定机制](/postimg/20210111758443315.png)

