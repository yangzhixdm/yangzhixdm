---
title: 兼容IE 6/7 inline-block
date: 2014-07-31 23:10:22
tags:
---
``` html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
  <meta http-equiv="Content-Type" content="text/html;charset=UTF-8" />
  <title></title>
  <style type="text/css">
      .html,body,ul{
          margin:0px;
          padding:0px;
      }
      .inline-demo{
         width: 100px;
          height:30px;
          display:inline-block;/*for firefox and chrome and ie8+ */
          *display:inline;/*for ie6 and ie 7*/
          zoom:1;/*for trigger the ie hasLayout method*/
          height:30px;
          background:red;
          margin:10px;
          padding:10px;
          font-size:14px;
      }
  </style>
</head>
<body>
    <div>
      <!-- font-size 解决ie 67 display:inline 引起的留白bug-->
      <!--这边可能还有一个bug, 在ie 9 10 下, li 的宽度似乎比chrome的宽度要小2个像素-->
        <ul style="background:green;font-size:0px;">
            <li class="inline-demo">ie6-7-inline-block</li>
            <li class="inline-demo">ie6-7-inline-block</li>
            <li class="inline-demo">ie6-7-inline-block</li>
            <li class="inline-demo">ie6-7-inline-block</li>
            <li class="inline-demo">ie6-7-inline-block</li>
        </ul>
    </div>
</body>
</html>
```