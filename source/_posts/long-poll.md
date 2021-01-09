---
title: 长轮询(LongPoll)
date: 2017-06-09 15:30:27
tags:
---

之前去某公司面试，需要解决一个在线聊天室的消息实时性问题，且需要兼容低版本浏览器。 
高版本浏览器自然不成问题，那么低版本的浏览器如何处理。 
最先想到的方案，自然是轮询发ajax请求。在一定的程度上，这个方案可以解决实时性的问题。 
但是随之带来的问题是，不停的向服务器发请求带来的服务器的压力，且多数时间都是无用请求。 
后实在没有方案，向面试官请教之，得出一个方案： 长轮询。 
服务器与客服端需保持一条长时间的请求，将请求Hold住，只在有数据或者超时情况下返回消息给客户端。

javascript代码:
``` javascript
$(function(){   
  $("#btn").on("click",,function(event){      
        $.ajax({   
          type:"POST",      
          dataType:"json",      
          url:"url", //请求url
          success:function(data,textStatus){
              //得到数据，显示数据并继续查询      
              if(data.success=="1"){
                  console.log('get message');
                  $("#btn").trigger('click'）;   
              }      
              //未得到数据，继续查询
              if(data.success=="0"){
                  console.log('no message');
                  $("#btn").trigger('click'）;      
              }
          }
      });      
  });
});
```

php代码:
``` php
set_time_limit(0);//无限请求超时时间
$maxInvalidCount = 30;//最大尝试次数
$i=0;
while (true){      
  $result = mysqli_query($link, $sql); 
  //获取到数据   
  if ($result) { 
      $arr = array('success'=>"1",'result'=>[]);      
      echo json_encode($arr);      
      exit();      
  }

  //尝试获取数据次数，超过最大次数之后，给客户端一个响应，如果客户端没有响应，则直接关闭
  if ($i == $maxInvalidCount) {
      $arr = array('success'=>"0",'result'=>[]);      
      echo json_encode($arr);      
      exit();      
  }
  //休眠一秒
  sleep(1);
}
```