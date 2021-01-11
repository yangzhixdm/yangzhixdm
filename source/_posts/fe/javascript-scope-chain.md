---
title: javascript中的[[scope]],scope chain,execution context
date: 2013-12-09 11:56:17
tags:
description: javascript中的[[scope]],scope chain,execution context
---
参考文章:
勇敢的心:http://blog.sina.com.cn/s/blog_912389e5010120n2.html
汤姆大叔: http://www.cnblogs.com/TomXu/archive/2012/01/18/2312463.html
汤姆大叔:http://www.cnblogs.com/TomXu/archive/2012/01/16/2309728.html
非常感谢以上博主的文章。
关于javascript的作用域的一些总结,主要参考以上文章，加上自己的整理的理解。

近日对javascript的作用域，以及它的运行过程，感觉不甚了解，尤其在使用闭包的时候，感觉都是模糊不清的。
与是在网上一探究竟，可是没想，网上的大多文章都是含糊其次，且大多雷同，经多方查找，加上自己看一些书。
最终对javascript有一丝丝了解。记录下来，以防忘记。
要彻底了解javascript的作用域就要了解一些概念。

### 运行期上下文
在javascript中，只有函数能够创建出独立的作用域，需要注意的是for循环是不能创建的，否则你的代码就可能得到意想不到的结果。
``` javascript
for (var k in {a: 1, b: 2}) {
  alert(k);
}
alert(k);
```
即使循环结束，一样可以alert出k的值.

### 变量对象以及活动对象(VO/AO)
-变量对象(缩写为VO)是一个与执行上下文相关的特殊对象，它存储着在上下文中声明的以下内容：

-变量 (var,变量声明);

-函数声明 (FunctionDeclaration,缩写为FD);
--函数的形参

只有全局上下文的变量对象允许通过VO的属性名称来间接访问(因为在全局上下文里，全局对象自身就是变量对象)，在其它上下文中是不能直接访问VO对象的，因为它只是内部机制的一个实现,通常使用AO(activation object来保存变量)。

VO保存变量对象示例：
``` javascript
var a = 10;
 functiontest(x) {
  var b = 20;
 };
 
test(30);
```
对应的变量对象是：
``` javascript
// 全局上下文的变量对象
VO(globalContext) = {
  a: 10,
  test:<reference to function>
};
// test函数上下文的变量对象
VO(test functionContext) = {
  x: 30,
  b: 20
};
```
#### 全局上下文中的变量对象(VO)
首先，我们要给全局对象一个明确的定义：

全局对象(Global object) 是在进入任何执行上下文之前就已经创建了的对象；
这个对象只存在一份，它的属性在程序中任何地方都可以访问，全局对象的生命周期终止于程序退出那一刻。

全局对象初始创建阶段将Math、String、Date、parseInt作为自身属性，等属性初始化，同样也可以有额外创建的其它对象作为属性（其可以指向到全局对象自身）。例如，在DOM中，全局对象的window属性就可以引用全局对象自身(当然，并不是所有的具体实现都是这样)：
``` javascript
global = {
  Math: <...>,
  String:<...>
  ...
  ...
  window: global//引用自身
};
```
当访问全局对象的属性时通常会忽略掉前缀，这是因为全局对象是不能通过名称直接访问的。不过我们依然可以通过全局上下文的this来访问全局对象，同样也可以递归引用自身。例如，DOM中的window。综上所述，代码可以简写为：
``` javascript
String(10); // 就是global.String(10);
 
// 带有前缀
window.a = 10; // === global.window.a = 10 ===global.a = 10;
 this.b = 20; //global.b = 20;
alert(this === window);
这里可以看到，用winodow调用其实是用的VO中的一个属性，而用this,则是用的全局变量。
因此，回到全局上下文中的变量对象——在这里，变量对象就是全局对象自己：
VO(globalContext) === global;
非常有必要要理解上述结论，基于这个原理，在全局上下文中声明的对应，我们才可以间接通过全局对象的属性来访问它（例如，事先不知道变量名称）。
var a = new String('test');
alert(a); // 直接访问，在VO(globalContext)里找到："test"
alert(window['a']); // 间接通过global访问：global === VO(globalContext): "test"
alert(a === this.a); // true
var aKey = 'a';
alert(window[aKey]); // 间接通过动态属性名称访问："test"
```
#### 函数上下文中的变量对象(AO)
在函数执行上下文中，VO是不能直接访问的，此时由活动对象(activation object,缩写为AO)扮演VO的角色。
``` javascript
VO(functionContext)=== AO;
```
活动对象是在进入函数上下文时刻被创建的，它通过函数的arguments属性初始化。arguments属性的值是Arguments对象：
``` javascript
AO = {
  arguments: <ArgO>
};
```
Arguments对象是活动对象的一个属性，它包括如下属性：

