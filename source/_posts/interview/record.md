---
title: 前端知识点梳理
date: 2022-11-07 12:26:00
description: 前端主要知识点梳理，用于备忘，同时也复习准备面试，希望一切顺利...
---

#### 有言在先
梳理前端开发知识点，回顾前端知识树，顺便记录一些常见的面试题目，不停更新中....
同时也为了准备面试...

#### 代码输出
常见笔试题:

``` javascript
(function(){
    var x = y = 1; // 拆分一下就是 var x = 1; y = 1;
})();

console.log(y); // 1
console.log(x); // Err
```

``` javascript
function Foo() {
  this.getValue = function() {
    console.log(2);
  }
}

Foo.prototype.getValue = function() {
  console.log(3)
} 

function Foo2() {}

Foo2.getValue = function() { // 定义到了Foo2构造函数上，他的实例无法访问到
  console.log(5);   
}

Foo2.prototype = Foo.prototype
const obj2 = new Foo2();
obj2.getValue(); // 3   // Foo2构造函数上没有，会去他的构造函数的原型 Foo.prototype上找
```

``` javascript
var A = function() {};
A.prototype.n = 1;
var b = new A(); // 实例化b的时候A的原型上只有n = 1
A.prototype = {
 n: 2,
 m: 3
}
var c = new A();

console.log(b.n); // 1
console.log(b.m); // undefined
console.log(c.n); // 2
console.log(c.m); // 3




function Test() {}

Object.prototype.printName = function() {
 console.log('Object');
}

Function.prototype.printName = function() {
 console.log('Function');
}

Test.printName(); // Function

var obj = new Test();
obj.printName(); // Object
```

``` javascript
function Parent(){
   this.a = 'Parent'
}

function Tom() {
   this.a = 'Tom'
}

Parent.__proto__.print = function(){ // Parent.__proto__ === Function.prototype
   console.log(this.a)
}

Parent.print()
Tom.print() // print方法添加到了Function的原型上，所以可以访问到

var child = new Parent()
child.print() // Parent.prototype.__proto__ === Object.prototype

执行以上代码，将分别输出什么？（请勿使用浏览器开发者工具调试） D

A. 'undefined' 'Uncaught TypeError ...' 'Parent'
B. 'Parent' 'Uncaught TypeError ...' 'Uncaught TypeError ...'
C. 'Parent' 'Tom' 'Uncaught TypeError ...'
D. 'undefined' 'undefined' 'Uncaught TypeError ...'
```

``` javascript
javascriptasync function fetchData(){
    return await new Promise((resolve, reject)=>{
        setTimeout(reject, 1000)
    })
}

try{
    fetchData()
    console.log('success')

}catch(e){
    console.log('error')
}

// success
// try catch 捕获不到Promise的错误
// promise 内部的抛出的错误，不会反映到外部
```

``` javascript
new Promise((resolve, reject) => {
    setTimeout(() => resolve(123), 1000);
})
.then((res) => {
    console.log("1", res);
    return 456;
})
.then((res) => {
    console.log("2", res);
    return Promise.resolve(789);
})
.then((res) => {
    console.log("3", res);
    throw 56;
})
.then((res) => console.log("4", res))
.catch((err) => console.log("err", err));
 
// 1 123    2 456   3 789   err 56
```

``` javascript
let foo={}

let obj = {}

foo[obj]='hello' // js Object对象的key只能是字符串, 所以会进行一个隐示转换为[Object Object]

let obj2 = {a:1}

foo[obj2] = 'hello2' // Object的key不能重复，后声明的会覆盖前边的

console.log(foo[obj], foo[obj2]) // 'hello2' 'hello2'
```

#### 为什么要有微任务
> 宏任务：先进先出的原则
在 JS 的执行过程中 或者说在页面的渲染过程中，宏任务是按照先进先出的原则执行，我们并不能准确的控制 这些任务被推进任务队列里，但是我们这个时候如果出来一个非常高优先级的任务，这个时候该怎么办？如果我们只有宏任务，再往任务队列里面推一个，秉承着先进先出的原则，那么它肯定是最后一个执行的，所以要引入微任务；

##### 宏任务和微任务执行顺序
- 代码开始执行，创建一个全局调用栈，script作为宏任务执行
- 执行过程过同步任务立即执行，异步任务根据异步任务类型分别注册到微任务队列和宏任务队列
- 同步任务执行完毕，查看任务队列
    - 若存在微任务，将微任务队列全部执行(包括执行微任务过程中产生的新微任务)
    - 若无微任务，查看宏任务队列，执行第一个宏任务，宏任务执行完毕，查看微任务队列，重复上述操作，直至宏任务队列为空

##### NOde执行顺序
- ① timers定时器：执行已经安排过的，setTimeout 和 setInterval 的回调函数；
- ② pending callback 待定回调：执行延迟到下一个循环迭代的I/O回调；
- ③ idle，prepare：仅系统内部使用；
- ④ poll：检索新的I/O事件，执行与I/O相关的回调
- ⑤ check：执行setImmediate() 回调函数
- ⑥ close callback：socket.on('close', (  )=>{  })

##### 执行顺序
``` javascript
async function async1() {
  console.log("async1 start");
  await async2();
  console.log("async1 end");
}
async function async2() {
  console.log("async2");
}
console.log("script start");
setTimeout(() => {
  console.log("setTimeout");
}, 0);
async1();
new Promise((resolve) => {
  console.log("promise1");
  resolve();
}).then(() => {
  console.log("promise2");
});
console.log("script end");
 
// 1. script start
// 2. async1 start
// 3. async2
// 4. promise1
// 5. script end
// 6. async1 end
// 7. promise2
// 8. setTimeout
```

``` javascript
console.log("start");
setTimeout(() => {
  console.log("children2");
  Promise.resolve().then(() => {
    console.log("children3");
  });
}, 0);
 
new Promise((resolve, reject) => {
  console.log("children4");
  setTimeout(() => {
    console.log("children5");
    resolve("children6"); // 此处大坑
  }, 0);
}).then((res) => {
  console.log("children7");
  setTimeout(() => {
    console.log(res);
  }, 0);
});
 
// 1. start
// 2. children4
/** 第一轮宏任务执行结束，尝试清空微任务队列,发现没有微任务，尝试执行下一轮宏任务   */
// 3. children2
/** 第二轮宏任务执行结束，尝试清空微任务队列,   */
// 4. children3
// 5. children5
/** 第三轮宏任务执行结束，尝试清空微任务队列,   */
// 6. children7
// 7. children6
```

``` javascript
const p = () => {
  return new Promise((resolve, reject) => {
    const p1 = new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve(1); // 这里 不会再输出了，因为 resolve(2) 已经把结果输出
      }, 0);
      resolve(2);
    });
    p1.then((res) => {
      console.log(res);
    });
    console.log(3);
    resolve(4);
  });
};
p().then((res) => {
  console.log(res);
});
console.log("end");
 
// 1. 3
// 2. end
// 3. 2
// 4. 4
```

##### 函数节流
时间戳写法，第一次立即执行:
``` javascript
const throttle = (fn, interval) => {
  let last = 0;
  return () => {
    let now = Date.now();
    if (now - last >= interval) {
      fn.apply(this, arguments);
    }
  };
};
const handle = () => {
  console.log(Math.random());
};
const throttleHandle = throttle(handle, 3000);
throttleHandle();
throttleHandle();
```
定时器写法:
``` javascript
const throttle = (fn, interval) => {
  let timer = null;
  return function () {
    let context = this;
    let args = arguments;
    if (!timer) {
      timer = setTimeout(() => {
        fn.apply(context, args);
        timer = null;
      }, interval);
    }
  };
};
const handle = () => {
  console.log(Math.random());
};
const throttleHandle = throttle(handle, 1000);
throttleHandle();
throttleHandle();
```
精准写法:
``` javascript
const throttle = (fn, delay) => {
  let timer = null;
  let startTime = Date.now();
  return function () {
    let curTime = null;
    let remainning = delay - (curTime - startTime);
    let context = this;
    let args = arguments;
    clearTimeout(timer);
    if (remainning <= 0) {
      fn.apply(context, args);
      startTime = Date.now();
    } else {
      timer = setTimeout(fn, remainning);
    }
  };
};
```

