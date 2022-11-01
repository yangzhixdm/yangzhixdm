---
title: 一些知识点记录
date: 2020-10-20
tags:
---

### 关于跨域
> 跨域是浏览器的安全策略所衍生的问题，主要表现为浏览器会对服务端返回的数据进行拦截。
需要注意是即使是在跨域的情况下，请求一样可以发送出去，只是服务端返回的数据被浏览器拦截掉了。
所有即使是请求失败了，但也有可能已经对服务器进行了操作。

> http请求分为以下两种请求
#### 简单请求
>请求方法是以下三种方法之一: HEAD, GET, POST
HTTP的头信息不超出以下几种字段:
```
Accept, 
Accept-Language, 
Content-Language, 
Last-Event-ID, 
Content-Type：只限于三个值 application/x-www-form-urlencoded、multipart/form-data、text/plain
```
> 对于简单请求，浏览器直接发出CORS请求。具体来说，就是在头信息之中，增加一个Origin字段

#### 复杂请求
> 复杂请求是那种对服务器有特殊要求的请求，比如请求方法是PUT或DELETE，或者Content-Type字段的类型是application/json
##### 预检请求
> 复杂请求的CORS请求，会在正式通信之前，增加一次HTTP查询请求，称为"预检"请求（preflight）。
浏览器发现，这是一个非简单请求，就自动发出一个"预检"请求，要求服务器确认可以这样请求,
"预检"请求用的请求方法是OPTIONS，表示这个请求是用来询问的。头信息里面，关键字段是Origin，表示请求来自哪个源。
除了Origin字段，"预检"请求的头信息包括两个特殊字段。
Access-Control-Request-Method
Access-Control-Request-Headers

##### 预检请求的回应
> 服务器收到"预检"请求以后，检查了Origin、Access-Control-Request-Method和Access-Control-Request-Headers字段以后，确认允许跨源请求，就可以做出回应

##### 浏览器的正常请求和回应
> 一旦服务器通过了"预检"请求，以后每次浏览器正常的CORS请求，就都跟简单请求一样，会有一个Origin头信息字段。服务器的回应，也都会有一个Access-Control-Allow-Origin头信息字段。



### ES6
#### 对象的setter和getter

``` javascript
module.exports = {

    /**
    * Return request header.
    *
    * @return {Object}
    * @api public
    */

    get header() {
        return this.req.headers;
    },

    /**
    * Set request header.
    *
    * @api public
    */

    set header(val) {
        this.req.headers = val;
    }
}

```
#### class

> 1 如果继承extends，必须在子类construstor中调用super方法
2 class中的方法，没有this指针
3 class语法中是语法糖而已，实际还是使用的function进行创建对象

> 增强的对象字面量
> 可以在对象字面量里面定义原型
定义方法可以不用function关键字

``` javascript
//通过对象字面量创建对象
var human = {
    breathe() {
        console.log('breathing...');
    }
};
var worker = {
    __proto__: human, //设置此对象的原型为human,相当于继承human
    company: 'freelancer',
    work() {
        console.log('working...');
    }
};
human.breathe();//输出 ‘breathing...’
//调用继承来的breathe方法
worker.breathe();//输出 ‘breathing...’
```

#### 字符串模板
> ES6中可以使用，反引号 ` 用来创建字符串，此种方式创建的字符串中可以包含美元符加花括号包裹的变量${varibel},如:

``` javascript
return `${this.protocol}://${this.host}`;
```


### Spread (展开运算符) ... 
> https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Spread_syntax

> Spread syntax allows an iterable such as an array expression or string to be expanded in places where zero or more arguments (for function calls) or elements (for array literals) are expected, or an object expression to be expanded in places where zero or more key-value pairs (for object literals) are expected

> 展开运算符允许将一个`可迭代的变量`如数组表达式或者字符串展开到需要0个或多个参数（函数调用）或元素（数组迭代）的位置，或者将一个对象表达式展开到一个需要0个或者多个键值对（对象迭代）的位置。

``` javascript
function sum(x, y, z) {
  return x + y + z;
}

const numbers = [1, 2, 3];

console.log(sum(...numbers));//for function call
// expected output: 6

console.log(sum.apply(null, numbers));
// expected output: 6
```

> for function  call

``` javascript
myFunction(...iterableObj);
```

> for array literals or strings

``` javascript
[...iterableObj, '4', 'five', 6];
```

> For object literals (new in ECMAScript 2018):

``` javascript
let objClone = { ...obj };
```

### Destructuring (解构)
> ES6 新的赋值方案，用来代理原来的手动赋值。包括数组解构(array destructuring)与对象解构(object destructuring)如下：

``` javascript
function foo() {
    return [1,2,3];
}

function bar() {
    return {
        x: 4,
        y: 5,
        z: 6
    };
}

var [ a, b, c ] = foo();
var { x: x, y: y, z: z } = bar();