callee — 指向当前函数的引用

length — 真正传递的参数个数

properties-indexes (字符串类型的整数) 属性的值就是函数的参数值(按参数列表从左到右排列)。 properties-indexes内部元素的个数等于arguments.length. properties-indexes 的值和实际传递进来的参数之间是共享的。

例如：
``` javascript
function foo(x, y, z) {
  // 声明的函数参数数量arguments (x, y, z)
  alert(foo.length); // 3
  // 真正传进来的参数个数(only x, y)
 alert(arguments.length); // 2
  // 参数的callee是函数自身
  alert(arguments.callee=== foo); // true
  // 参数共享
  alert(x === arguments[0]); // true
  alert(x); //10
  arguments[0] =20;
  alert(x); //20
  x = 30;
 alert(arguments[0]); // 30
// 不过，没有传进来的参数z，和参数的第3个索引值是不共享的
  z = 40;
 alert(arguments[2]); // undefined
   arguments[2] = 50;
   alert(z); // 40
}
foo(10, 20);
```
这个例子的代码，在当前版本的Google Chrome浏览器里有一个bug  — 即使没有传递参数z，z和arguments[2]仍然是共享的。

### 作用域链
从上面可以知道,每个上下文拥有自己的变量对象：对于全局上下文，它是全局对象自身；对于函数，它是活动对象。
``` javascript
var x = 10;
function foo() {
var y = 20;
  function bar(){
alert(x +y);
}
return bar;
}
foo()(); // 30
```
作用域链正是内部上下文所有变量对象（包括父变量对象）的列表。此链用来变量查询。即在上面的例子中，“bar”上下文的作用域链包括AO(bar)、AO(foo)和VO(global)。这里的AO(foo)和VO(global)都是来自父变量对象.

作用域链与一个执行上下文相关，变量对象的链用于在标识符解析中变量查找。

函数上下文的作用域链在函数调用时创建的，包含活动对象和这个函数内部的[[scope]]属性。

在上下文中示意如下：
``` javascript
activeExecutionContext= {
  VO: {...}, //or AO  上下文初始化的VO/AO
  this: thisValue,
  Scope: [ // Scope chain,作用域链
  // 所有变量对象的列表
  // for identifiers lookup
  ]
};
```
其scope定义如下：
``` javascript
Scope= AO + [[Scope]]
```
这种联合和标识符解析过程，我们将在下面讨论，这与函数的生命周期相关。

对于Scope = AO + [[Scope]]，在函数被调用的时候，上下文会创建一个Scope China，也就是Scope属性,然后初始化为函数的内部属性，也就是函数内部属性[[Scope]],再将进入上下文时创建的VO/AO，压入Scopechina的最前端。

#### 函数创建
函数的[[scope]]属性是所有父变量对象的层级链，处于当前函数上下文之上，在函数创建时(函数生命周期分为函数创建和函数调用阶段)存于其中。函数能访问更高一层上下文的变量对象,这种机制是通过函数内部的[[scope]]属性来实现的。