#### Promise

{% asset_image promise-process.png %}

##### Promise.resolve

- 参数是一个 Promise 实例那么Promise.resolve将不做任何修改、原封不动地返回这个实例
- 参数是一个thenable对象，Promise.resolve方法会将这个对象转为 Promise 对象，然后就立即执行thenable对象的then方法
``` javascript
let thenable = {
  then: function(resolve, reject) { // 直接被执行，promise会被resovle掉
    resolve(42); 
  }
};

let p1 = Promise.resolve(thenable);
p1.then(function(value) {
  console.log(value);  // 42
});
```
- 参数不是具有then方法的对象，或根本就不是对象，如果参数是一个原始值，或者是一个不具有then方法的对象，则Promise.resolve方法返回一个新的 Promise 对象，状态为resolved
``` javascript
const p = Promise.resolve('Hello');

p.then(function (s){
  console.log(s)
});
// Hello
```
- 不带有任何参数，Promise.resolve方法允许调用时不带参数，直接返回一个resolved状态的 Promise 对象

##### resolve()本质作用
- resolve()是用来表示promise的状态为fullfilled，相当于只是定义了一个有状态的Promise，但是并没有调用它；
- promise调用then的前提是promise的状态为fullfilled；
- 只有promise调用then的时候，then里面的函数才会被推入微任务中；

##### Promise.all

- 如果有一个报错了，那么整个 Promise.all(  )，就会返回一个 catch
- Promise.all( ) 会接受一个 Promise 数组，数组里面可以是 Promise 也可以是一个常量或者其他；执行情况为：Promise 里面的所有 Promise 执行完成以后才会返回结果

##### 实现 Promise.all
``` javascript
// 测试
const pro1 = new Promise((resolve, reject) => {
  setTimeout(() => {
    resolve("1");
  }, 1000);
});
const pro2 = new Promise((resolve, reject) => {
  setTimeout(() => {
    resolve("3");
  }, 2000);
});
const pro3 = new Promise((resolve, reject) => {
  setTimeout(() => {
    resolve("3");
  }, 3000);
});
 
// 测试题
const PromiseAll = (promiseArray) => {};
```

``` javascript
// 测试题
const PromiseAll = (promiseArray) => {
  return new Promise((resolve, reject) => {
    if (!Array.isArray(promiseArray)) {
      return reject(new Error("Type can only be array"));
    }
    const result = []; // promise 执行的结果集
    const promiseNums = promiseArray.length; // 当前循环次数
    let counter = 0; // 记录当前 promise 执行顺序，需要按照 传入的 promise 顺序返回
    for (let i = 0; i < promiseNums; i++) {
      Promise.resolve(promiseArray[i])
        .then((value) => {
          counter++;
          result.push(value);
          if (counter === promiseNums) {
            resolve(result);
          }
        })
        .catch((e) => reject(e));
    }
  });
};
 
console.log(
  PromiseAll([pro1, pro2, pro3])
    .then((res) => {
      console.log(res);
    })
    .catch((e) => {
      console.log(e);
    })
);
```

##### promisify
``` javascript
function promisify(fn){
  return (...args) => {
      // return Promise，符合题意的链式调用
      return new Promise((resolve, reject) => {
          fn.apply(this, [...args, (err, data) =>{
              if(!err){
                  return resolve(data);
              }
              return reject(err);
          }])
      })
  }
}
```

#### webpack
##### 优化
- 开启dll
- 提取公共代码
- thread-loader
    多进程打包，可以大大提高构建的速度
- cache-loader
- 开启热更新
- exclude & include
    - exclude：不需要处理的文件
    - include：需要处理的文件
- 构建区分环境
    - 开发环境：去除代码压缩、gzip、体积分析等优化的配置，大大提高构建速度
    - 生产环境：需要代码压缩、gzip、体积分析等优化的配置，大大降低最终项目打包体积
- CSS代码压缩
- JS代码压缩
- tree-shaking
    只打包用到的代码，没用到的代码不打包
- source-map类型
``` javascript
module.exports = {
  mode: 'development',
  devtool: 'eval-cheap-module-source-map'
}
module.exports = {
  mode: 'production',
  devtool: 'nosources-source-map'
}
````
- webpack-bundle-analyzer
- 模块懒加载
``` javascript
const routes = [
  {
    path: '/login',
    name: 'login',
    component: login
  },
  {
    path: '/home',
    name: 'home',
    // 懒加载
    component: () => import('../views/home/home.vue'),
  },
]
```
- Gzip
``` javascript
const CompressionPlugin = require('compression-webpack-plugin')

  plugins: [
    // 之前的代码...
    
    // gzip
    new CompressionPlugin({
      algorithm: 'gzip',
      threshold: 10240,
      minRatio: 0.8
    })
  ]
```
- 小图片转base64
    减少用户的http网络请求次数
- 配置hash
    改过的文件需要更新hash值，而没改过的文件依然保持原本的hash值，这样才能保证在上线后，浏览器访问时没有改变的文件会命中缓存，从而达到性能优化的目的
``` javascript
  output: {
    path: path.resolve(__dirname, '../dist'),
    // 给js文件加上 contenthash
    filename: 'js/chunk-[contenthash].js',
    clean: true,
  },
