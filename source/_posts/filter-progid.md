---
title: filter:progid:DXImageTransform.Microsoft.AlphaImageLoader
date: 2014-04-16 15:56:59
tags:
description: 浏览器如何显示本地图片，主要针对IE.
---
很多时候需要将图片显示在网页上，一般都会这样做，如下:
``` html
<img src="xxx.jpg"/>
```

是的，这样是可以做到，但是如果我要将本地的图片显示到页面上呢？你可能会说，我还有办法，如下:
``` html
<img src="C:\Users\Administrator\Desktop\xxx.jpg"/>
```

是的这样是可以？在你的本地确实能够完美的显示出来。

但是可惜的是，如果将你的网页放到服务器上，发现，额，怎么什么都没有了?

原来这一切在ie下都是不支持的直接去显示本地图片。至于在chrome下，比较easy，这里就不多话了，主要说说IE。

既然不支持，那当我们需要做图片的缩略显示的时候，怎么弄呢？

好家伙，那就用ie提供的css渲染吧。
### filter: progid

AlphaImageLoader：在元素的背景和内容之间插入一张图片，并提供对此图片的剪切和改变尺寸的操作。如果载入的是PNG(Portable Network Graphics)格式，则0%-100%的透明度也被提供。

语法格式：filter : progid:DXImageTransform.Microsoft.AlphaImageLoader ( enabled=bEnabled , sizingMethod=sSize , src=sURL )

enabled：可选项，布尔值(Boolean)。设置或检索滤镜是否激活。 true：默认值。滤镜激活。 false：滤镜被禁止。
sizingMethod：可选项。字符串(String)。设置或检索滤镜作用的对象的图片在对象容器边界内的显示方式。
1）crop：剪切图片以适应对象尺寸。
2）image：默认值。增大或减小对象的尺寸边界以适应图片的尺寸。
3）scale：缩放图片以适应对象的尺寸边界。
src：必选项。字符串(String)。使用绝对或相对 url 地址指定背景图像。假如忽略此参数，滤镜将不会作用。

给个demo:
``` javascript
this.select(); 
this.blur();
path = document.selection.createRange().text; 
var node = imageLi.find("div:eq("+_step+")");
node.css({"filter":"progid:DXImageTransform.Microsoft.AlphaImageLoader(sizingMethod=scale)"});
node[0].filters.item("DXImageTransform.Microsoft.AlphaImageLoader").src = path;
node[0].style.width = 150 +"px";
node[0].style.height = 100 +"px";
node.css({"display":"inline-block","margin":"5px;"});

```
### filter:alpha

关于在ie下的透明度问题,如下:

语法格式:
``` css
filter:Alpha(Opacity=opacity,　FinishOpacity=finishopacity，Style=style,　StartX=startX，StartY=startY，FinishX=finishX，FinishY=finishY)

filter:Alpha(Opacity="50",FinishOpacity="100",Style="3");
```

opacity:值0-100,0表示完全透明，100表示完全不透明， 这个玩过ps 的朋友应该都蝗

FinishOpacity: 表示结束处的透明度.

style: 1 ，2， 3 ;1 表示线行，2表示圆形，中心发散，3 表示星形中心发散。

在标准浏览器下：直接opacity:0.5 就可以了。