注意重要的一点－－[[scope]]在函数创建时被存储－－静态（不变的），永远永远，直至函数销毁。即：函数可以永不调用，但[[scope]]属性已经写入，并存储在函数对象中。由于是静态存储，再配合上内部函数的[[scope]]属性是所有父变量的层级链，就导致了闭包的存在。如下：
``` javascript
var x = 10;
function foo() {
  alert(x);
}
 
(function () {
  var x = 20;
  foo(); // 10,but not 20,这里会访问foo中的[[scope]]的VO中的x
})();
```

这个例子也清晰的表明，一个函数（这个例子中为从函数“foo”返回的匿名函数）的[[scope]]持续存在，即使是在函数创建的作用域已经完成之后。

另外一个需要考虑的是－－与作用域链对比，[[scope]]是函数的一个属性而不是上下文。考虑到上面的例子，函数“foo”的[[scope]]如下：


foo.[[Scope]] = [ globalContext.VO // === Global ];
举例来说，我们用通常的ECMAScript 数组展现作用域和[[scope]]。

#### 函数激活
正如在定义中说到的，进入上下文创建AO/VO之后，上下文的Scope属性（变量查找的一个作用域链）作如下定义：
``` javascript
Scope= AO|VO + [[Scope]]
```
上面代码的意思是：活动对象是作用域数组的第一个对象，即添加到作用域的前端。
``` javascript
Scope= [AO].concat([[Scope]]);
```
这个特点对于标示符解析的处理来说很重要。

标示符解析是一个处理过程，用来确定一个变量（或函数声明）属于哪个变量对象。

这个算法的返回值中，我们总有一个引用类型，它的base组件是相应的变量对象（或若未找到则为null）,属性名组件是向上查找的标示符的名称。

标识符解析过程包含与变量名对应属性的查找，即作用域中变量对象的连续查找，从最深的上下文开始，绕过作用域链直到最上层。

这样一来，在向上查找中，一个上下文中的局部变量较之于父作用域的变量拥有较高的优先级。万一两个变量有相同的名称但来自不同的作用域，那么第一个被发现的是在最深作用域中。

我们用一个稍微复杂的例子描述上面讲到的这些。
``` javascript
var x = 10;
functionfoo() {
var y = 20;
function bar() {
var z = 30;
alert(x +  y +z);
  }
  bar();
}
foo(); //60
```

对此，我们有如下的变量/活动对象，函数的的[[scope]]属性以及上下文的作用域链：

全局上下文的变量对象是：
``` javascript
globalContext.VO=== Global = {
  x: 10
  foo: <reference to function>
};
```

在“foo”创建时，“foo”的[[scope]]属性是：
``` javascript
foo.[[Scope]]= [
  globalContext.VO
];
```
在“foo”激活时（进入上下文），“foo”上下文的活动对象是：
``` javascript
fooContext.AO= {
  y: 20,
  bar: <reference to function>
};
```
“foo”上下文的作用域链为：
``` javascript
fooContext.Scope= fooContext.AO + foo.[[Scope]] // i.e.:
 
fooContext.Scope= [
  fooContext.AO,
  globalContext.VO
];
```
内部函数“bar”创建时，其[[scope]]为：
``` javascript
bar.[[Scope]]= [
  fooContext.AO,
  globalContext.VO
];
```
在“bar”激活时，“bar”上下文的活动对象为：
``` javascript
barContext.AO= {
  z: 30
};
```
“bar”上下文的作用域链为：
``` javascript
barContext.Scope= barContext.AO + bar.[[Scope]] // i.e.:
 
barContext.Scope= [
  barContext.AO,
  fooContext.AO,
  globalContext.VO
];
```
对“x”、“y”、“z”的标识符解析如下：
``` javascript
-"x"
--barContext.AO // not found
--fooContext.AO // not found
--globalContext.VO // found - 10
 
-"y"
--barContext.AO // not found
--fooContext.AO // found - 20
 
-"z"
--barContext.AO // found – 30
```
### 闭包

创建闭包的常见方式是在一个函数内部创建另一个函数，以create ()函数为例：
``` javascript
function create(){
var x = 0
return function(){
       alert(++x);
};
}
var c = create();
c();     //alert 1not 0
c();     //alert 2not 0
c();     //alert 3not 0
```
上面代码中的内部函数（一个匿名函数），访问了外部函数中的变量x。即使这个内部函数被返回了，且在其他地方被调用了，但它仍可访问变量x.之所以还能够访问这个变量，是因为内部函数的作用域链中包含create ()的作用域。

当某个函数第一次被调用时，会创建一个执行环境及相应的作用域链，并把一个特殊的内部属性（即[[Scope]] ) 赋值给作用域链。然后使用this、arguments和其他命名参数的值来初始化函数的活动对象。然后把该活动对象压入作用域链的最前端。所以在作用域链中，父级活动对象始终处于第二位，直到作用域链终点的全局执行环境。