```

##### source Map
Sourcemap是什么？有什么作用？在生产环境怎么用？
`Sourcemap` 本质上是一个信息文件，里面储存着代码转换前后的对应位置信息。它记录了转换压缩后的代码所对应的转换前的源代码位置，是源代码和生产代码的映射。 `Sourcemap` 解决了在打包过程中，代码经过压缩，去空格以及 `babel` 编译转化后，由于代码之间差异性过大，造成无法`debug`的问题，简单说 `Sourcemap` 构建了处理前以及处理后的代码之间的一座桥梁，方便定位生产环境中出现 `bug` 的位置。因为现在的前端开发都是模块化、组件化的方式，在上线前对 js 和 css 文件进行合并压缩容易造成混淆。如果对这样的线上代码进行调试，肯定不切实际，`sourceMap` 的作用就是能够让浏览器的调试面版将生成后的代码映射到源码文件当中，开发者可以在源码文件中 debug，这样就会让程序员调试轻松、简单很多。

`Sourcemap` 的种类有很多， 在生产环境下可以用`process.env `判断一下。  `webpack`中可以在`devtool`中设置, 在开发环境中可以配置`devtool`为`cheap-module-source-map`，方便调试。生产环境下建议采用`none`的方式，这样做不暴露源代码。或者是`nosources-source-map` 的方式，既可以定位源代码位置，又不暴露源代码。


#### Vue 面试点
##### 响应式原理
{% pdf vue-responseive.pdf %}
##### Vue2  Vue3
##### Vue React
##### 更新机制
- 组件级别更新
- 异步更新机制
- nextTick问题
- 更新与宏任务以及微任务的关系

##### 双向数据绑定
##### Vue初始化
##### v-if v-show
    - v-show 只是 CSS 级别的 display: none; 和 display:block; 之间的切换，⽽ v-if 决定是否会选择代码块的内容（或组件）
    - 使⽤ v-show，⼀次性渲染完的，使⽤ v-if，只要意思对就好
    - 因为当 v-if="false" 时，内部组件是不会渲染的，所以在特定条件才渲染部分组件（或内容）时，可以先将条件设置为 false，需要时（或异步，⽐如 $nextTick）再设置为 true，这样可以优先渲染重要的其它内容，合理利⽤，可以进⾏性能优化

##### v-for v-if
    v-for 遍历必须为 item 添加 key，且避免同时使用 v-if
##### 数组方法
##### $set $watch
##### watch computed
- 计算属性是⾃动监听依赖值的变化，从⽽动态返回内容，监听是⼀个过程，在监听的值变化时，可以触发⼀个回调，并做⼀些事情。
- computed 具有缓存，当依赖的属性没有变化时，会直接返回数据，而watch的变量即使没有变化也会照常执行。
##### slot 复用
##### 长列表性能优化
    {% pdf Vue-long-list.pdf %}
##### 组件中 data 为什么是函数
因为组件是⽤来复⽤的，JS ⾥对象是引⽤关系，这样作⽤域没有隔离，⽽ new Vue 的实例，是不会被复⽤的，因此不存在引⽤对象的问题。
##### keep-alive
    缓存组件
##### 递归组件
- 要给组件设置 name
- 要有⼀个明确的结束条件。

##### v-model如何实现
- 双向数据流
- input 输入框中的v-model，Vue 在编译后会生成 input 事件，并且讲给定的值赋值给value.
- 自定义组件，需要手动实现 value的传递和 @input事件的绑定

``` html
<custom-component v-model="value"></custom-component>
```
``` javascript
<template>
<div>
{{ currentValue }}
<button @click="handleClick">Click</button>
</div>
</template>
<script>
export default {
    props: {
        value: {
            type: Number,
            default: 0
        }
    },
    data () {
        return {
            currentValue: this.value
        }
    },
    methods: {
        handleClick () {
            this.currentValue += 1;
            this.$emit('input', this.currentValue);
        }
    },
    watch: {
        value (val) {
            this.currentValue = val;
        }
    }
}
</script>
```

##### 生命周期
- 渲染过程
    父beforeCreate->父created->父beforeMount->子beforeCreate->子created->子beforeMount->子mounted->父mounted
- 更新过程
    父beforeUpdate->子beforeUpdate->子updated->父updated
- 销毁过程
    父beforeDestroy->子beforeDestroy->子destroyed->父destroyed

##### 虚拟DOM
- 可能涉及手写

{% pdf Virtual-DOM.pdf %}

##### Render 函数


##### Vuex
- action
    - 用于通过提交mutation改变数据
    - 会默认将自身封装为一个Promise
    - 可以包含任意的异步操作
- mutation
    - 通过提交commit改变数据
    - 只是一个单纯的函数
    - 不要使用异步操作，异步操作会导致变量不能追踪
- action与 mutation 区别
主要的区别是，actions 可以执⾏异步。actions 是调⽤mutations⽽ mutations 来修改 store
- Vuex 与 event 区别

##### 组件通信
- 父子
    - 事件
    - 为什么不能修改父组件数据
- 兄弟
    event / vuex
- 跨级
    provider/inject的问题

#### 网络
##### TCP
{% pdf tcp.pdf %}

##### 三次握手/四次挥手
由于HTTP 协议属于半双工协议， 所以在断开连接需要四次挥手，

##### 链接建立过程

- 浏览器查询DNS
- DNS 解析，分析服务器地址
- 建立链接

##### 请求过程
- HTTP/HTTP1.1
- 请求连接复用
- keep-alive
- expire
- cache-control
- max-age
- HTTPS
- websocket
    首次协议升级，HTTP方式，建立双通道socket协议。

##### 请求头/体参数
- 请求头和请求体采用空行隔开
- GET请求参数放在参数后面进行连接，POST请求参数放在请求中
- 文件传输，数据以  --boundary 进行分割二进制数据，最后以 --boundary-- 进行标记结束 

##### 中间人攻击

中间服务器伪造自己的公钥发送给客户端，拿到数据之后用自己的私钥进行解密，并使用真正的服务端的公钥进行加密，然后发送给服务端。

##### 加密方式

数据对称加密，并且对内容进行hash生成指纹，密钥/证书 非对称加密

##### 请求次数
浏览器建立链接的个数限制，以及链接的复用。

##### 网络优化

##### 代理的原理
- Http Proxy 
- Http Tunnel
- 中间人模式

##### 浏览器缓存机制

| field | diff |
| --- | --- |
| Expire | 绝对时间 |
| Cache-contro | 相对时间 |
| Etag | Hash值 |

##### http2与http以及http3
HTTP2
在 HTTP/1 中，为了性能考虑，我们会引⼊雪碧图、将⼩图内联、使⽤多个域名等等的⽅式。这⼀切都是因为浏览器限制了同⼀个域名下的请求数量（Chrome 下⼀般是限制六个连接），当⻚⾯中需要请求很多资源的时候，队头阻塞（Head of line blocking）会导致在达到最⼤请求数量时，剩余的资源需要等待其他资源请求完成后才能发起请求。

在 HTTP/2 中引⼊了多路复⽤的技术，这个技术可以只通过⼀个TCP 连接就可以传输所有的请求数据。多路复⽤很好的解决了浏览器限制同⼀个域名下的请求数量的问题，同时也接更容易实现全速传输，毕竟新开⼀个 TCP 连接都需要慢慢提升传输速度。

二进制传输
HTTP/2 中所有加强性能的核⼼点在于此。在之前的 HTTP 版本中，我们是通过⽂本的⽅式传输数据。在 HTTP/2 中引⼊了新的编码机制，所有传输的数据都会被分割，并采⽤⼆进制格式编码。

{% asset_image http2-transition.png %}

多路复⽤
在 HTTP/2 中，有两个⾮常重要的概念，分别是帧（frame）和流（stream）。帧代表着最⼩的数据单位，每个帧会标识出该帧属于哪个流，流也就是多个帧组成的数据流。多路复⽤，就是在⼀个 TCP 连接中可以存在多条流。换句话说，也就是可以发送多个请求，对端可以通过帧中的标识知道属于哪个请求。通过这个技术，可以避免 HTTP 旧版本中的队头阻塞问题，极⼤的提⾼传输性能.
{% asset_image http2-connect.png %}

Header Push
在 HTTP/1 中，我们使⽤⽂本的形式传输 header，在 header 携带cookie 的情况下，可能每次都需要重复传输⼏百到⼏千的字节。在 HTTP /2 中，使⽤了 HPACK 压缩格式对传输的 header 进⾏编码，减少了 header 的⼤⼩。并在两端维护了索引表，⽤于记录出现过的 header ，后⾯在传输过程中就可以传输已经记录过的 header的键名，对端收到数据后就可以通过键名找到对应的值。

服务端 Push
在 HTTP/2 中，服务端可以在客户端某个请求后，主动推送其他资源。可以想象以下情况，某些资源客户端是⼀定会请求的，这时就可以采取服务端 push 的技术，提前给客户端推送必要的资源，这样就可以相对减少⼀点延迟时间。当然在浏览器兼容的情况下你也可以使⽤prefetch 。

- HTTP3
因为 HTTP/2 使⽤了多路复⽤，⼀般来说同⼀域名下只需要使⽤⼀个 TCP 连接。当这个连接中出现了丢包的情况，那就会导致HTTP/2 的表现情况反倒不如 HTTP/1 了.
因为在出现丢包的情况下，整个 TCP 都要开始等待重传，也就导致了后⾯的所有数据都被阻塞了。但是对于 HTTP/1 来说，可以开启多个 TCP 连接，出现这种情况反到只会影响其中⼀个连接，剩余的TCP 连接还可以正常传输数据。

Google 就更起炉灶搞了⼀个基于 UDP 协议的QUIC 协议，并且使⽤在了 HTTP/3 上，当然 HTTP/3 之前名为HTTP-over-QUIC，从这个名字中我们也可以发现，HTTP/3 最⼤的改造就是使⽤了 QUIC.

##### HTTP1.1 队头阻塞
http1.1 来自基于文本协议在网络上传输并可读的时代。

{% asset_image http1.1-packetization.png %}

这主要是因为协议本质上是纯文本的，在资源块（resource chunks）之间不使用分隔符。
在本例中，浏览器基于 HTTP/1.1上 请求简单的script.js文件（绿色），图1显示了服务器对该请求的响应。我们可以看到 HTTP 方面本身很简单：它只是在明文文件内容或“有效荷载”（payload）前面直接添加一些文本"headers"（红色）。然后，头（Headers）+ 有效荷载（payload）被传递到底层 TCP（橙色），以便真实传输到客户端。对于这个例子，假设我们不能将整个文件放入一个 TCP 包中，并且必须将它分成两部分。

现在让我们看看当浏览器也请求style.css时发生了什么，同时加载script.js和 style.css:
{% asset_image http1.1-packetization-2.png %}

在本例中，当script.js的响应传输之后，我们发送style.css（紫色）。style.css的头部（headers）和内容只是附加在 JavaScript（JS）文件之后。接收者使用Content-Length header 来知道每个响应的结束位置和另一个响应的开始位置（在我们的简化示例中，script.js是1000字节，而style.css只有600字节）。
在这个包含两个小文件的简单示例中，所有这些似乎都很合理。但是，假设 JS 文件比 CSS 大得多（比如说 1MB 而不是 1KB）。这种情况下，在下载整个JS文件之前，CSS 必须等待，尽管它要小得多，其实可以更早地解析/使用。更直接地将其可视化，使用数字 1 表示large_script.js和 2 表示`style.css`，我们会得到这样的结果：

> 11111111111111111111111111111111111111122

你可以看到这是一个队头阻塞问题的例子！现在你可能会想：这很容易解决！只需让浏览器在JS文件之前请求CSS文件！然而，至关重要的是，浏览器无法预先知道这两个文件中的哪一个在请求时会成为更大的文件。这是因为没有办法在HTML中指明文件有多大（类似这样的东西很不错，HTML工作组：\<img src="thisisfine.jpg" size="15000" \/>）。

如果直接在Http1.1中直接使用多路复用，既上面的script.js和style.css 交替加载，那么由于 Http1.1 采用的是文本协议，会导致无法知道加载的文件类型。
{% asset_image http1.1-multiplexing.png %}

比如上面 浏览器解析js 脚本，并知道了脚本的长度为 1000，那么由于后面的部分都是纯文本，并由于css的头部会由于多路复用导致提前返回，会被当做是js的内容解释为js 的一部分。
而后面的css的文件则加载不到真正的css头部，导致文件无法解析。

这就是为什么浏览器开始为 HTTP/1.1 上的每个页面加载打开多个并行 TCP 连接（通常为6个）。这样，请求可以分布在这些单独的连接上，并且不再有队头阻塞。也就是说，除非每页有超过6个资源…这当然是很常见的。这就是在多个域名上"分片"（sharding）资源的实践(img.mysite.com, static.mysite.com, 等）和 CDN 的由来。
其实多域名，还有cookie的好处。

##### HTTP2 队头阻塞
HTTP/2 的目标非常明确：我们能够回到单个 TCP 连接，解决队头阻塞问题。HTTP/1.1 因为没有办法分辨一个块属于哪个资源，或者它在哪里结束，另一个块从哪里开始。
HTTP2 在每个资源块前添加了桢（frames）

{% asset_image http1.1-vs-http2-packetization.png %}

HTTP/2 在每个块前面放置一个所谓的数据帧（DATA frame）。这些数据帧主要包含两个关键的元数据。
首先：下面的块属于哪个资源。每个资源的“字节流（bytestream）”都被分配了一个唯一的数字，即流id（stream id）。第二：块的大小是多少。协议还有许多其他帧类型，图5也显示了头部帧（HEADERS frame）。这再次使用流id（stream id）来指出这些头（headers）属于哪个响应，这样甚至可以将头（headers）从它们的实际响应数据中分离出来。
{% asset_image http2-packetization-muliplexing.png %}
通过上面的数据帧，浏览器知道属于不同的资源。
因此，通过"framing"单个消息，HTTP/2 比 HTTP/1.1 更加灵活。它允许在单个 TCP 连接上通过交错排列块来多路传输多个资源。
它还解决了第一个资源缓慢时的队头阻塞问题：而不必等待查询数据库生成的index.html，服务器可以在等待index.html时开始发送其他资源。

关于对于不同的资源的帧，连接如何对资源块进行调度是由系统进行驱动。大概有以下几种方式：
- 公平多路复用（例如两个渐进的 JPEGs）：12121212
- 加权多路复用（2是1的两倍）：22122122121
- 反向顺序调度（例如2是密钥服务器推送的资源）：22221111
- 部分调度（流1被中止且未完整发送）：112222

HTTP2 解决了 HTTP 的队头阻塞问题，但是还有TCP队头阻塞。

##### TCP 队头阻塞 
HTTP2 不同的数据帧，会在TCP层同样分割为不同的 TCP包，当网络进行抖动时，突然出现某个数据包丢失之后，那么在上层的HTTP 帧发现缺少了某个中间数据，则会进行阻塞等待重传的数据回来。
其实TCP 并不知道上层的数据流，是文本还是二进制，当前面的数据缺失时，只能选择阻塞，等待数据重传，才能进行将整体的数据传递给浏览器。
TCP 不知道 HTTP/2 的独立流（streams）这一事实意味着 TCP 层队头阻塞（由于丢失或延迟的数据包）也最终导致 HTTP 队头阻塞.

##### HTTP1.1 管道

<a href="https://calendar.perfplanet.com/2020/head-of-line-blocking-in-quic-and-http-3-the-details/" target="_blank">队头阻塞问题</a>

#### 跨域方案
同源策略（Same origin policy）是一种约定，协议，端口，主机必须完全完全相同。

##### 跨域请求是否发出
不通浏览器不同策略，chrome会将请求发出。如果是复杂请求，则会发送option预检请求。

##### cors
服务端进行配置可通过的域名，已经方法等。默认不带cookie, 如果要cookie则需要配置 Access-Control-Allow-Credentials: true 
```
Access-Control-Allow-Origin: <origin> | * // 授权的访问源 
Access-Control-Max-Age: <delta-seconds> // 预检授权的有效期，单位：秒 
Access-Control-Allow-Credentials: true | false // 是否允许携带 
Cookie Access-Control-Allow-Methods: <method>[, <method>]* // 允许的请求动词 
Access-Control-Allow-Headers: <field-name>[, <field-name>]* // 额外允许携带的请求头 
Access-Control-Expose-Headers: <field-name>[, <field-name>]* // 额外允许访问的响应头
```
并且在 客户端中需要配置

``` javascript
var xhr = new XMLHttpRequest();
xhr.withCredentials = true;
```

###### cors处理流程

- 首先查看http头部有无origin字段；
- 如果没有，或者不允许，直接当成普通请求处理，结束；
- 如果有并且是允许的，那么再看是否是preflight(method=OPTIONS)；
- 如果不是preflight（简单请求），就返回Allow-Origin(必须的)、[Allow-Credentials，Access-Control-Expose-Headers]，并返回正常内容。
- 如果是preflight（预先请求），正式通信前多一次预检请求，就返回Allow-Headers、Allow-Methods等，内容为空


###### 复杂请求/简单请求
- 简单请求：一次请求
- 非简单请求：两次请求，在发送数据之前会先发一次请求用于做“预检”，只有“预检”通过后才再发送一次请求用于数据传输。
- CORS需要浏览器和服务器同时支持。目前，所有浏览器都支持该功能
- 浏览器将CORS请求分成两类：简单请求（simple request）和非简单请求（not-so-simple request）
- 如果请求满足以下两种情况，就是简单请求，否则就是非简单请求
    - 请求方法是以下三种方法之一：
        HEAD
        GET
        POST
    - HTTP的头信息不超出以下几种字段：
        Accept
        Accept-Language
        Content-Language
        Last-Event-ID
        Content-Type：只限于三个值urlencoded、form-data、text/plain
- 简单请求发送一次请求，数据拿回来，但是被浏览器同源策略拦截了
- 非简单请求发送两次，一次options请求，如果允许再发真正的请求，如果不允许就不发了
- 通过跨域资源共享解决跨域
    - 简单请求解决：其实就是在响应头中加一些东西
``` python
    res= HttpResponse('ok')
    res['Access-Control-Allow-Origin']='*'
    return res
