---
title: Webpack externals 以及 libraryTarget 作用
date: 2021-10-21 11:51:44
tags:
description: 关于 webpack的一些配置的了解与熟悉。
---
### 参考文章
[babel-polyfill](https://babel.docschina.org/docs/en/6.26.3/babel-polyfill/)
[风动之石博客](https://blog.windstone.cc/es6/babel/@babel/plugin-transform-runtime.html#core-js-%E6%9B%BF%E6%8D%A2)
[@babel/plugin-transform-runtime 到底是什么](https://zhuanlan.zhihu.com/p/147083132)
[babel-polyfill 关系](https://blog.csdn.net/hbiao68/article/details/104100731)
[Babel转码-corejs3](https://segmentfault.com/a/1190000020237817)
[Babel转码-polyfill还是transform-time](https://segmentfault.com/a/1190000020237790)

### Webpack externals
防止将某些 import 的包(package)打包到 bundle 中，而是在运行时(runtime)再去从外部获取这些扩展依赖(external dependencies)。
具有外部依赖(external dependency)的 bundle 可以在各种模块上下文(module context)中使用，例如 CommonJS, AMD, 全局变量和 ES2015 模块。外部 library 可能是以下任何一种形式：

root：可以通过一个全局变量访问 library（例如，通过 script 标签）。
commonjs：可以将 library 作为一个 CommonJS 模块访问。
commonjs2：和上面的类似，但导出的是 module.exports.default.
amd：类似于 commonjs，但使用 AMD 模块系统。

#### 支持类型
externals这个参数的传入形式有多种，但是总结而言，实际上就是array > object，reg，这三种形式都可以传入，前者其实是对后者的包含。

##### 字符串
如果只是提供一个字符串的话，那么会被认为是 root 类型。（`when a string is provided it will be treated as root`）
如果希望将一个类库指定为其他类型，可以添加一个类型在类库的前面；
``` json
{
  module.exports = {
    // ...
    externals: {
      'fs-extra': 'commonjs2 fs-extra',
    },
  };
}
```
那么将会被 fs-extra 为
``` javascript
const fs = require('fs-extra');
```

##### 字符串数组
``` json
module.exports = {
  //...
  externals: {
    subtract: ['./math', 'subtract'],
  },
};
```
subtract: ['./math', 'subtract'] 转换为父子结构，其中 ./math 是父模块，而 bundle 只引用 subtract 变量下的子集。该例子会编译成 require('./math').subtract;

##### 1.1.2 Object
``` json
module.exports = {
  //...
  externals: {
    react: 'react',
  },

  // or

  externals: {
    lodash: {
      commonjs: 'lodash',
      amd: 'lodash',
      root: '_', // indicates global variable
    },
  },

  // or

  externals: {
    subtract: {
      root: ['math', 'subtract'],
    },
  },
};
```
##### function
```
function ({ context, request, contextInfo, getResolve }, callback)
function ({ context, request, contextInfo, getResolve }) => promise # 5.15.0+
```
对于 webpack 外部化，通过定义函数来控制行为，可能会很有帮助。例如，webpack-node-externals 能够排除 node_modules 目录中所有模块，还提供一些选项，比如白名单 package(whitelist package)。

##### RegExp
##### Combining syntaxes
``` json
module.exports = {
  //...
  externals: [
    {
      // String
      react: 'react',
      // Object
      lodash: {
        commonjs: 'lodash',
        amd: 'lodash',
        root: '_', // indicates global variable
      },
      // [string]
      subtract: ['./math', 'subtract'],
    },
    // Function
    function ({ context, request }, callback) {
      if (/^yourregex$/.test(request)) {
        return callback(null, 'commonjs ' + request);
      }
      callback();
    },
    // Regex
    /^(jquery|\$)$/i,
  ],
};
```
##### byLayer
``` json
module.exports = {
  externals: {
    byLayer: {
      layer: {
        external1: 'var 43',
      },
    },
  },
};
```

##### externalsType
```
string = 'var'
```
Specify the default type of externals. amd, umd, system and jsonp externals depend on the output.libraryTarget being set to the same value e.g. you can only consume amd externals within an amd library.

##### externalsType.module
Specify the default type of externals as 'module'. Webpack will generate code like import * as X from '...' for externals used in a module.

Make sure to enable experiments.outputModule first, otherwise webpack will throw errors.
``` json
module.exports = {
  experiments: {
    outputModule: true,
  },
  externalsType: 'module',
};
```
##### externalsType.node-commonjs
Specify the default type of externals as 'node-commonjs'. Webpack will import createRequire from 'module' to construct a require function for loading externals used in a module.

``` json
module.export = {
  experiments: {
    outputModule: true,
  },
  externalsType: 'node-commonjs',
};
```

``` json
import { createRequire } from 'module';
const require = createRequire(import.meta.url);

// sibling-module.js is a CommonJS module.
const siblingModule = require('./sibling-module');
```
##### externalsType.script
`Specify the default type of externals as 'script'. Webpack will Load the external as a script exposing predefined global variables with HTML <script> element. The <script> tag would be removed after the script has been loaded.`
``` json
module.exports = {
  externalsType: 'script',
  externals: {
    packageName: [
      'http://example.com/script.js',
      'global',
      'property',
      'property',
    ], // properties are optional
  },
};
```
You can also use the shortcut syntax if you're not going to specify any properties:

```
module.exports = {
  externalsType: 'script',
  externals: {
    packageName: 'global@http://example.com/script.js', // no properties here
  },
};
```
请注意，output.publicPath 不会被添加到提供的 URL 中。



### 关于 output 
#### library
Output a library exposing the exports of your entry point.
``` json
module.exports = {
  // …
  entry: './src/index.js',
  output: {
    library: 'MyLibrary',
  },
};
```
导出一个类库，并指定一个名字。大致可以理解为 导出对象的时候，给你的对象赋值了一个名字，导出的文件类似
```
const MyLibrary = {
  // some code....
}
```

#### libraryTarget
指定以什么类型来暴露变量。
##### 暴露为一个变量 var
``` json
libraryTarget: 'var'
```
导出的内容
``` javascript
var MyLibrary = _entry_return_;

// In a separate script...
MyLibrary.doSomething();
```

##### libraryTarget: 'assign'
注入到已经存在的变量中，如果这个变量不存在，那么就会暴露在全局作用于中。
``` javascript
MyLibrary = _entry_return_;
```

##### libraryTarget: 'assign-properties'
``` javascript
// create the target object if it doesn't exist
MyLibrary = typeof MyLibrary === 'undefined' ? {} : MyLibrary;
// then copy the return value to MyLibrary
// similarly to what Object.assign does

// for instance, you export a `hello` function in your entry as follow
export function hello(name) {
  console.log(`Hello ${name}`);
}

// In another script running MyLibrary
// you can run `hello` function like so
MyLibrary.hello('World');
```
tip: 当使用此选项时，将 output.library 设置为空，将产生一个破损的输出 bundle。

##### 通过在对象上赋值暴露
如果 output.library 未赋值为一个非空字符串，则默认行为是，将入口起点返回的所有属性都赋值给一个对象（此对象由 output.libraryTarget 特定），通过如下代码片段：
``` javascript
(function (e, a) {
  for (var i in a) {
    e[i] = a[i];
  }
})(output.libraryTarget, _entry_return_);
```

##### libraryTarget: 'this'
``` javascript
this['MyLibrary'] = _entry_return_;

// 在一个单独的 script...
this.MyLibrary.doSomething();
MyLibrary.doSomething(); // 如果 this 是 window
```

##### libraryTarget: 'window'
``` javascript
window['MyLibrary'] = _entry_return_;

window.MyLibrary.doSomething();
```

##### libraryTarget: 'global'
``` javascript
global['MyLibrary'] = _entry_return_;

global.MyLibrary.doSomething();

```

##### libraryTarget: 'commonjs'
``` javascript
exports['MyLibrary'] = _entry_return_;

require('MyLibrary').doSomething();

```

##### libraryTarget: 'module'
输出 ES 模块。请确保事先启用 experiments.outputModule。

##### libraryTarget: 'commonjs2'
``` javascript
module.exports = _entry_return_;

require('MyLibrary').doSomething();
// webpack5 注意，output.library 不能与 output.libraryTarget 一起使用。
```

##### libraryTarget: 'amd'
``` javascript
define('MyLibrary', [], function () {
  return _entry_return_;
});
```
如果 library 没有定义则会生成如下方式：
``` javascript
define([], function () {
  return _entry_return_;
});
```

##### libraryTarget: 'amd-require'
这将使用立即执行的 AMD require(dependencies, factory) 包装器包装您的输出。
'amd-require' 目标（target）允许使用 AMD 依赖项，而无需单独的后续调用。与 'amd' 目标（target）一样, 这取决于在加载 webpack 输出的环境中适当可用的 require function。
对于此 target，库名称将被忽略。

##### libraryTarget: 'umd'
将你的 library 暴露为所有的模块定义下都可运行的方式。它将在 CommonJS, AMD 环境下运行，或将模块导出到 global 下的变量。

``` javascript
(function webpackUniversalModuleDefinition(root, factory) {
  if (typeof exports === 'object' && typeof module === 'object')
    module.exports = factory();
  else if (typeof define === 'function' && define.amd) define([], factory);
  else if (typeof exports === 'object') exports['MyLibrary'] = factory();
  else root['MyLibrary'] = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  return _entry_return_;
});
```
如果没有传递 library , 会导致将入口起点返回的所有属性，直接赋值给 root 对象
``` javascript
(function webpackUniversalModuleDefinition(root, factory) {
  if (typeof exports === 'object' && typeof module === 'object')
    module.exports = factory();
  else if (typeof define === 'function' && define.amd) define([], factory);
  else if (typeof exports === 'object') exports['MyLibrary'] = factory();
  else root['MyLibrary'] = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  return _entry_return_;
});
```

从 webpack 3.1.0 开始，你可以将 library 指定为一个对象，用于给每个 target 起不同的名称：

##### libraryTarget: 'system'
``` javascript
System.register([], function (_export) {
  return {
    setters: [],
    execute: function () {
      // ...
    },
  };
});
```

##### libraryTarget: 'jsonp'
This will wrap the return value of your entry point into a jsonp wrapper.

``` javascript
MyLibrary(_entry_return_);
```
### 3 Webpack webpack-node-externals
externals中可以支持function的配置方式，该插件主要用于 移除 对 node_module中的模块。如果没有使用该插件，在第三方的打包过程中，会将node_module中引用的包也打进来，这样就会导致自身类库过于庞大。

``` js
var nodeExternals = require('webpack-node-externals');

module.exports = {
  externals: [
    {
      vue: config.vue
    },
    nodeExternals()
  ],
}
```

### 其他的一些问题
#### externals
某些情况，可能需要的一些包，不希望打包进自身的类库中，希望使用者自己去安装，那么就可以使用 externals 将其划分出去。

#### babel-polyfill
babel-polyfill，通过改写全局prototype的方式实现，它会加载整个polyfill，针对编译的代码中新的API进行处理，并且在代码中插入一些帮助函数，比较适合单独运行的项目。

babel-polyfill解决了Babel不转换新API的问题，但是直接在代码中插入帮助函数，会导致污染了全局环境，并且不同的代码文件中包含重复的代码，导致编译后的代码体积变大。虽然这对于应用程序或命令行工具来说可能是好事，但如果你的代码打算发布为供其他人使用的库，或你无法完全控制代码运行的环境，则会成为问题。

#### babel-runtime
Babel为了解决上述问题，提供了单独的包babel-runtime用以提供编译模块的工具函数，启用插件babel-plugin-transform-runtime后，Babel就会使用babel-runtime下的工具函数。

#### babel-plugin-transform-runtime

为了方便使用 babel-runtime，解决手动 require 的苦恼。它会分析我们的 ast 中，是否有引用 babel-rumtime 中的垫片（通过映射关系），如果有，就会在当前模块顶部插入我们需要的垫片。
在大多数情况下，你应该安装 babel-plugin-transform-runtime 作为开发依赖（使用 --save-dev），并且将 babel-runtime 作为生产依赖（使用 --save）。这个看vue-cli生成的 package.json就能发现。

因为babel编译es6到es5的过程中，babel-plugin-transform-runtime这个插件会自动polyfill es5不支持的特性，这些polyfill包就是在babel-runtime这个包里（core-js 、regenerator等）

#### useBuiltIns
``` json
{
  "presets": [
    [
      "@babel/preset-env",
      {
        "useBuiltIns": "usage"
      }
    ]
  ]
}
```
将 useBuiltIns 改为 "usage"，babel 就可以按需加载 polyfill，并且不需要手动引入 @babel/polyfill。会自动按需加载需要的babel-pollfill。并不需要再在entry中进行引入。

``` bash
yarn add @babel/plugin-transform-runtime -D
yarn add @babel/runtime-corejs2
```


@babel/plugin-transform-runtime的作用就是，将这些辅助工具函数转换成引入@babel/runtime模块的形式，进而消除掉各个文件都引入同一辅助工具函数导致的重复

使用 @babel/plugin-transform-runtime 的原因
若你不想污染全局内置对象
若你是开发工具库
提取出helps作为共用模块，进而缩小最终编译输出的体积

``` json
{
  "presets": [
    [
      "@babel/preset-env",
      {
        "useBuiltIns": "usage",
        "debug": true
      }
    ]
  ],
  "plugins": [
    [
      "@babel/plugin-transform-runtime",
      {
        "corejs": 2 // 指定 runtime-corejs 的版本，目前有 2 3 两个版本
      }
    ]
  ]
}
```
这里的corejs与presets里设置的corejs是不同的，这里的corejs指定了一个runtime-corejs的版本，因此使用时也需要通过 NPM 安装对应的包。
``` bash
npm i @babel/runtime-corejs2 -S
```

#### 几个包之间的关系
##### babel-polyfill
仅仅是引用core-js、regenerator-runtime这两个npm包。

##### @babel/runtime
包含两个文件夹：helpers（定义了一些处理新的语法关键字的帮助函数）、regenerator（仅仅是引用regenerator-runtime这个npm包）。
##### @babel/runtime-corejs2
包含三个文件夹：core-js（引用core-js这个npm包）、helpers（定义了一些处理新的语法关键字的帮助函数）、regenerator（仅仅是引用regenerator-runtime这个npm包）。
可以看出，@babel/runtime-corejs2≈@babel/runtime + babel-polyfill：
##### @babel/runtime
只能处理语法关键字，而@babel/runtime-corejs2还能处理新的全局变量（例如，Promise）、新的原生方法（例如，String.padStart);
使用了@babel/runtime-corejs2，就无需再使用@babel/runtime了。