在函数执行过程中，为读取和写入变量的值，需要在作用域中查找变量：
``` javascript
function compare(value1,value2){
    if(value2<value2){
        return -1;
  }
    else if(value1>value2){
        return1;
  }
    else {
      return0;
  }
}
var result =compare(5,10);
```

上面的代码先定义了compare()函数，又在全局作用域调用了它。第一次调用compare()时，会创建一个包含this、arguments、value1、value2的活动对象。全局执行环境的变量对象(包含this,result,compare)在compare()执行环境的作用域中处于第二位，如图：

后台的每个执行环境都有一个表示变量的对象－－变量对象。全局环境的变量对象始终存在，而像compare()函数这样的局部环境的变量对象则只在函数执行过程中存在。在创建compare()函数（注意，这里是创建）时，会创建一个预先包含全局变量对象的作用域链，这个作用域链被保存在内部的[[Scope]]属性中。当调用compare函数时，会为函数创建一个执行环境，然后通过复制函数的[[Scope]]属性中的对象构建起执行环境的作用域链。

此后又有一个活动对象（在此作为变量对象使用）被创建并被推入执行环境作用域链的前端。对于这个例子中compare()函数的执行环境而言，其作用域链中包含两个变量对象：本地活动对象和全局变量对象。

作用域链本质上是一个指向变量对象的指针列表，它只引用但不实际包含变量对象

一般来讲在当函数执行完毕后，局部活动对象就会被销毁，内在中仅保存全局作用域，但闭包情况有所不同。另一个函数内部定义的函数会将包含函数的活动对象添加到它的作用域链中。因此，createComparisonFunction()函数内定义的匿名函数的作用域链中，实际上将会包含外部函数createComparisonFunction()的活动对象，
``` javascript
function createComparisonFunction(propertyName){
   return function(object1,object2){
        varvalue1 = object1[propertyName];
        varvalue1 = object2[propertyName];
        if(value1 < value2){return -1;}
        elseif (value1>value2){return 1;}
        else{return 0;}
   };
}

var compare =createComparisonFunction("name");
var result =compare({name:'Nicholas"},{name:"Greg"});
```

在匿名函数从createComparisonFunction()中被返回后，它的作用域链被初始化为包含createComparisonFunction()函数的活动对象和全局变量对象。这样createComparisonFunction()函数执行完毕后其活动对象也不会被销毁，因为匿名函数的作用域链仍然在引用这个活动对象。换句话说，createComparisonFunction()函数返回后，其执行环境的作用域链会被销毁，但它的活动对象仍会留在内存中；直到匿名函数被销毁后，createComparisonFunction()的活动对象才会被销毁。

### 闭包和变量
看如下代码:
``` javascript
function fn(){
    var result =new Array();
    for (vari=0;i<10;i++){
      result[i]= function(){return i;}
   }
    returnresult;
}
var funcs = fn();
for (var i=0;i<funcs.length;i++){
   document.write(funcs[i]()+"<br/>");
}
```
这段代码代码可能说是堪称经典。你也许知道结果会输出10个10。

这里的原因还是因为作用域的问题。

在fn 被调用结束后，那么在fn的作用链的AO中保存i的值为10。