```
    - 非简单请求的解决，也是在响应头中加东西
``` python
    if request.method == 'OPTIONS':
    res['Access-Control-Allow-Methods'] = 'DELETE,'
    res['Access-Control-Allow-Headers'] = 'Content-Type'
```

简单请求：浏览器在发出CORS 请求时会在 头信息 之中增加一个`Origin`字段。许可访问时服务器的返回会多出`3`个字段：

| 字段 | 必须 or 可选 | 解释 |
| --- | --- | --- |
| Access-Control-Allow-Origin | 必须 | 允许跨域的源，要么是一个*，表示接受任意域名的请求。|
| Access-Control-Allow-Credentials | 可选 | 表示服务器是否允许客户端发送Cookie。默认情况下，Cookie可以包含在请求中，一起发给服务器，如果服务器不需要浏览器发送Cookie，删除该字段即可。|
| Access-Control-Expose-Headers | 可选 | CORS请求时，XMLHttpRequest对象的getResponseHeader()方法只能拿到6个基本字段：Cache-Control、Content-Language、Content-Type、Expires、Last-Modified、Pragma。如果想拿到其他字段，就必须在Access-Control-Expose-Headers里面指定。如指定Access-Control-Expose-Headers: FooBar，则可通过getResponseHeader(‘FooBar’)获取FooBar字段的值。|

###### 关于预检
- 请求方式：OPTIONS
- `预检`其实做检查，检查如果通过则允许传输数据，检查不通过则不再发送真正想要发送的消息
- 如何`预检`
    - 如果复杂请求是PUT等请求，则服务端需要设置允许某请求，否则`预检`不通过
        Access-Control-Request-Method
    -如果复杂请求设置了请求头，则服务端需要设置允许某请求头，否则`预检`不通过
        Access-Control-Request-Headers

##### nginx
反向代理跨域，使用代理服务器。步骤：1. 把访问其它域的请求替换为本域的请求。2. 服务器端的动态脚本负责将本域的请求转发成实际的请求。更加直接，因为同源限制是浏览器实现的.

##### domain
通过修改document.domain来跨子域
该⽅式只能⽤于⼆级域名相同的情况下，⽐如 a.test.com 和b.test.com 适⽤于该⽅式。
只需要给⻚⾯添加 document.domain = 'test.com' 表示⼆级域名都相同就可以实现跨域


##### window.name
使用window.name来进行跨域

##### postmessage
使用HTML5中新引进的window.postMessage方法来跨域传送数据.这种⽅式通常⽤于获取嵌⼊⻚⾯中的第三⽅⻚⾯数据。⼀个⻚⾯发送消息，另⼀个⻚⾯判断来源并接收消息
``` javascript
// 发送消息端
window.parent.postMessage('message', 'http://test.com')
// 接收消息端
var mc = new MessageChannel()
mc.addEventListener('message', event => {
    var origin = event.origin || event.originalEvent.origin
    if (origin === 'http://test.com') {
        console.log('验证通过')
    }
})
```

##### jsonp
JSON with Padding的略称。利用script标签的src属性来实现跨域，将前端方法作为参数传递到服务器端，然后由服务器端注入参数之后再返回，实现服务器端向客户端通信，仅支持get方法。
JSONP 的原理很简单，就是利用 `<script>` 标签没有跨域限制的漏洞。通过 `<script>` 标签指向一个需要访问的地址并提供一个回调函数来接收数据当需要通讯时。

```js
<script src="http://domain/api?param1=a&param2=b&callback=jsonp"></script>
<script>
    function jsonp(data) {
    	console.log(data)
	}
