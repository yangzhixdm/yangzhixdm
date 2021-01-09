---
title: jQuery操作select、checkbox、radio!
date: 2014-01-09 11:24:14
tags:
---
### Select
jQuery获取Select选择的Text和Value: 
``` javascript
1. $("#select_id").change(function(){//code...}); //为Select添加事件，当选择其中一项时触发 
2. var checkText=$("#select_id").find("option:selected").text(); //获取Select选择的Text 
3. var checkValue=$("#select_id").val(); //获取Select选择的Value 
4. var checkIndex=$("#select_id ").get(0).selectedIndex; //获取Select选择的索引值 
5. var maxIndex=$("#select_id option:last").attr("index"); //获取Select最大的索引值
```
jQuery设置Select选择的Text和Value:
``` javascript
1. $("#select_id ").get(0).selectedIndex=1; //设置Select索引值为1的项选中 
2. $("#select_id ").val(4); //设置Select的Value值为4的项选中 
3. $("#select_id option[text='jQuery']").attr("selected", true); //设置Select的Text值为jQuery的项选中
```
jQuery添加/删除Select的Option项： 
``` javascript
1. $("#select_id").append("<option value='Value'>Text</option>"); //为Select追加一个Option(下拉项) 
2. $("#select_id").prepend("<option value='0'>请选择</option>"); //为Select插入一个Option(第一个位置) 
3. $("#select_id option:last").remove(); //删除Select中索引值最大Option(最后一个) 
4. $("#select_id option[index='0']").remove(); //删除Select中索引值为0的Option(第一个) 
5. $("#select_id option[value='3']").remove(); //删除Select中Value='3'的Option 
6. $("#select_id option[text='4']").remove(); //删除Select中Text='4'的Option 
7. $("#SelectID").remove(); //删除所有项
```
### Checkbox
#### 全选/取消
jQuery.attr 获取/设置对象的属性值,如：
``` javascript
$("input[name='chk_list']").attr("checked"); //读取所有name为'chk_list'对象的状态（是否选中） 
$("input[name='chk_list']").attr("checked",true); //设置所有name为'chk_list'对象的checked为true 
$("#img_1").attr("src","test.jpg"); //设置ID为img_1的<img>src的值为'test.jpg' 
$("#img_1").attr("src"); //读取ID为img_1的<img>src值
```
下面的代码是获取上面实例中选中的checkbox的value值:
``` javascript    
$(document).ready(function() {    
    var arrChk=$("input[name='chk_list'][checked]");    
    $(arrChk).each(function(){    
        window.alert(this.value);    
    });    
});
```
#### 获取checkbox的value 
``` javascript
$("#checkboxID").value或$("input[type='checkbox']").eq(n).attr("checked").value
```
#### 设置选中项 
``` javascript
$("input[type='checkbox']").eq(1).attr("checked")//设置第一个checkbox为选中的项
```
#### 删除所有checkbox
``` javascript
$("input[type='checkbox']").remove()
```
#### checkbox方法
``` javascript
$(document).ready(function() {    
    var check = $("input[type='checkbox']");    
    check.each(function(n) {    
        check.eq(n).bind("click", function() {    
            if (check.eq(n).attr("checked") != false) {    
                var value = check.eq(n).val();    
                alert(value);    
            }    
            else {    
                alert(check.eq(n).attr("checked"));    
            }    
        })    
    });    
});
```
### radio
#### 获取选中的value值 
``` javascript
$("input[type='radio']:checked").val();
```
#### 设置指定的项为当前选中项 
``` javascript
$("input[type='radio']").eq(1).attr("checked", true);//设置第二项为选中项 
$("input[type='radio'][value='值']").attr("checked, true");
```
#### 解决多个Radio
``` javascript
$("input[type='radio'][@name='rdoTest2']").eq(0).attr("checked", true);




