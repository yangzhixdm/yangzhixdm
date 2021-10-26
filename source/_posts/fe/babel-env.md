---
title: Babel-env 配置问题
date: 2021-10-26 10:49:41
tags: babel, fe-build
description: Babel-env 配置问题，主要涉及一些之前没有关注的模糊问题，或者一些零碎的问题，规整。
---
### @babel/preset-env
用于对 babel进行预设环境处理。其中可以对 需要针对的浏览器进行预先指定，已经module转换成哪一种模式，以及其他的一些配置项。

#### preset order
多个 preset 执行顺序为逆序，也就是从右往左执行。
```
{
  "presets": ["a", "b", "c"]
}
```
执行顺序为 c, b, a.
#### module
用于指定是否 需要 将 模块进行转换为 指定的 结构，比如 amd, umd, systemjs, commonjs，默认情况会将 代码转换为 commonjs, 设置为false,则不转换。
这里需要注意一点是，从webpack2开始已经内置了将 babel之后的文件进行转换成对应的格式文件，所以在babel中就无需进行设置了，所以如果babel配合webapck使用，那么在module需要设置成false.
``` json
{
  "presets": [
    [
      "@babel/preset-env",
      {
        "modules": false
      }
    ]
  ]
}
```
#### targets
targets 用于指定目标环境，可以指定node，浏览器等环境，需要注意的的是这里每个环境的数组中的条件为 `或` 逻辑。
``` json
  "targets": {
    "browsers": ["> 0.25%", "not dead"]
  }
```

#### useBuiltIns
如果使用了 useBuiltIns 属性，那么在babel7中会使用内置的 @babel/polyfill 对特定的内置特性进行polyfill。
useBuiltIns可以配置三种类型： 'enrty', 'usage', false。
entry 需要在入口进行应用，usage 按需进行引用，false则不使用，需要自己手动在文件头部自己引入 @babal/polyfill。

#### corejs
如果使用了 useBuiltIns, 那么@babel/preset-env 直接指定 `core-js` 模块为一个`基础模块引用`。也就代表`core-js` 将会被解析。
由于 `@babel/polyfill` 在7.4已经 deprecated,所以需要手动引入core-js。指定了哪个版本号，就引入哪个版本的core-js。
``` bash
npm install core-js@3 --save

# or

npm install core-js@2 --save
```
配置corejs为3，需要预先安装@babel/runtime-corejs3
配置corejs为2，需要预先安装@babel/runtime-corejs2
配置corejs为false，需要预先安装@babel/runtime

### @babel/plugin-transform-runtime
@babel/plugin-transform-runtime插件用于动态的将 es6代码转换成 es5代码。一般需要设置属性 'corejs: 2'.
但是目前`corejs:2 ` 对于 一些 `实例方法` 无法进行转换，而 最新的版本 'corejs: 3'已经可以是实现对 `实例方法` 的转换，但打包出来的文件相对较大。
如果使用 `corejs:2` 版本，那么就需要配置上面 的 @babel/preset-env 中的 `useBuiltIns` 使用，对于无法转换的 `实例方法` 就采用polyfill的实行进行处理。

#### @babel/plugin-transform-runtime中的corejs作用是什么
设置了 corejs：2，是指定了 `babel/runtime-corejs`的版本，那么指定了 2 的版本，就需要引入一个新的 npm包 @babel/runtime-corejs2 ，并且需要设置为本地依赖，不能是开发依赖。
``` bash
npm i @babel/runtime-corejs2 --save
```
`@babel/runtime-corejs2`包含三个文件夹：core-js（引用core-js这个npm包）、helpers（定义了一些处理新的语法关键字的帮助函数）、regenerator（仅仅是引用regenerator-runtime这个npm包）。

### plugins

#### plugins 执行顺序
> 1 plugins中的插件在presets中的插件先执行
2 plugins中的插件执行顺序为从左往右
3 presets中的插件执行顺序为从右往左

