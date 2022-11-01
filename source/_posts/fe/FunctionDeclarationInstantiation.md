---
title: FunctionDeclarationInstantiation
date: 2021-11-26 19:48:48
tags:
description: 函数声明实例化过程。
---

### 有言在先
就是在网上看到了一个题目而已，思考了一下，竟然弄错了，于是就有了下面发生的故事。
``` javascript
var x = 1;
function f(x, y = function () { x = 3; console.log(x); }) {
  console.log(x)
  var x = 2
  y()
  console.log(x)
}
f()
console.log(x)
// //1、上面的代码输出的是什么？
// //2、如果把var x = 2注释掉，输出的又是什么？
// //3、如果把f函数第一个参数x改成xx，输出的又是什么？
// //4、如果把f函数第一个参数x设置了默认值为4，输出的又是什么？
```
题目答案其实不重要，重要的是这个过程。 

### 思考过程
像我这样，已经工作很久的人来说，一般的问题怎么可能拦得住。
亿看这题目很简单嘛：
> 1 首先全局作用域定义一个 x 变量，然后赋值给 var x = 1;
> 2 声明一个 function f， 然后传入 两个形参，x 和 y（当然这里 y 被赋值了一个默认方法）; 
> 2.1 该默认值方法里面直接进行了赋值操作，将 3 赋值给了一个 x, 而在之前从来没有定义过 x (所以这里有坑); 

> 猜想： 这里的 x 肯定就是父级作用域的x吧，哈哈哈，后面会马上执行y() 方法，那这里的x 肯定就是在 f 作用域中定义的 x

> 3 f 方法体中，直接打印 x, 

> 猜想： 这能难倒谁？ 不就是一个 `变量提升` 的问题么？ 函数体内已经定义了 x 了，那么这里 变量进行了提升，肯定就是打印 undefined 嘛，哈哈哈

> 然后使用 var 定义个 x, 并赋值为 2；
> 3.1 执行 y()
> 3.2 再次打印 x

> 猜想： 哈哈哈，上面y() 执行的时候都已经被赋值成 3 了，那么这里肯定也是 3

> 4 执行 f
> 5 再次打印 x

> 猜想： 这里肯定就是答应了 全局的 x 嘛，根据上面的想法，根本就没有修改过全局的 x , 所以还是 1嘛，哈哈哈

所以结果就是：
```
undefined
3
3
1
```

### 有点尴尬
往浏览器里面一丢，执行一下，奔溃了：
```
undefined
3
2
1
```

有点搞不懂，上面的都还好，为啥第三个 x 输出会是 2 呢？
脑子里面引发一系列血案, 凭借 小爷 这匮乏的知识能想到问题：
> 1 function y 函数体中的 x 是从哪里来的？ 他指向谁？
> 2 function f 中的 x 到底和 function y 中的 x 有没有关系？ 如果有，是什么关系？
> 3 function y 和 funciton f 的作用域 是什么关系？ 是否存在继承的关系
> 4 function f 中的形式参数 和 var 定义的变量存储是否在一起？ 如果有冲突会怎么处理？

爆炸了，脑子不够用了，不知道怎么肥事，字母全是认识，但是不知道为啥会这样。

### 寻找出路
首先 chrome 的调试工具确实是个好东西，可以在函数内部随时打断点，可以查看作用域。

{% asset_image function-declaration-scope.jpg %}

啥玩意，竟然有两个 x ? 还出来一个  block 和 local 作用域？ 
完了，又出来一堆新的东西，我又不知道了，羞愧中。。。我是废物。

接下来的问题变成了：
> 两个 x 从哪里来的，谁是谁的问题。

