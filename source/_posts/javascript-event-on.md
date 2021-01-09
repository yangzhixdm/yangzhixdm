---
title: 你还在使用jquery的click(),bind()方法么
date: 2013-12-28 14:51:03
tags:
---
这里只是突然发现jquery 1.7 之后的事件绑定推荐了一些变化。jquery推荐使用on方法来绑定事件。

相信以前都是用的click,bind,unbind方法吧。

如果你仔细看看jquery的源码的话，那么你会发现bind的方法也是用的on方法来实现的。

在一定程度上on方法比bind或者是click之类的方法，更高效。

下面只是一个demo，当然这里包含了jquery的插件的写法。

关于jquery的插件的写法，可以自行百度。如果你的自学能力够厉害的话，可以查看jquery的源码。
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
  <meta http-equiv="Content-Type" content="text/html;charset=UTF-8" />
   <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
  <title></title>
</head>
<body>
    <input type="button" value="test" id="test"/>
    <script type="text/javascript">
        $(function(){
            $("#test").hello({
                "size":1
            });
        });
    </script>
    <script type="text/javascript">
    ;(function($){
        $.fn.hello = function(options){
            var defaults = {"size":0},

                opts     = $.extend({},defaults,options),

                show = {
                    play:function(options){
                        var _root =  this; 
                        _root.autoPlay();
                        _root.eventClick();
                    },
                    autoPlay:function(){
                        console.log("auto");
                    },
                    eventClick:function(){
                        //$("#test").on("click",{show:"dd"},function(e){
                        //    console.log("click :" +  e.data.show);
                        //});

                        $("#test").on({
                            click:function(){
                                alert("click");
                            },
                            mouseenter:function(){
                                alert("enter");
                            },
                            mouseleave:function(){
                                alert("leave");
                            }
                        });
                    }
                };
            return this.each(function(){
                show.play(opts);
            });
        };
    })(jQuery);
    </script>
</body>
</html>
```

如里你从这里学到了什么，那么恭喜！