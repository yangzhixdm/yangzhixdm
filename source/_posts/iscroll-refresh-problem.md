---
title: IScroll 图片导致refresh不准确问题
date: 2017-01-06 18:11:17
tags:
---
最近同事在用iscrolljs做app端的滚动。
页面中的内容是采用ajax加载出来的，但是在上拉的时候，不知道为什么总是拉不到底部，而查看容器的高度又是正确的。
每次加载内容之后，也调用了refresh方法，但是就是拉不到底部。
仔细查看之后，发现原来是加载出来的内容当中，包含有图片。
而这里每次调用refresh方法的时候，这些图片并没有加载完成，也就是没有触发onload事件，这个时候调用refresh方法，自然计算的高度是有问题的。所以导致了每次上拉的时候，拉不到底部。
想了一下并没有特别好的解决方案，最后只能检测每一张图片的加载情况，每次图片加载完之后，调用一次refresh方法，如下：

``` javascript
//解决图片高度产生的iscroll.refresh不准确产生的bug
varimgList = document.getElementsByClassName('imgauto');
for(var i = 0 ; i < imgList.length ;i++){
    var img = imgList[i];
    img.onload = (function(img){
        return function(){
            iscroll.refresh();
            img.onload = null;
        }
    })(img);
}
```
以上解决方案，能够解决目前的问题，但是存在问题是每一个图片都会调用一次refresh方法，会造成资源的浪费。
但由于不同的设备图片的高度会不一致，所以图片的高度无法预先设定，故容器的实际高度是无法计算的。所以目前并没有更好的解决方案。如果谁有方案，欢迎沟通。