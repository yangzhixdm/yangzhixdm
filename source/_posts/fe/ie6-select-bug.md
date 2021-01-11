---
title: IE6 select穿透问题(div 定位无法遮盖select)!
date: 2014-11-17 12:30:01
tags:
description: 有时候运气是很重要的。
---
此问题，早有耳闻，但是一直都没有亲身碰到过，也就没有过多的去在意这些问题。

这一回总算是见识到了ie的厉害，虽是在ie选项中大大的写着ie版本是ie11 ，但是有些部分确实走的ie6的内核，怪不得微软天天在各种window update.

解决此问题的方法很简单，给你需要显示的内容的后面加一个iframe，让他去挡住select不要穿透过来，这样世界就算是清净了。

如下html:
``` html
<body>
  <div>
    <select>
      <option value="">select穿透问题</option>
      <option value="">select穿透问题</option>
    </select>
  </div>
  <div class="wrap" style="position:relative;width:200px;height:200px;">
    <div class="content">需要显示的内容</div>
    <iframe class="ie-select-iframe" style="position:absolute;top:0;left:0;z-index:-1;width:200px;" scrolling="no" frameborder="0"></iframe>
  </div>
</body>
```

这里需要注意的是:

1 wrap只是一个容器，所以border之类的样式，最好不要放在这里，因为这个div是会被select穿透的。

2 iframe的宽度最好给一个值，在ie6下，要使用100%，似乎还要做一些别的设置，需要给,html,body都给height:100%,才能实现，否则在ie6下无效。