主要的问题就是result[i]的方法的作用链中保存了父级活动对象。那么当激活result方法时，会先去自已的活动对象中搜索，当没有发现表示符时，则会去父级活动对象中搜索，当搜索到i的时候，这个i已经是10了。这就是为什么结果都为10。当然需要如果需要得到你想要的结果，那么可以采用如下的方式：
``` javascript
result[i] = function(num){
  returnfunction(){
    returnnum;
  }
}(i);
```
这里为什么能得出正确的结果呢？其实这里是将修改了result的活动对象。

第一个result的函数的活动对象中多了一个 num属性，而这个属性是用一个自启动函数传入的。每次激活函数的时候直接在自已的活动对象中去搜索到num就返回了，不再去父级变量中搜索了。这样就能结果问题了。

### javascript 代码执行过程
javascript中的function的一些东西这里就不说了。

关于function对象的[[scope]]是一个内部属性。

Functiion对象在创建的时候会自动创建一个[[scope]]内部属性，且只有js引擎才能够去访问。同时也会创建一个scope chian的作用域链。且[[scope]]链接到scope china中。
``` javascript
function add(){
  Var name = “hello”;
}
```
那么如上：

Function add 在创建的时候就会创建一个[[scope]]的属性，且指向scope chian.由于是在全局的环境中，那么scope chian刚指向window action object.(这里不是很清楚).

这么以上的情况都是要定义的时候完成的，也可以说是在js解释的时候完成的。

执行此函数时会创建一个称为“运行期上下文(execution context)”的内部对象，运行期上下文定义了函数执行时的环境。每个运行期上下文都有自己的作用域链，用于标识符解析，当运行期上下文被创建时，而它的作用域链初始化为当前运行函数的[[Scope]]所包含的对象。

运行期上下文的代码被分成两个基本的阶段来处理：

-进入执行上下文
-执行代码

变量对象的修改变化与这两个阶段紧密相关。

注：这2个阶段的处理是一般行为，和上下文的类型无关（也就是说，在全局上下文和函数上下文中的表现是一样的）。

#### 进入执行上下文
当进入执行上下文(代码执行之前)时，VO/AO里已经包含了下列属性(前面已经说了)：

    函数的所有形参(如果我们是在函数执行上下文中)

         — 由名称和对应值组成的一个变量对象的属性被创建；没有传递对应参数的话，那么由名称和undefined值组成的一种变量对象的属性也将被创建。

    所有函数声明(FunctionDeclaration,FD)

         —由名称和对应值（函数对象(function-object)）组成一个变量对象的属性被创建；如果变量对象已经存在相同名称的属性，则完全替换这个属性。

    所有变量声明(var,VariableDeclaration)

         — 由名称和对应值（undefined）组成一个变量对象的属性被创建；如果变量名称跟已经声明的形式参数或函数相同，则变量声明不会干扰已经存在的这类属性。

让我们看一个例子：
``` javascript
function test(a, b) {
  var c = 10;
  function d(){}
  var e =function _e() {};
  (function x(){});
}
test(10); // call
```

当进入带有参数10的test函数上下文时，AO表现为如下：

AO(test) = {
  a: 10,
  b: undefined,
  c: undefined,
  d:<reference to FunctionDeclaration "d">
  e: undefined
};


注意，AO里并不包含函数“x”。这是因为“x”是一个函数表达式(FunctionExpression, 缩写为 FE) 而不是函数声明，函数表达式不会影响VO。 不管怎样，函数“_e” 同样也是函数表达式，但是就像我们下面将看到的那样，因为它分配给了变量“e”，所以它可以通过名称“e”来访问。

这之后，将进入处理上下文代码的第二个阶段— 执行代码。

#### 代码执行
这个周期内，AO/VO已经拥有了属性(不过，并不是所有的属性都有值，大部分属性的值还是系统默认的初始值undefined )。

还是前面那个例子, AO/VO在代码解释期间被修改如下：

AO['c'] = 10;
AO['e'] = <reference to FunctionExpression "_e">;