</script>    
```

JSONP 使用简单且兼容性不错，但是只限于 `get` 请求。

在开发中可能会遇到多个 JSONP 请求的回调函数名是相同的，这时候就需要自己封装一个 JSONP，以下是简单实现

```js
function jsonp(url, jsonpCallback, success) {
  let script = document.createElement('script')
  script.src = url
  script.async = true
  script.type = 'text/javascript'
  window[jsonpCallback] = function(data) {
    success && success(data)
  }
  document.body.appendChild(script)
}
jsonp('http://xxx', 'callback', function(value) {
  console.log(value)
})
```

##### websocket
webSocket，不受同源策略影响，它不使用HTTP协议

#### JS 基础
##### 闭包
借助《你不知道的javascript》中的解释: 当函数可以记住并访问所在的词法作用域，即使函数是在当前词法作用域之外执行，这时就产生了闭包。
函数在当前定义的词法作用域外运行时，如果函数内引用了函数外的变量，那么函数会通过作用域链导致外部作用域无法释放，并且保存在内存中，这样就形成了一层闭包，保存着引用的变量。

常见问题
- setTimeout
``` javascript
  function wait(message) {|
    setTimeout(function timer () {
      console.log(message);
    }, 1000)
  }

  wait('hello world');
```
  这里其实timer具有涵盖了wait的作用域，因此还保留着对message的引用。

- 循环问题
``` javascript
for (var i =0 ; i <= 5; i++) {
  setTimeout(function  timer() {
    console.log(i)
  }, i * 1000)
}
```
  这里期望每次一次打印数字0-5，每秒一个，每次一个。
  其实这里真实情况会每秒一次的打印数字 6.

  > 大致的原因，是每次循环，都会执行setTimeout方法，将timer函数压入到定时器队列中，而每个timer的外包闭包中的作用域其实都是同一个，那么当定时器执行的时候，i已经由于for循环的原因早早的自增到 6 了。

  解决办法，考虑将 timer 中要 访问的数据和  for 隔离开即可。

  ``` javascript
  for (var i = 0; i <= 5; i++) {
    (function (j) {
      setTimeout(function timer () {
        console.log(i)
      }, i * 1000)
    })(i);
  }
  ```
  这样就把for 循环中的 i 传递给了 匿名函数中的 j, 那么每个 timer 外包的闭包中的此法作用域都是不同的，并且保存的 j 的也不一样。但是这些匿名函数依旧如果引用了外面的变化，那么他们也会引用同一个外部此法作用域。

  在ES6 中还有一种方案解决上面的 循环问题:
  ``` javascript
    for (let i = 0; i <=5 ; i++) {
      setTimeout(function timer() {
          console.log(i)
      }, i * 1000)
    }
  ```
  > 这里涉及到了 let 定义的变量和 var 定义的变量存储的问题。 由于 let 定义的变量是存储在 词法环境中，并且 let 定义变量是具有块级作用域，所以在for 语句的条件下，timer闭包中保存的作用域是不一样的。

  上面代码和下面一样

  ``` javascript
  {
    let j;
    for (j = 0; j <= 5; j++) {
      {
        let j = i;
        setTimeout(function timer() {
          console.log(i);
        }, i * 1000)
      }
    }
  }
  ```
  所以这里其实多了一层块级作用域。

  另外，其实函数嵌套的情况，是不一定会产生闭包的，或者说，即使产生了， 在函数执行完，闭包就已经被释放了。
  ``` javascript
    function A () {
      var a = 0;
      function B() {
        console.log(a);
      }
      
      B();  // 在 B 执行完之后，就已经作用域就已经释放了。
    }
  ```
  其实这根本就没有满足上面的条件，在词法作用域之外的地方执行，但是如果将内部函数进行 return 之后，就会导致函数的执行环境变化，从而产生闭包。

##### 函数作用域 
函数作用域，属于函数的全部变量都可以在整个函数的范围内使用及复用（事实上在嵌套的作用域中也可以使用）。

##### try/catch
catch分句会创建一个 块级作用域，声明的变量只会在catch内部有效。

##### 词法作用域
- 简单的说，词法作用域就是定义在词法阶段的作用域，换句话说就是由你在写代码时将变量定义和块作用域写在哪里决定的。
- 无论函数在哪里被调用，无论它如何被调用，它的词法作用域都只由函数被声明时所处的位置决定。

但是存在一些可以欺骗词法作用域的方法，通常用来修改词法作用域
- eval
``` javascript
function foo(str, a) {
  eval(str); // 欺骗
  console.log(a, b)
}
var b = 2;
foo('var b = 3;', 1);
```
eval执行的代码`var b = 3`,会导致屏蔽掉外部作用域中的同名变量b.
- with

``` javascript
  function foo (obj) {
    with(obj) {
      a = 2;  // 如果 obj 中不存在a属性，会导致导致全局作用域的污染
    }
  }