### 其他细节
#### 名称本地化
babel为了避免重复书写，对于名称会有一定的简写方式
1 绝对路径不处理
2 以`./`开头的相对路径不进行处理
3 引用package中的文件不进行处理
4 任何以`module:`作为前缀的名称将会移除 `module:`
5 以 @babel作用域的并且没有包含 `plugin-`或者`preset-`前缀，将会给 @babel作用域下的包名注入 `plugin-`或者`preset-`字符
6 如果一个包名没有在任何作用域下，并且没有包含`babel-plugin-`/`babel-preset-`前缀，那么就给把`babel-plugin-`/`babel-preset-`作为这个包的前缀
7 `babel-plugin-`/`babel-preset-`会被作为一个前缀注入到这个包名下，如果一个包名为 @-scoped，并且没有包含`babel-plugin-`/`babel-preset-`字符串。
8 `babel-plugin`/babel-preset`会被注入为一个包的名字，如果这个包名为 @-scoped

举例：

|Input|Normalized|
|----|-----|
|"/dir/plugin.js"	|"/dir/plugin.js"|
|"./dir/plugin.js"|"./dir/plugin.js"|
|"mod"|"babel-plugin-mod"|
|"mod/plugin"|"mod/plugin"|
|"babel-plugin-mod"|"babel-plugin-mod"|
|"@babel/mod"|"@babel/plugin-mod"|
|"@babel/plugin-mod"|"@babel/plugin-mod"|
|"@babel/mod/plugin"|"@babel/mod/plugin"|
|"@scope"|"@scope/babel-plugin"|
|"@scope/babel-plugin"|"@scope/babel-plugin"|
|"@scope/mod"|"@scope/babel-plugin-mod"|
|"@scope/babel-plugin-mod"|"@scope/babel-plugin-mod"|
|"@scope/prefix-babel-plugin-mod"|"@scope/prefix-babel-plugin-mod"|
|"@scope/mod/plugin"|"@scope/mod/plugin"|
|"module:foo"|"foo"|

### Demo
``` javascript
const presets = [
  ['@babel/env', {
    // chrome, opera, edge, firefox, safari, ie, ios, android, node, electron
    // targets 和 browerslist 合并取最低版本
    targets: {
      ie: 8,

      // 是否使用 esmodules
      esmodules: true,
    },

    // 启用更符合规范的转换，但速度会更慢，默认为 `false`，从目前来看，是更严格的转化，包括一些代码检查。
    spec: false,

    // 有两种模式：normal, loose。其中 normal 更接近 es6 loose 更接近 es5
    loose: false,

    // "amd" | "umd" | "systemjs" | "commonjs" | "cjs" | false, defaults to "commonjs"
    modules: false,

    // 打印插件使用情况
    debug: true,

    // 按需增加移除一些功能
    // include: [],
    // exclude: [],

    // 增加 polyfills
    // 按需使用
    // useBuiltIns: 'usage',
    // 引用一次
    // useBuiltIns: 'entry',
    // 不引用，独自使用
    // useBuiltIns: false,

    // 强制使用所有的插件
    // forceAllTransforms: false,

    // 配置 browerslist 的位置
    // configPath: ,
    // 配置是否忽略 browerslist 文件
    // ignoreBrowserslistConfig: false,

    // 是否跳过 proposal 的文件
    // shippedProposals: false,
  }]
];

const plugins = [
  [
    // 重用把 babel 注入的帮助代码， 依赖 @babel/runtime
    "@babel/plugin-transform-runtime",
    {
      // false || 2, 变成全局或者局部
      "corejs": false,
      
      // 生成器运行时的使用，变成全局或者局部
      "regenerator": true,

      // 帮助函数是变成 inline, 还是  lib
      "helpers": true,

      // helpers 切换成 esm
      "useESModules": true
    }
  ]
];

module.exports = { presets, plugins };
```
测试代码：
``` javascript
const a = 'hello'.includes('xx');
const b = ['hello'].includes('xx');
console.log(a, b);

const p = new Promise((resolve) => {
  resolve();
});

p.then(() => {
  console.log('then');
});

class Person {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  };
  getX() {
    return this.x;
  }
}

let person = new Person(25, 8);
console.log(person);

export default Person;
```
当 `@preset/env`中 modules设置为默认值或者不设置时。
``` javascript
"use strict";

var _Object$defineProperty = require("@babel/runtime-corejs2/core-js/object/define-property");

var _interopRequireDefault = require("@babel/runtime-corejs2/helpers/interopRequireDefault");

_Object$defineProperty(exports, "__esModule", {
  value: true
});

exports.default = void 0;

var _classCallCheck2 = _interopRequireDefault(require("@babel/runtime-corejs2/helpers/classCallCheck"));

var _createClass2 = _interopRequireDefault(require("@babel/runtime-corejs2/helpers/createClass"));

var _promise = _interopRequireDefault(require("@babel/runtime-corejs2/core-js/promise"));

require("core-js/modules/es6.string.includes.js");

require("core-js/modules/es7.array.includes.js");

var a = 'hello'.includes('xx');
var b = ['hello'].includes('xx');
console.log(a, b);
var p = new _promise.default(function (resolve) {
  resolve();
});
p.then(function () {
  console.log('then');
});

var Person = /*#__PURE__*/function () {
  function Person(x, y) {
    (0, _classCallCheck2.default)(this, Person);
    this.x = x;
    this.y = y;
  }

  (0, _createClass2.default)(Person, [{
    key: "getX",
    value: function getX() {
      return this.x;
    }
  }]);
  return Person;
}();

var person = new Person(25, 8);
console.log(person);
var _default = Person;
exports.default = _default;
```

将module的值设置为 false 时:
``` javascript
import _classCallCheck from "@babel/runtime-corejs2/helpers/classCallCheck";
import _createClass from "@babel/runtime-corejs2/helpers/createClass";
import _Promise from "@babel/runtime-corejs2/core-js/promise";
import "core-js/modules/es6.string.includes.js";
import "core-js/modules/es7.array.includes.js";
var a = 'hello'.includes('xx');
var b = ['hello'].includes('xx');
console.log(a, b);
var p = new _Promise(function (resolve) {
  resolve();
});
p.then(function () {
  console.log('then');
});

var Person = /*#__PURE__*/function () {
  function Person(x, y) {
    _classCallCheck(this, Person);

    this.x = x;
    this.y = y;
  }

  _createClass(Person, [{
    key: "getX",
    value: function getX() {
      return this.x;
    }
  }]);

  return Person;
}();

var person = new Person(25, 8);
console.log(person);
export default Person;
```