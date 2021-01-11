---
title: 静态资源缓存的处理方式
date: 2016-11-11 19:02:06
tags:
description: 前端工程一直是很重要的一个方面，静态资源缓存的处理方式
---
原问题是： 
一般浏览器对静态资源的缓存导致的更新不及时问题，我们是怎么处理的？有哪几种方式？ 
为什么会产生这些方式？（其实就是各种方式的优劣）

之前大家可能都知道 一般的公司对于静态资源以及缓存的处理方式无非就这么几种。
1 在静态资源后面加一个版本号 v=1.111
![这里写图片描述](/postimg/20161111184734681.png)
类似于上面这种方式。

2 为了准确的确定文件是否修改，将后面的版本号修改为文件摘要（主要根据文件内容生成的一个值）。
![这里写图片描述](/postimg/20161111184849839.png)
类似于上面这种，后面的红框表示的部分就是根据文件的摘要生成的key.

3 直接将资源文件名使用文件摘要或者说某个固定的字符串加上一个文件摘要拼接成一个文件名。
![这里写图片描述](/postimg/20161111185400095.png)
类似上面这种方式，最后面红圈内表示的代码是根据文件摘要来生成的，这里需要区别和第二种方式，第二种方式是拿来放在url后面作为一个参数，但文件名没有改变。而这里直接选择修改了文件名。

（彩蛋：有意思的，找了几个TX的网站，发现其实并不是所有的网站都采用了最后一种方式。我想应该技术都是用来追求完美的，但实现还是人实现的，毕竟人的天性是喜欢偷懒的。）

那么问题来了？ 以上三种方式的区别是什么？为什么最后会最终演变为第三种方式？

1 第一种方式，需要维护版本号，如果在一个文件中，存在多个资源，那么没有被修改过的资源文件也会被修改版本号，导致不必要的资源加载。（当然，如果需要加上时间戳之类的，就已经不属于第一个的范围了）

2 第二种方式，可以精确的发现哪一个文件被修改过。从而要求客户端进行重新加载。但是同样会存在一些问题。
一般能做到第二种方式的公司，网页流量自然可以想像（小公司请自动忽略）。
那么当在发布版本的时候，会存在两个类型的文件需要发布：
1） html文件，上面有资源文件的引用
2 ）资源文件

那么发布以上两个文件的顺序就成问题了。

如果先发 html文件：
那么会导致重新加载资源，但一样还是无法访问到最新的特性。（毕竟资源文件还没有真正的更新。），如是Html页面的结构有更新，但加载了旧的资源，很有可能导致页面结构的错乱。并且会缓存资源，直到资源过期，否则除非强制刷新，会一直是错误页面。（这里要注意到，由于第一次加载了旧的资源，版本号又是新的版本号，所以即使在这之后上了资源，这里依旧会读取旧的资源.)

如果先发资源文件： 
如果之前访问过页面，那就会有保存有本地缓存，那么由于访问的还是缓存文件，不会出现问题。但如果是新用户，那么就会访问到新的资源文件，很有可能导致页面错乱。而等到页面html也发布之后，页面又恢复了正常。

PS: 当然有的人可能会说，发布就那么一会的时间，有必要那么在乎这些一点点时间么？ 
如果你这么想，那么我只能说，我无话可说。

发上两种都是属于覆盖式资源发布，不管如何处理，都会存在这样的问题。那么解决方案就是第三种。非覆盖式发布。

3 第三种方式，应该是最完美的解决方案：
1 首先发资源文件，由于文件名已经不一样了，所以不会覆盖掉之前存在的资源文件，客户端依旧可以安全的访问。
2 再发客户端文件，在客户端文件一旦发布成功，那么就会立马切成新的特性，中间可以做到无缝衔接。
这就是所谓的非覆盖发布的方案。