```

一般不推荐使用，性能太差

##### 作用域链
简单说，JS 函数在执行的过程中，如果发现找不到变量，会沿着作用域链一级一级的往上查询。
但是如果在当前作用域中没有查到值，就会向上级作用域去查，直到查到全局作用域，这么一个查找过程形成的链条就叫做作用域链。
而JS 在执行的时候，会将 函数定义所在的 词法作用域压入到 队列中。

根据 js高程的说法，作用域链队列的最前端则为当前 函数的函数作用域，其实保存的是环境的是变量对象，如果环境是函数，则将活动对象作为变量对象。
作用域链中的下一个变量对象来自包含（外部）环境，而再下一个对象则来自下一个包含环境。这样一直延伸到全局执行环境，全局执行环境的变量对象时钟都是作用域链的最后一个对象。

每次函数被创建时，会预先创建一个作用域链，作用域链的外部函数的活动对象处于第一位，外部函数的外部函数活动对象为第二位，一直到作用域链的终点的全局作用域。并且作用域链会被保存在函数的内部属性[[scope]]中。
当函数执行的时候，会创建一个执行环境（execution contentx), 并将函数的[[scope]]属性拷贝到 执行环境的作用域链。然后将当前函数的变量对象作为活动对象推入执行环境作用域链的前端。

##### 原型 and 原型链
##### ES6
##### Promise
``` javascript
    async function async1() {
        console.log("AAAA");
        async2();
        console.log("BBBB");
    }
    async function async2() {
        console.log("CCCC");
    }

    console.log("DDDD");
    setTimeout(function () {
        console.log("FFFF");
    }, 0);

    async1();
    new Promise(function (resolve) {
        console.log("GGGG");
        resolve();
    }).then(function () {

        console.log("HHHH");
    });

    console.log("IIII");
```
两种区别:

```javascript
    //第一种
    promise.then((res) => {
        console.log('then:', res);
    }).catch((err) => {
        console.log('catch:', err);
    })
    //第二种
    promise.then((res) => {
        console.log('then:', res);
    }, (err) => {
        console.log('catch:', err);
})
```
第一种 catch 方法可以捕获到 catch 之前整条 promise 链路上所有抛出的异常。
第二种 then 方法的第二个参数捕获的异常依赖于上一个 Promise 对象的执行结果。
promise.then(successCb, faildCd) 接收两个函数作为参数，来处理上一个promise 对象的结果。then f 方法返回的是 promise 对象。第一种链式写法，使用catch，相当于给前面一个then方法返回的promise 注册回调，可以捕获到前面then没有被处理的异常。第二种是回调函数写法，仅为为上一个promise 注册异常回调。
如果是promise内部报错 reject 抛出错误后，then 的第二个参数就能捕获得到，如果then的第二个参数不存在，则catch方法会捕获到。 
如果是then的第一个参数函数 resolve 中抛出了异常，即成功回调函数出现异常后，then的第二个参数reject 捕获捕获不到，catch方法可以捕获到。

##### Generator
Generator 算是 ES6 中难理解的概念之⼀了，Generator 最⼤的特点就是可以控制函数的执⾏.

##### Generator 与 Promise关系

##### async and await
- ⼀个函数如果加上 async ，那么该函数就会返回⼀个 Promise.
- async 就是将函数返回值使⽤ Promise.resolve() 包裹了下，和 then 中处理返回值⼀样，并且 await 只能配套 async 使⽤
``` javascript
async function test() {
    let value = await sleep()
}
```
> async 和 await 可以说是异步终极解决⽅案了，相⽐直接使⽤Promise 来说，优势在于处理 then 的调⽤链，能够更清晰准确的写出代码，毕竟写⼀⼤堆 then 也很恶⼼，并且也能优雅地解决回调地狱问题。当然也存在⼀些缺点，因为 await 将异步代码改造成了同步代码，如果多个异步代码没有依赖性却使⽤了 await 会导致性能上的降低.

##### 定时器函数
常⻅的定时器函数有 setTimeout、setInterval、requestAnimationFrame
- JS 是单线程执⾏的，如果前⾯的代码影响了性能，就会导致 setTimeout 不会按期执⾏.
- setInterval，其实这个函数作⽤和setTimeout 基本⼀致，只是该函数是每隔⼀段时间执⾏⼀次回调函数。

> setTimeout 和 setInterval属于宏任务; requestAnimationFrame属于“渲染任务”（调用GUI 引擎），执行优先级在宏任务前，微任务之后

每个tick执行逻辑如下：
> ...->上一个宏任务 -> 微任务(下一个宏任务前的所有微任务) -> 渲染任务 -> 下一个宏任务 ->...

测试:

``` javascript
setTimeout(function(){
  console.log("我是setTimeout", new Date().getTime())
},5)

requestAnimationFrame(function(){ //浏览器刷新频率是16.7ms左右，远大于5ms
  console.log("我是requestAnimationFrame", new Date().getTime());
})  

new Promise(function(resolve){
  resolve('我是微任务')
}).then(res=>{
  console.log(res, new Date().getTime());
})
//下边是打印信息：

//我是微任务 1662356658402
//我是requestAnimationFrame 1662356658403
//我是setTimeout 1662356658408
```

通常来说不建议使⽤ setInterval。第⼀，它和 setTimeout ⼀样，不能保证在预期的时间执⾏任务。第⼆，它存在执⾏累积的问题
``` javascript
function demo() {
    setInterval(function(){
        console.log(2)
    },1000)
    sleep(2000)
}