再次注意，因为FunctionExpression“_e”保存到了已声明的变量“e”上，所以它仍然存在于内存中。而FunctionExpression “x”却不存在于AO/VO中，也就是说如果我们想尝试调用“x”函数，不管在函数定义之前还是之后，都会出现一个错误“x is not defined”，未保存的函数表达式只有在它自己的定义或递归中才能被调用。

另一个经典例子：
``` javascript
alert(x); // function
var x = 10;
alert(x); // 10
x = 20;
function x() {};
alert(x); // 20
```
为什么第一个alert “x” 的返回值是function，而且它还是在“x” 声明之前访问的“x” 的？为什么不是10或20呢？因为，根据规范函数声明是在当进入上下文时填入的； 同意周期，在进入上下文的时候还有一个变量声明“x”，那么正如我们在上一个阶段所说，变量声明在顺序上跟在函数声明和形式参数声明之后，而且在这个进入上下文阶段，变量声明不会干扰VO中已经存在的同名函数声明或形式参数声明，因此，在进入上下文时，VO的结构如下：

VO = {};
VO['x'] = <reference to FunctionDeclaration"x">
// 找到var x = 10;
// 如果function"x"没有已经声明的话
// 这时候"x"的值应该是undefined
// 但是这个case里变量声明没有影响同名的function的值
VO['x'] = <the value is not disturbed, stillfunction>
紧接着，在执行代码阶段，VO做如下修改：
VO['x'] = 10;
 VO['x'] = 20;
我们可以在第二、三个alert看到这个效果。

在下面的例子里我们可以再次看到，变量是在进入上下文阶段放入VO中的。(因为，虽然else部分代码永远不会执行，但是不管怎样，变量“b”仍然存在于VO中。)
``` javascript
if (true) {
  var a = 1;
} else {
  var b = 2;
}
alert(a); //
alert(b); // undefined,不是b没有声明，而是b的值是undefined
```
### 关于变量
通常，各类文章和JavaScript相关的书籍都声称：“不管是使用var关键字(在全局上下文)还是不使用var关键字(在任何地方)，都可以声明一个变量”。请记住，这是错误的概念：

任何时候，变量只能通过使用var关键字才能声明。

上面的赋值语句：

a= 10;

这仅仅是给全局对象创建了一个新属性(但它不是变量)。“不是变量”并不是说它不能被改变，而是指它不符合ECMAScript规范中的变量概念，所以它“不是变量”(它之所以能成为全局对象的属性，完全是因为VO(globalContext) === global，大家还记得这个吧？)。

让我们通过下面的实例看看具体的区别吧：
``` javascript
alert(a); // undefined
alert(b); // "b" 没有声明
b = 10;
var a = 20;
```

所有根源仍然是VO和进入上下文阶段和代码执行阶段：

进入上下文阶段：

VO = {
a: undefined
};

我们可以看到，因为“b”不是一个变量，所以在这个阶段根本就没有“b”，“b”将只在代码执行阶段才会出现(但是在我们这个例子里，还没有到那就已经出错了)。

让我们改变一下例子代码：
``` javascript
alert(a); // undefined, 这个大家都知道，
b = 10;
alert(b); // 10, 代码执行阶段创建
var a = 20;
```

关于变量，还有一个重要的知识点。变量相对于简单属性来说，变量有一个特性(attribute)：{DontDelete},这个特性的含义就是不能用delete操作符直接删除变量属性。
``` javascript
a = 10;
alert(window.a); // 10
alert(delete a); // true
alert(window.a); // undefined
var b = 20;
 alert(window.b); // 20
alert(delete b); // false
alert(window.b); // still 20
但是这个规则在有个上下文里不起走样，那就是eval上下文，变量没有{DontDelete}特性。
eval('var a = 10;');
alert(window.a); // 10
alert(delete a); // true
alert(window.a); // undefined
```

使用一些调试工具(例如：Firebug)的控制台测试该实例时，请注意，Firebug同样是使用eval来执行控制台里你的代码。因此，变量属性同样没有{DontDelete}特性，可以被删除。

如果能认真看完博客，相信你一定对javascript很感兴趣。

文档如果对你有一丝丝的帮助，那么恭喜。