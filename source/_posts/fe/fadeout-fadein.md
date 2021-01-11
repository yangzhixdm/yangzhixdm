---
title: fadeOut-fadeIn效果
date: 2014-04-21 10:44:51
tags:
description: fadeOut-fadeIn效果。
---
首先感谢司徒正美的文章!

在司徒大神的博客看到一个简单的渐入渐出的效果，完全采用js实现。

如下：
``` html
<!doctype html>
<html dir="ltr" lang="zh-CN" >
  <head>
    <meta charset="utf-8"/>
    <meta http-equiv="X-UA-Compatible" content="IE=8">      
    <title>相册</title>
  </head>
  <body>
    <div id="album" >
      <img src="http://images.cnblogs.com/cnblogs_com/rubylouvre/202906/t_type1.jpg" />
      <img src="http://images.cnblogs.com/cnblogs_com/rubylouvre/202906/t_type2.jpg" />
      <img src="http://images.cnblogs.com/cnblogs_com/rubylouvre/202906/t_type3.jpg" />
      <img src="http://images.cnblogs.com/cnblogs_com/rubylouvre/202906/t_type4.jpg" />
    </div>
    <style type="text/css">
      #album{
        position:relative;
        border:10px solid #000;
        width:85px;
        height:120px;
        overflow:hidden;
      }
      #album img{
        position:absolute;
        top:0;
        left:0;
        opacity:0;
        filter:alpha(opacity=0);
      }
    </style>
    <script type="text/javascript">
      var album = function(el){
        var node = (typeof el == "string")? document.getElementById(el):el;
        var images = node.getElementsByTagName("img");
        var length = images.length;
        for(var i=1;i<length;i++){
          images[i].opacity = 0;//为所有图片设置一个自定义属性opacity
        } 
        images[0].opacity = 0.99;
        images[0].style.display = "block";
        var current = 0;
        (function(){
          setTimeout(function(){
            var cOpacity = images[current].opacity,
              next = images[current+1]?current+1:0;//下一张图片的索引，如果没有就回到第一张
            var nOpacity = images[next].opacity;
            cOpacity-=.05;
            nOpacity+=.05;
            images[current].opacity = cOpacity;
            images[next].opacity = nOpacity;
            images[next].style.display = "block";//确保肯定有一张图片是可见的
            setOpacity(images[current]);
            setOpacity(images[next]);
            if(cOpacity<=0) {
              images[current].style.display = "none";
              current = next;
              setTimeout(arguments.callee,1000);
            }else{
              setTimeout(arguments.callee,50);
            }
          },100);
        })()
      }
      var setOpacity =function(obj) {
        if(obj.opacity>.99) {
            obj.opacity = .99;
        }
        obj.style.opacity = obj.opacity;
        obj.style.filter = "alpha(opacity=" + (obj.opacity*100) + ")";
      }
      window.onload = function(){
        var el = document.getElementById("album");
        album(el);
      }
    </script>
    <p>请稍等4秒………………</p>
  </body>
</html>
```
我一直很不明白，为什么这里的图片的效果一定需要等待4秒才会出现。于是，研究之，

发现，原来一切都是由于positon所造成的。由于定位是采用的absolute,所以导致了，最后一张图片会覆盖前面的图片。

那么在改变前面的图片的opacity的时候，所有的效果都会被最后的一张图给挡住.

如需要改变这种情况，只需要在初始化的时候，将所有的图的opacity都重置为0.

如下：
``` css
 #album img{
  position:absolute;
  top:0;
  left:0;
  opacity:0;
  filter:alpha(opacity=0);
}
```

这样重置之后，发现这个世界又开始清静了。

总体来说效果还是蛮简单的。

是的，很多人会说，干嘛不用jquery，jquery可能就是一句话的问题。

对于这个，不做解释。

偶有所得，以记录之，谨防忘记!