demo()
```
以上代码在浏览器环境中，如果定时器执⾏过程中出现了耗时操作，多个回调函数会在耗时操作结束以后同时执⾏，这样可能就会带来性能上的问题。如果你有循环定时器的需求，其实完全可以通过requestAnimationFrame 来实现.
``` javascript
function setInterval(callback, interval) {
    let timer
    const now = Date.now
    let startTime = now()
    let endTime = startTime
    const loop = () => {
        timer = window.requestAnimationFrame(loop)
        endTime = now()
        if (endTime - startTime >= interval) {
        startTime = endTime = now()
        callback(timer)
    }
}
timer = window.requestAnimationFrame(loop)
    return timer
}
let a = 0
setInterval(timer => {
    console.log(1)
    a++
    if (a === 3) cancelAnimationFrame(timer)
}, 1000)
```

requestAnimationFrame ⾃带函数节流功能，基本可以保证在 16.6 毫秒内只执⾏⼀次（不掉帧的情况下），并且该函数的延时效果是精确的，没有其他定时器时间不准的问题，当然你也可以通过该函数来实现 setTimeout。

requestAnimationFrame 优点
- 动画每一帧的执行的间隔时间紧跟浏览器的刷新频率，动画更流畅，不会掉帧。
- 在隐藏或不可见的元素中，requestAnimationFrame 将不会进行重绘或回流；
- requestAnimationFrame 是由浏览器专门为动画提供的 API，在运行时浏览器会自动优化方法的调用，如果浏览器在后台运行或者该页面tab在后台运行时，动画会自动暂停。

##### GUI 渲染引擎
GUI 渲染引擎，用来处理浏览器的渲染操作，在 js 中渲染操作也是异步的。比如 操作DOM的代码会在事件队列中生成一个渲染任务，js 执行到这个任务时就会去调用 GUI 引擎渲染。
浏览器为了能够使得JS内部macro-task(宏任务)与 DOM 任务能够有序的执行（如果在 GUI 渲染的时候,js 改变了dom,那么就会造成渲染不同步），会在一个macro-task执行结束后，在下一个macro-task执行开始前，调用 GUI 引擎渲染对页面进行重新渲染，并会阻塞 js引擎计算。而micro-task(微任务)不涉及DOM操作，在渲染任务前执行.


简单版动画:
``` javascript
//简单的兼容性处理
window.requestAnimationFrame = (function() {
    return window.requestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.mozRequestAnimationFrame ||
        function(callback) {
            window.setTimeout(callback, 1000/60);
        }
})();
```
兼容版本:

``` javascript
(function() {
    var lastTime = 0;
    var vendors = ['webkit', 'moz'];
    //如果window.requestAnimationFrame为undefined先尝试浏览器前缀是否兼容
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x] + 'RequestAnimationFrame'];
        window.cancelAnimationFrame = window[vendors[x] + 'CancelAnimationFrame'] ||  window[vendors[x] + 'CancelRequestAnimationFrame'];
    }
    //如果仍然不兼容，则使用setTimeOut进行兼容操作
    if(!window.requestAnimationFrame) {
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16.7 - (currTime - lastTime));
            var id = window.setTimeout(function() {
            callback(currTime + timeToCall);
            }, timeToCall);
            lastTime = currTime + timeToCall;
            return id; 
        }
    }

    if(!window.cancelAnimationFrame) {
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        }
    }
})();
```

##### 变量
- 变量提升
- const/let and var
- 函数会优先提升，然后才是变量
  ``` javascript
    foo();
    var foo; // 1
    function foo() {
      console.log(1)
    }

    foo = function () {
      console.log(2)
    }
  ```

  ``` javascript
  foo (); // 3
  function foo() {
    console.log(1)
  }

  var foo = function () {
    console.log(2)
  }

  function foo () {
    console.log(3)
  }
  ```

##### 并发与并行
并发（concurrency）和并⾏（parallelism）区别
- 并发是宏观概念，我分别有任务 A 和任务 B，在⼀段时间内通过任务间的切换完成了这两个任务，这种情况就可以称之为并发。
- 并⾏是微观概念，假设 CPU 中存在两个核⼼，那么我就可以同时完成任务 A、B。同时完成多个任务的情况就可以称之为并⾏。
##### 事件机制
##### 内存泄漏问题
##### 函数执行
##### 异步
##### 函数式
##### compose
##### 协程
##### 错误捕获
##### 数据拷贝/深拷贝-浅拷贝
对象类型在赋值的过程中其实是复制了地址，从⽽会导致改变了⼀⽅其他也都被改变的情况
``` javascript
let a = {
    age: 1
}
let b = a
a.age = 2
console.log(b.age) // 2
```

浅拷贝
``` javascript
let a = {
    age: 1
}
let b = Object.assign({}, a)
a.age = 2
console.log(b.age) // 1
```

``` javascript
let a = {
    age: 1
}
let b = { ...a }
a.age = 2
console.log(b.age) // 1
```

存在的问题：

``` javascript
let a = {
    age: 1,
    jobs: {
        first: 'FE'
    }
}
let b = { ...a }
a.jobs.first = 'native'
console.log(b.jobs.first) // native
```
深拷贝
- JSON.parse(JSON.stringify(object)) 来解决
``` javascript
let a = {
    age: 1,
    jobs: {
        first: 'FE'
    }
}
let b = JSON.parse(JSON.stringify(a))
a.jobs.first = 'native'
console.log(b.jobs.first) // FE
```

    有局限性的:
    - 会忽略 undefined
    - 会忽略 symbol
    - 不能序列化函数
    - 不能解决循环引⽤的对象

    ``` javascript
    let obj = {
        a: 1,
        b: {
            c: 2,
            d: 3
        }
    }
    obj.c = obj.b
    obj.e = obj.a
    obj.b.c = obj.c
    obj.b.d = obj.b
    obj.b.e = obj.b.c
    let newObj = JSON.parse(JSON.stringify(obj))
    console.log(newObj)
    ```

    ``` javascript
    let a = {
        age: undefined,
        sex: Symbol('male'),
        jobs: function() {},
        name: 'yck'
    }
    let b = JSON.parse(JSON.stringify(a))
    console.log(b) // {name: "yck"}
    ```
- 如果你所需拷⻉的对象含有内置类型并且不包含函数，可以使⽤MessageChannel
``` javascript
function structuralClone(obj) {
    return new Promise(resolve => {
        const { port1, port2 } = new MessageChannel()
        port2.onmessage = ev => resolve(ev.data)
        port1.postMessage(obj)
    })
}
var obj = {
    a: 1,
    b: {
        c: 2
    }
}
obj.b.d = obj.b
// 注意该⽅法是异步的
// 可以处理 undefined 和循环引⽤对象
const test = async () => {
    const clone = await structuralClone(obj)
    console.log(clone)
}
test()
```

lodash版本:

``` javascript
function deepClone(obj) {
    function isObject(o) {
        return (typeof o === 'object' || typeof o === 'function') && o !== null
    }
    if (!isObject(obj)) {
        throw new Error('⾮对象')
    }
    let isArray = Array.isArray(obj)
    let newObj = isArray ? [...obj] : { ...obj }
    Reflect.ownKeys(newObj).forEach(key => {
        newObj[key] = isObject(obj[key]) ?
        deepClone(obj[key]) : obj[key]
    })
    return newObj
}

let obj = {
    a: [1, 2, 3],
    b: {
        c: 2,
        d: 3
    }
}
let newObj = deepClone(obj)
newObj.b.c = 1
console.log(obj.b.c) // 2
```
##### Symbol
##### this
``` javascript
    function Foo() {
        getName = function () {
            console.log(1);
        };
        return this;
    }
    Foo.getName = function () {
        console.log(2);
    };
    Foo.prototype.getName = function () {
        console.log(3);
    };
    var getName = function () {
        console.log(4);
    };
    function getName() {
        console.log(5);
    }
    Foo.getName();
    Foo().getName();
    getName();
    new Foo.getName();
    new Foo().getName();
    new new Foo().getName();
```
##### New
##### 事件循环
##### proxy
##### 继承
##### 函数柯里化
##### 数组去重
##### 宏任务/微任务
{% asset_image macro-micro-task-process.webp %}

这里需要注意一下， 浏览器事件也是属于 宏任务的范畴。
``` javascript
document.getElementById('btn').addEventListener('click', () => {
  Promise.resolve().then(() => console.log(1))
  console.log(2);
}, false)

