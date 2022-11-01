---
title: 渲染阻塞和解析阻塞
date: 2019-02-28
tags: browser
description: render-blocking和parser-blocking的区别
---

#### 有言在先
浏览器在加载 css 和 js 时，会造成阻塞，但是这两种阻塞又是有区别的，但是js会进行解析阻塞，css 则是渲染阻塞。

#### 介绍
以下内容来自[StackOverflow](https://stackoverflow.com/questions/37759321/parser-blocking-vs-render-blocking)
![render-blocking和parser-blocking](https://raw.githubusercontent.com/helloyangzhi/learn/master/parse-blocking-and-render-blocking.png)