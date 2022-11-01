---
title: match，test，exec，replace
date: 2018-10-21
tags:
---

#### 有言在先
JavaScript 中四种对字符串进行匹配的方式的使用以及区别。

#### test
正着RegExp中方法，用来检测字符串是否与正则匹配，返回`ture` or `false`。

#### exec
正则RegExp中的方法，只返回`第一个匹配`。
1 当匹配的正则后没有带`g`时，则返回匹配到的值。
2 当匹配的正则后带有`g`时，返回一个数组。数组第一个值为完整匹配，后面的值依次为该匹配中第一、第二、第...个分组（反向引用）

``` javascript
var regExp = /(a)(b)(c)/g;
var str = 'abcabc';

regExp.exec(str);  //["abc", "a", "b", "c", index: 0, input: "abcabc", groups: undefined]
```

#### match
字符串String中方法，返回正则匹配的值。
1 当匹配的正则后没有带`g`时，则返回匹配到的值。
2 当匹配的正则后带有`g`时，返回一个数组。数组的值依次为匹配到的值。注意与上面的分组的区别，这里为可以匹配到的多个值。
``` javascript
var regExp = /(a)(b)(c)/g;
var str = 'abcabc';

str.match(regExp); //["abc", "abc"]
```

#### replace
字符串String中方法，可以对匹配到的值进行操作。
``` javascript
var str = '123ddd';
str.replace(/(\d)/g, function(match, item) {
  return item * 2;
}
//246ddd
```

``` javascript
var str = '123ddd';
str.replace(/(\d+)(ddd)/g, function(match, item, item2) {
  return (item * 2) + ( '(' + item2 + ')' );
});
//"246(ddd)"
```

``` javascript
var str = '123ddd';
str.replace(/(\d+)(ddd)/g, '($2)[$1]'); //(ddd)[123]
```