document.getElementById('btn').addEventListener('click', () => {
  Promise.resolve().then(() => console.log(3))
  console.log(4);
}, false)
```

#### 浏览器
##### 输入URL 发生了什么
<a href="https://github.com/skyline75489/what-happens-when-zh_CN" target="_blank">This repository is an attempt to answer the age-old interview question "What happens when you type google.com into your browser's address box and press enter?"</a>

##### 请求阻塞
js 文件放在head中和body中的区别，body会在页面加载完成之后再进行请求，不会阻塞页面的加载。
然后当浏览器在解析到 script 标签时，会暂停构建 DOM，完成后才会从暂停的地⽅重新开始。也就是说，如果你想⾸屏渲染的越快，就越不应该在⾸屏就加载 JS ⽂件，这也是都建议将 script 标签放在body 标签底部的原因。
可以再使用 async defer 进行控制。

当然在当下，并不是说 script 标签必须放在底部，因为你可以给script 标签添加 defer 或者 async 属性。当 script 标签加上 defer 属性以后，表示该 JS ⽂件会并⾏下载，但是会放到 HTML 解析完成后顺序执⾏，所以对于这种情况你可以把 script 标签放在任意位置。对于没有任何依赖的 JS ⽂件可以加上 async 属性，表示 JS ⽂件下载和解析不会阻塞渲染。

##### 渲染阻塞
head中的css 文件会阻塞页面的渲染。


##### compose
递归:
``` javascript
function compose (...fns) {
  let len = fns.length;
  var res = null;

  return function fn (...args) {
    res = fns[len - 1].apply(null, ...args);
    if (len > 1) {
      len--;
      return fn.call(null, res);
    } else {
      return res;
    }
  }
}
```

reduce方案:
``` javascript
function compose (...fns) {
  return function fn (...args) {
    return fns.reduce((arr, cur) => {
      if (typeof arr === 'function') {
        return cur(arr(...args))
      } else {
        return cur(arr)
      }
    })
  }
}
```

promise 异步:
``` javascript
function compose (...fns) {
  let pop = fns.pop();

  return function (..args) {
    return fns.reduce((arr, cur) => {
      arr.then(function(v) => {
        cur.call(null, v)
      })
    }, Promise.resolve(pop.apply(null)));
  }
}
```

generator实现:
``` javascript
const compose = function(...steps) {
  let g = iterateSteps(steps)
  return function(...args) {
    let val = steps.pop().apply(null, args)
    // 这里是第一个值
    console.log(val)
    // 因为无法传参数 所以无所谓执行 就是空耗一个yield
    g.next()
    return steps.reverse.reduce((val, val1) => g.next(val).value, val)
  }
}
```

##### 渲染过程
##### 优化
##### chrome架构
- 多进程架构
##### 请求流程
##### 垃圾处理机制

#### 前后端交互
##### cookie
##### session
##### Localstorage
##### SeesionStorage
##### 比较
|特性|cookie|localstorage| sessionStorage | indexDB |
|---|---|---|---|---|
|数据⽣命周期|⼀般由服务器⽣成，可以设置过期时间|除⾮被清理，否则⼀直存在|⻚⾯关闭就清理|除⾮被清理，否则⼀直存在|
|数据存储⼤⼩|4K| 5M |5M| 无限|
|与服务端通信|每次都会携带在header 中，对于请求性能影响|不参与|不参与|不参与|

对于 cookie 来说，我们还需要注意安全性。

|属性|作用|
|---|---|
|value|如果⽤于保存⽤户登录态，应该将该值加密，不能使⽤明⽂的⽤户标识|
|http-only|不能通过 JS 访问 Cookie，减少 XSS 攻击|
|secure|只能在协议为 HTTPS 的请求中携带|
|same-site|规定浏览器不能在跨域请求中携带 Cookie，减少 CSRF 攻击|

##### Service Worker
Service Worker 是运⾏在浏览器背后的独⽴线程，⼀般可以⽤来实现缓存功能。使⽤ Service Worker的话，传输协议必须为HTTPS。因为 Service Worker 中涉及到请求拦截，所以必须使⽤HTTPS 协议来保障安全。
Service Worker 实现缓存功能⼀般分为三个步骤：⾸先需要先注册
Service Worker
，
然后监听到 install 事件以后就可以缓存需要的⽂件，那么在下次⽤户访问的时候就可以通过拦截请求的⽅式查询是否存在缓存，存在缓存的话就可以直接读取缓存⽂件，否则就去请求数据。以下是这个步骤的实现：

``` javascript
// index.js
if (navigator.serviceWorker) {
    navigator.serviceWorker
    register('sw.js').then(function(registration) {
        console.log('service worker 注册成功')
    }).catch(function(err) {
        console.log('servcie worker 注册失败')
    })
}
// sw.js
// 监听 `install` 事件，回调中缓存所需⽂件
self.addEventListener('install', e => {
    e.waitUntil(
        caches.open('my-cache').then(function(cache) {
            return cache.addAll(['./index.html','./index.js'])
        })
    )
})
// 拦截所有请求事件
// 如果缓存中已经有请求的数据就直接⽤缓存，否则去请求数据
self.addEventListener('fetch', e => {
    e.respondWith(caches.match(e.request).then(function(response) {
        if (response) {
            return response
        }
    console.log('fetch source')
    })
)
```

##### xss/csrf
##### 交互优化
##### 请求限制/limit
##### 请求抓包/数据修改
##### Repaint 和 Reflow
重绘和回流会在我们设置节点样式时频繁出现，同时也会很⼤程度上影响性能。
- 重绘是当节点需要更改外观⽽不会影响布局的，⽐如改变color 就叫称为重绘
- 回流是布局或者⼏何属性需要改变就称为回流。
回流必定会发⽣重绘，重绘不⼀定会引发回流。回流所需的成本⽐重绘⾼的多，改变⽗节点⾥的⼦节点很可能会导致⽗节点的⼀系列回流。
以下⼏个动作可能会导致性能问题：
- 改变 window ⼤⼩
- 改变字体
- 添加或删除样式
- ⽂字改变
- 定位或者浮动
- 盒模型

并且很多⼈不知道的是，重绘和回流其实也和 Eventloop 有关。
- 当 Eventloop 执⾏完 Microtasks 后，会判断 document 是否需要更新，因为浏览器是 60Hz 的刷新率，每 16.6ms 才会更新⼀次。
- 然后判断是否有 resize 或者 scroll 事件，有的话会去触发事件，所以 resize 和 scroll 事件也是⾄少 16ms 才会触发⼀次，并且⾃带节流功能。
- 判断是否触发了 media query
- 更新动画并且发送事件
- 判断是否有全屏操作事件
- 执⾏ requestAnimationFrame 回调
- 执⾏ IntersectionObserver 回调，该⽅法⽤于判断元素是否可⻅，可以⽤于懒加载上，但是兼容性不好
- 更新界⾯
- 以上就是⼀帧中可能会做的事情。如果在⼀帧中有空闲时间，就会去执⾏ requestIdleCallback 回调。

##### 为什么DOM 操作慢
因为 DOM 是属于渲染引擎中的东⻄，⽽ JS ⼜是 JS 引擎中的东⻄。当我们通过 JS 操作 DOM 的时候，其实这个操作涉及到了两个线程之间的通信，那么势必会带来⼀些性能上的损耗。操作 DOM 次数⼀多，也就等同于⼀直在进⾏线程之间的通信，并且操作 DOM 可能还会带来重绘回流的情况，所以也就导致了性能上的问题。

#### CSS
##### 权重计算
> 元素选择符： 1
> class选择符： 10
> id选择符：100
> 元素标签：1000

- !important声明的样式优先级最高，如果冲突再进行计算。
- 如果优先级相同，则选择最后出现的样式。
- 继承得到的样式的优先级最低。
- style > link 

##### 定位
static（默认）：按照正常文档流进行排列；
relative（相对定位）：不脱离文档流，参考自身静态位置通过 top, bottom, left, right 定位；
absolute(绝对定位)：参考距其最近一个不为static的父级元素通过top, bottom, left, right 定位；
fixed(固定定位)：所固定的参照对像是可视窗口。

##### css 三角形
首先，需要把元素的宽度、高度设为0。然后设置边框样式。
``` css
width: 0;
height: 0;
border-top: 40px solid transparent;
border-left: 40px solid transparent;
border-right: 40px solid transparent;
border-bottom: 40px solid #ff0000;
```

##### 居中
##### 清除浮动
##### BFC
BFC规定了内部的Block Box如何布局。
定位方案：

内部的Box会在垂直方向上一个接一个放置。
Box垂直方向的距离由margin决定，属于同一个BFC的两个相邻Box的margin会发生重叠。
每个元素的margin box 的左边，与包含块border box的左边相接触。
BFC的区域不会与float box重叠。
BFC是页面上的一个隔离的独立容器，容器里面的子元素不会影响到外面的元素。
计算BFC的高度时，浮动元素也会参与计算。
满足下列条件之一就可触发BFC

根元素，即html
float的值不为none（默认）
overflow的值不为visible（默认）
display的值为inline-block、table-cell、table-caption
position的值为absolute或fixed
##### Flex布局
##### px rem em vh vw
##### 0.5px的线
##### 伪元素/伪类
单冒号(:)用于CSS3伪类，双冒号(::)用于CSS3伪元素。
::before就是以一个子元素的存在，定义在元素主体内容之前的一个伪元素。并不存在于dom之中，只存在在页面之中。
:before 和 :after 这两个伪元素，是在CSS2.1里新出现的。起初，伪元素的前缀使用的是单冒号语法，但随着Web的进化，在CSS3的规范里，伪元素的语法被修改成使用双冒号，成为::before ::after
##### 图片类型
png是便携式网络图片（Portable Network Graphics）是一种无损数据压缩位图文件格式.优点是：压缩比高，色彩好。 大多数地方都可以用。
jpg是一种针对相片使用的一种失真压缩方法，是一种破坏性的压缩，在色调及颜色平滑变化做的不错。在www上，被用来储存和传输照片的格式。
gif是一种位图文件格式，以8位色重现真色彩的图像。可以实现动画效果.
webp格式是谷歌在2010年推出的图片格式，压缩率只有jpg的2/3，大小比png小了45%。缺点是压缩的时间更久了，兼容性不好，目前谷歌和opera支持。

#### 其他

##### 进程和线程
|进程|线程|
|---|---|
|进程是资源分配的最小单位 | 线程是程序执行的最小单位，CPU调度的最小单位|
|进程有自己独立的地址空间 | 线程共享进程的地址空间|
|进程之间的资源是独立的 | 线程共享本进程的资源|

##### 进程通信

|进程通信|线程通信|
|---|---|
|管道(包括管道和命名管道) 内存中类似于文件的模型，多进程可读写 |	共享内存 |
|消息队列 内核中的队列 | 管道 |
|共享内存||	 
|信号量||
|套接字 不同主机上的进程通信方式||