console.log( a, b, c );             // 1 2 3
console.log( x, y, z );             // 4 5 6
```
> Destructuring symmetrically flips that pattern, so that [a,b,c] on the lefthand side of the = assignment is treated as a kind of "pattern" for decomposing the righthand side array value into separate variable assignments.

> 一般情况下，我们习惯 [a,b,c] 在 = (赋值号) 右边的这种语法，Destructuring（解构）语法翻转了这种模式，使[a,b,c] 放在了 = 的左边

> 解构赋值定义了一种特殊的赋值pattern模式，用来分解=(赋值号)右边的数组，并把值赋值给独立的变量。

#### Object Property Assignment Pattern (对象属性赋值模式)

> 如果对象属性名和你声明的变量名一样，可以简写如下：

``` javascript
var { x, y, z } = bar();

console.log( x, y, z );             // 4 5 6
```

> 但是 { x, ... } 的这种语法是省略了 `x:`还是 `:x` 呢？ 我们可以确认是省略了 `x:`当使用简写方式。 

> 既然可以使用缩写方式，那为什么要使用完整的方式呢？ 由于完整的方式可以将属性赋值给一个不同的变量名，如：

``` javascript
var { x: bam, y: baz, z: bap } = bar();

console.log( bam, baz, bap );       // 4 5 6
console.log( x, y, z );             // ReferenceError
```


> 这里有一个问题需要注意，虽然很微妙，但是在某些情况下非常重要，对象解构 { x : bam, y: baz } 与 字面量对象 { a: b } 是有区别的。
> 需要注意的是，这里的 {x: bam, y: baz, z: bap} 的模式是 { source : target }的模式。 所有这里直接获取 x,y,z 会报引用错误。
也就是说，这里的x 是对象解构出来的值，而 bam才是用户声明的引用。
> 而字面量对象采用的模式是 { target: source } 模式，如{ a: b }，那么 a是引用，b 是值。

> 从上也就可以解释为什么使用简写方案时，省略的是`x:`,因为 解构模式中右边的值才是表示标识符。

#### 不只是声明
> 解构destructuring 是一个通用表达式，不只是用来声明变量。

``` javascript
var a, b, c, x, y, z;

[a,b,c] = foo();
( { x, y, z } = bar() );

console.log( a, b, c );             // 1 2 3
console.log( x, y, z );             // 4 5 6
````

> 对象解构需要如果前面没有var/let/const 声明符的话，需要加上( )，否则的话会被当成 { } 块级作用域符。

> In fact, the assignment expressions (a, y, etc.) don't actually need to be `just` variable identifiers. Anything that's a valid assignment expression is allowed.

>实际上，赋值表达式 (a、 y 等 ) 并不必须是变量标识符。任何合法的赋值表达式都可以

``` javascript
var o = {};

[o.a, o.b, o.c] = foo();
( { x: o.x, y: o.y, z: o.z } = bar() );

console.log( o.a, o.b, o.c );       // 1 2 3
console.log( o.x, o.y, o.z );       // 4 5 6
```

> 甚至可以在解构中使用计算出的属性表达式

``` javascript
var which = "x",
    o = {};

( { [which]: o[which] } = bar() );

console.log( o.x ); 
```
> [which]: 这一部分是计算出的属性，结果是 x——要从涉及的对象解构出来作为赋值源的属性。 o[which] 部分就是一个普通的对象键值引用，等价于 o.x 作为赋值的目标。

#### 几种解构应用
> 创建对象映射 / 变换

``` javascript
var o1 = { a: 1, b: 2, c: 3 },
    o2 = {};

( { a: o2.x, b: o2.y, c: o2.z } = o1 );

console.log( o2.x, o2.y, o2.z );    // 1 2 3
```

> 映射对象为数组

``` javascript
var o1 = { a: 1, b: 2, c: 3 },
    a2 = [];

( { a: a2[0], b: a2[1], c: a2[2] } = o1 );

console.log( a2 );                  // [1,2,3]
```

> 映射数组为对象

``` javascript
var a1 = [ 1, 2, 3 ],
    o2 = {};

[ o2.a, o2.b, o2.c ] = a1;

console.log( o2.a, o2.b, o2.c );    // 1 2 3
```

> 重排数组顺序为一个新数组

``` javascript
var a1 = [ 1, 2, 3 ],
    a2 = [];

[ a2[2], a2[0], a2[1] ] = a1;

console.log( a2 );                  // [2,3,1]
```

> 甚至可以解决经典的两个数交换问题

``` javascript
var x = 10, y = 20;

[ y, x ] = [ x, y ];

console.log( x, y );                // 20 10
```

> Warning: Be careful: you shouldn't mix in declaration with assignment unless you want all of the assignment expressions also to be treated as declarations. Otherwise, you'll get syntax errors. That's why in the earlier example I had to do var a2 = [] separately from the [ a2[0], .. ] = .. destructuring assignment. It wouldn't make any sense to try var [ a2[0], .. ] = .., because a2[0] isn't a valid declaration identifier; it also obviously couldn't implicitly create a var a2 = [] declaration to use.

> 注意:你不应该混入声明与赋值，除非你希望所有赋值表达式也被当作是声明。否则你会触发语法错误。这就是为什么我必须在 [ a2[0], ...] 之前定义 var a2 = []. 语句 var [ a2[0], .. ] = .. 是不合法的，因为 a2[0] 不是有效的声明标识符；显然它也不会隐式地创建一个var a2 = [] 声明.
