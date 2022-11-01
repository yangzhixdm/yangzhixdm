---
title: 了解 CSRF 
date: 2018-10-07
tags:
---

#### CSRF
csrf（cross site request forgery）跨站请求伪造，攻击者在自己的网站（也就是钓鱼网站）伪造请求发送给别人（正常）的服务器。

#### 产生原因
关于session，与cookie，以及http通信协议。
http协议需要使用seeion以及cookie来保持用户的回话过程，所以会在每次回话的过程中，在cookie中带有session的标记sessionId，通俗称为令牌。
而http在每次请求的过程中，都会默认带上cookie（当然cookie是可以手动禁用的，如果禁用的话，会在url中携带sessionId作为参数）。

正常请求一般都会验证登录态，所以一般无法成功。但是你可能在某些时候已经登录了当前的网站，然后又点开了另外一个网站（钓鱼网站），或许你点击了其中的一个图片或者连接，然后它会向你之前登录的网站发送一个请求，而在这时，http协议是正常会带上cookie里的信息，由于你之前正常登录过，所以请求就被正常执行了。

#### CSRF过程

{% asset_image csrf-progress.jpg %}

#### 如何防范？
- referer校验
- token校验
- 验证码校验
- 手机验证码验证
- cookie域限定
- post请求