### 怀疑
看到这问题，我开始怀疑是 因为 `function 的形式参数 和 var 定义变量产生冲突导致的问题`(let/const 不会出现该问题，因为就不允许这么用）。
百度一堆，说什么的都有，唧唧哇哇。唉，我太难了。
最后在某知乎大佬说要解决这个问题，可以看下ECMAScript标准中的 <a href="https://262.ecma-international.org/6.0/#sec-functiondeclarationinstantiation"> FunctionDeclarationInstantiation </a>部分。

好吧，那就去呗，不就是啃英文么？ 又不是没啃过。

### 官方解释
> When an execution context is established for evaluating an ECMAScript function a new function Environment Record is created and bindings for each formal parameter are instantiated in that Environment Record. Each declaration in the function body is also instantiated. If the function’s formal parameters do not include any default value initializers then the body declarations are instantiated in the same Environment Record as the parameters. If default value parameter initializers exist, a second Environment Record is created for the body declarations. Formal parameters and functions are initialized as part of FunctionDeclarationInstantiation. All other bindings are initialized during evaluation of the function body.

> 当为了执行一个 ECMAScript function而创建一个执行上下文的时候，一个新的 函数 `Environment Record` 对象将会被创建，并且每个在 `Environment Record`中 实例化(instantiated) 的 `形式参数` 将会被绑定。任何在函数体内的声明也将会被实例化。如果这个 function 的形式参数没有被赋予任何的默认值在初始化的时候，那么后续 function body中的声明都会被实例化在上面的 `Environment Record` 中。如果默认值存在，那么第二个 `Environment Record` 会被创建用于 function body中的声明。 形参和 function 会被作为 FunctionDeclarationInstantiation 过程的部分。任何其他绑定会被在初始化当 funciton body开始执行的时候。

在看一下 FunctionDeclarationInstantiation 的执行过程：
大致过程有 37 个步骤，这里直接看第 27/28 个步骤的操作，上面主要包含一些 参数的初始化什么的。

- 1\. Let calleeContext be the running execution context.
- 2\. Let env be the LexicalEnvironment of calleeContext. `看吧，LexicalEnvironment 词法环境`
- 3\. Let envRec be env’s EnvironmentRecord.
- ....
- 27\. If hasParameterExpressions is false, then
  - a. NOTE Only a single lexical environment is needed for the parameters and top-level vars.
  - b. Let instantiatedVarNames be a copy of the List parameterNames.
  - c. For each n in varNames, do
    - i.If n is not an element of instantiatedVarNames, then
      - 1\. Append n to instantiatedVarNames.
      - 2\. Let status be envRec.CreateMutableBinding(n).
      - 3\. Assert: status is never an abrupt completion.
      - 4\. Call envRec.InitializeBinding(n, undefined).
  - d. Let varEnv be env.
  - e. Let varEnvRec be envRec.
- 28\. Else,
  - a. NOTE A separate Environment Record is needed to ensure that closures created by expressions in the formal parameter list do not have visibility of declarations in the function body.
  - b. Let varEnv be NewDeclarativeEnvironment(env). `一般这里叫 变量环境`
  - c. Let varEnvRec be varEnv’s EnvironmentRecord.
  - d. Set the VariableEnvironment of calleeContext to varEnv.
  - e. Let instantiatedVarNames be a new empty List.
  - f. For each n in varNames, do
    - i. If n is not an element of instantiatedVarNames, then
      - 1\. Append n to instantiatedVarNames.
      - 2\. Let status be varEnvRec.CreateMutableBinding(n).
      - 3\. Assert: status is never an abrupt completion.
      - 4\. If n is not an element of parameterNames or if n is an element of functionNames, let initialValue be undefined.
      - 5\. else,
        - a. Let initialValue be envRec.GetBindingValue(n, false).
        - b. ReturnIfAbrupt(initialValue).
      - 6\. Call varEnvRec.InitializeBinding(n, initialValue).
      - 7\. NOTE vars whose names are the same as a formal parameter, initially have the same value as the corresponding initialized parameter.

主要逻辑：
1 判断参数列表是不是存在表达式，其实就是判断是不是有赋值操作，如果存在赋值操作，那么也就是 ES6 的环境了。
2 如果不存在的话那么就直接将所有的变量都直接丢到 envRec 中
3 如果存在表达式，也就是 `28` 中的操作：
  - 3.1 先创建一个新的 `EnvironmentRecord` varEnvRec ,然后绑定一下执行上下文
  - 3.2 然后在创建一个新的 `instantiatedVarNames` 变量，并至为空列表。
  - 3.3 然后遍历 varNames（就是所有var的声明列表）: for n in varNames
    - 3.3.1 如果 n 不在 instantiatedVarNames 中，则直接将变量绑定在新的环境 varEnvRec 中（上面创建的时候已经至空了，这里只是一个去重的操作而已）
    - 3.3.2 如果这个 n 并不在 `形参` 中出现 或者 没有和 functionNames 中的元素重复(声明函数可能会重复），那么就把 `initialValue` 这个变量初始化为 undefined
    - 3.3.3 否则（如果和形参或者函数名冲突），通过调用 envRec.GetBindingValue 获取在 envRec 中给该变量绑定的值，并赋值给 initialValue。然后 return 返回值。
    - 3.3.4 调用 varEnvRec.InitializeBinding 将 initialValue 的值同步到 变量环境 varEnvRec 中。
4 注意 和 形式参数相同的var变量，在初始化的时候会有和 形式参数相同的值。

> 所以，你会发现，如果存在冲突，那么该变量会在 两个环境中都存在，并且最开始的默认值（也就是形参上面给的默认值）会保存在 `词法环境`中,然后它的值被拷贝到 `变量环境`中。当我们修改的时候，会优先搜索 `变量环境` 中的值，如果查找不到再去搜索词法环境中的变量（所以如果两个环境中存在重复，那么词法环境中的值将不会被改动）。
> 如果涉及作用域的继承或者说闭包的问题，那么作用域链搜索的其实是 `词法环境`。这就是为什么上面最开始的答案是 `undefing 3 2 1`, 因为 y 方法向上搜索的作用域其实是先搜索 f 的`词法环境`，但是由于`词法环境` 在初始化之后，它的值就没有办法变更了（因为f内部的赋值操作会优先处理`变量环境`）。

``` javascript
var x = 1;
function f(x = 5, y = function () { x = 3; console.log(x); }) {
  console.log(x)
  debugger
  var x = 2 
  y() 
  console.log(x) 
}
f()
console.log(x)
```

{% asset_image function-declaration-env.jpg %}

再来看一下修改了 一下 x 的值之后的变化：

{% asset_image function-declaration-change.jpg %}

### 搜索过程
如果将 f function中的 let 定义的变量尝试传递给 y 方法会如何呢。
``` javascript
var x = 1;
function f(x = 5, y = function () { g = 3; console.log(g); }) {
  console.log(x)
  debugger
  let g = 2 
  y() 
  console.log(x) 
}
f()
console.log(x)
```
结果如下：
所以结果就是：
```
5
3
5
1
```
{% asset_image function-declaration-let.png %}

这里会发现，执行到最后 global 作用域上面，有了一个 g 的变量属性，所以看到这里相当于是在 y 方法中，直接在 global 作用域上面定义了新的变量。
所以可以看到 f 方法的变量作用域并没有被压入到 y 方法的作用域链中。

值得注意的是： 主要形式参数中出现了表达式，就会出现两个 `环境`, 然后body中的变量就会被存入到 变量环境中，而不是说需要变量和形参冲突才会存入到 变量环境（这个条件是不必要的）。

考虑如下代码：
``` javascript
var x = 1;

function f(x) {
  console.log(x)
  let g = 2 
  var y = function () { g = 3; console.log(g); }
  y() 
  console.log(x) 
}
f()
console.log(x)
```
以及如下代码：
``` javascript
var x = 1;

function f(x = 2) {
  console.log(x)
  let g = 2 
  var y = function () { g = 3; console.log(g); }
  y() 
  console.log(x) 
}
f()
console.log(x)
```
{% asset_image function-declaration-process2.png %}

我蒙圈了，哈哈哈，这里的
```
block: {
  g: 2
} 
```
这个block是从哪里来的？ 为什么 f 的变量环境会出现在这里？ g 又是从哪里来的？

在看一段代码：
``` javascript
var x = 1;

function f(x = 2) {
  console.log(x)
debugger
  let g = 2 
  var y = function () { x = 3; console.log(x); }
  y() 
  console.log(x) 
}
f()
console.log(x)
```
{% asset_image function-declaration-process3.png %}

将 y 方法中的 g 换成 x ，之后如愿的看到了，这里出现了 `Closure` 闭包，总算有那么一点正常了。
那上面的 的 block 是什么原因呢？ 我还没有找到答案。
个人猜想：

function y 在编译的时候，会进行搜索 g 变量，首先自己 肯定是找不到的，然后就是去找 上层的环境
如果发现 自己本身不存在的变量，出现在上层环境的 `变量环境`中，会将该 `变量环境` 压入到  `当前函数` 的 `作用域链`中.
就像： 
```
local:
  this: window
block:
  g: 2
global:
  ...
```
如果该变量出现在 `词法环境`, 那么就是闭包的形式。

### 最后
顺便说一句 chrome 中显示的 scope中： local 表示的 `LexicalEnvironment` 词法环境 ， block 表示的是 `NewDeclarativeEnvironment` 变量环境。

结束了, see you!