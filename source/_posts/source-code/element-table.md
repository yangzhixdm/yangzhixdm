---
title: 探究 ElementUI 中 el-table的实现方案
date: 2021-10-26 17:53:26
tags: source-code, el-table
description: 探究 ElementUI 中 el-table的实现方案。
---

### How to use
从elementUI文档上面copy一个基本的使用例子：
``` html
<template>
  <el-table
    :data="tableData"
    style="width: 100%">
    <el-table-column
      prop="date"
      label="日期"
      width="180">
    </el-table-column>
    <el-table-column
      prop="name"
      label="姓名"
      width="180">
    </el-table-column>
    <el-table-column
      prop="address"
      label="地址">
    </el-table-column>
  </el-table>
</template>

<script>
  export default {
    data() {
      return {
        tableData: [{
          date: '2016-05-02',
          name: '王小虎',
          address: '上海市普陀区金沙江路 1518 弄'
        }, {
          date: '2016-05-04',
          name: '王小虎',
          address: '上海市普陀区金沙江路 1517 弄'
        }, {
          date: '2016-05-01',
          name: '王小虎',
          address: '上海市普陀区金沙江路 1519 弄'
        }, {
          date: '2016-05-03',
          name: '王小虎',
          address: '上海市普陀区金沙江路 1516 弄'
        }]
      }
    }
  }
</script>
```
从使用上来看，el-table 包含了两个基本的组件，`el-table` 和`el-table-column`,其中 `el-table-column`作为子组件传递给 `el-table`。

### el-table
el-table 通过 install插件的方式进行安装到 vue 组件中：

#### 模版
``` javascript
import ElTable from './src/table';

ElTable.install = function(Vue) {
  Vue.component(ElTable.name, ElTable);
};

export default ElTable;
```

这里直接引入了`./src/table`文件。移除了部分细节代码，看一下主体结构如下：
``` html
<template>
  <div class="el-table">
    <div class="hidden-columns" ref="hiddenColumns"><slot></slot></div>
    <div v-if="showHeader"ref="headerWrapper">
      <table-header ref="tableHeader"></table-header>
    </div>
    <div ref="bodyWrapper">
      <table-body
        :store="store"
        :stripe="stripe"
      >
      </table-body>
      <div
        v-if="!data || data.length === 0"
        ref="emptyBlock">
        <span class="el-table__empty-text" >
          <slot name="empty">{{ emptyText || t('el.table.emptyText') }}</slot>
        </span>
      </div>
      <div
        v-if="$slots.append"
        ref="appendWrapper">
        <slot name="append"></slot>
      </div>
    </div>
    <div
      v-if="showSummary"
      v-show="data && data.length > 0"
      ref="footerWrapper">
      <table-footer
        :store="store"
      >
      </table-footer>
    </div>
    <div
      v-if="fixedColumns.length > 0"
      ref="fixedWrapper">
      <div
        v-if="showHeader"
        ref="fixedHeaderWrapper" >
        <table-header
          ref="fixedTableHeader"
          :store="store"
        ></table-header>
      </div>
      <div
        ref="fixedBodyWrapper"
      >
        <table-body
          fixed="left"
          :store="store"
        >
        </table-body>
        <div
          v-if="$slots.append"
        ></div>
      </div>
      <div
        v-if="showSummary"
        v-show="data && data.length > 0"
        ref="fixedFooterWrapper"
      >
        <table-footer
          fixed="left"
          :store="store"
        ></table-footer>
      </div>
    </div>
    <div
      v-if="rightFixedColumns.length > 0"
      ref="rightFixedWrapper"
    >
      <div v-if="showHeader"
        ref="rightFixedHeaderWrapper">
        <table-header
          ref="rightFixedTableHeader"
          fixed="right"
          :store="store"
        ></table-header>
      </div>
      <div
        ref="rightFixedBodyWrapper"
      >
        <table-body
          fixed="right"
          :store="store"
        >
        </table-body>
         <div
          v-if="$slots.append"
        ></div>
      </div>
      <div
        v-if="showSummary"
        v-show="data && data.length > 0"
        ref="rightFixedFooterWrapper">
        <table-footer
          fixed="right"
          :store="store"
        ></table-footer>
      </div>
    </div>
    <div
      v-if="rightFixedColumns.length > 0"
      ref="rightFixedPatch"
    ></div>
    <div class="el-table__column-resize-proxy" ref="resizeProxy" v-show="resizeProxyVisible"></div>
  </div>
</template>
```
组件分析：
>1 最重要的部分，`el-table`中首选给出了一个隐藏的插槽slot， 用来渲染 `el-table-colmun`组件。
2 涉及的组件 `table-header`, `table-body`, `table-footer`
3 数据渲染部分包括三个，左右fixed组件，以及数据展示组件。
4 列表`table-header`根绝show-header进行判定

#### 加载过程
el-table在加载过程中，主动加载一个 directives `Mousewheel`,用于实现鼠标滚动事件。并且 添加了 两个mixins 用于实现国际化Locale，以 Migrating。

Migrating的作用：开发阶段生效，用于在组件mounted时判定 getMigratingConfig 方法返回的属性是否在当前节点`this.$vnode`中已经存在该属性，如果已经存在则提示警告信息。用于提示某些功能的迁移与升级。
##### Migrating 组件
``` javascript
mounted() {
  if (process.env.NODE_ENV === 'production') return;
  if (!this.$vnode) return;
  const { props = {}, events = {} } = this.getMigratingConfig();
  const { data, componentOptions } = this.$vnode;
  const definedProps = data.attrs || {};
  const definedEvents = componentOptions.listeners || {};

  for (let propName in definedProps) {
    propName = kebabCase(propName); // compatible with camel case
    if (props[propName]) {
      console.warn(`[Element Migrating][${this.$options.name}][Attribute]: ${props[propName]}`);
    }
  }

  for (let eventName in definedEvents) {
    eventName = kebabCase(eventName); // compatible with camel case
    if (events[eventName]) {
      console.warn(`[Element Migrating][${this.$options.name}][Event]: ${events[eventName]}`);
    }
  }
},
methods: {
  getMigratingConfig() {
    return {
      props: {},
      events: {}
    };
  }
}
```


#### data()
在 数据初始化过程中，会进行创建数据存储对象store, 并且创建  tableLayout对象。
需要注意的是这里的`store`并没有纳入到响应式管理，而 layout是被纳入到响应式中。

``` javascript
data() {
  this.store = createStore();
  const layout = new TableLayout({
    store: this.store,
    table: this,
    fit: this.fit,
    showHeader: this.showHeader
  });
  return {
    layout,
    ...
  };
}
```
####  mapStates
el-table会从 `store` 中拉取需要的数据，之后将数据传递给 `table-body` 进行渲染。
这里el-table将数据放在computed中，并关联到 store 中，这样如果后续 store 中的数据有变化，那么el-table中的数据也会同时触发 响应式。

``` javascript
...mapStates({
  selection: 'selection',
  columns: 'columns',
  tableData: 'data', // 
  fixedColumns: 'fixedColumns',
  rightFixedColumns: 'rightFixedColumns'
})
```
看一下 `mapStates` 方法:
``` javascript
export function mapStates(mapper) {
  const res = {};
  Object.keys(mapper).forEach(key => {
    const value = mapper[key];
    let fn;
    if (typeof value === 'string') {
      fn = function() {
        return this.store.states[value];
      };
    } else if (typeof value === 'function') {
      fn = function() {
        return value.call(this, this.store.states);
      };
    } else {
      console.error('invalid value type');
    }
    if (fn) {
      res[key] = fn;
    }
  });
  return res;
};
```
说白了，就是把 根据 store中的属性，生成一个对应 属性，然后展开 到当前的对象中。然后赋值到computed中，这样就实现了，el-table中的`computed`属性和 `store` 中的属性的响应式链接。 
比如上面就 会 将 store中的 `data` 属性 绑定到 el-table 中computed的 `tableData` 属性。

#### watched
``` javascript
data: {
  immediate: true,
  handler(value) {
    this.store.commit('setData', value);
  }
},
```
如果 data 产生变化，那么就会开发触发 store 的 commit 方法，重新设置 setData，更新数据，然后重新渲染数据列表。这里打data就是 用户通过 el-table中的属性data传递进来的渲染数据列表。

##### created
el-table在created时，设置 tableId,并设定`debouncedUpdateLayout` 更新布局方法.
``` javascript
created() {
  this.tableId = 'el-table_' + tableIdSeed++;
  this.debouncedUpdateLayout = debounce(50, () => this.doLayout());
},
```
#### 初始化 el-tablel-colmun
加载 <a href="../element-table-column" target="_blank">el-table-colmun</a>, 加载完成之后，进入mounted生命周期。

##### mounted
mounted 生命周期，绑定对应的events，并更新updateColumns 数据，然后执行 layout方法。并执行filter过滤。
``` javascript
mounted() {
  this.bindEvents();
  this.store.updateColumns();
  this.doLayout();

  this.resizeState = {
    width: this.$el.offsetWidth,
    height: this.$el.offsetHeight
  };

  // init filters
  this.store.states.columns.forEach(column => {
    if (column.filteredValue && column.filteredValue.length) {
      this.store.commit('filterChange', {
        column,
        values: column.filteredValue,
        silent: true
      });
    }
  });

  this.$ready = true;
}
```
bindEvents ，给当前的body节点绑定一个scroll事件，在左右滚动的时候修改 `scrollPosition` 的值，`right`, `left`, `middle`.用于表示当前横向滚动栏的状态。
``` javascript
bindEvents() {
  this.bodyWrapper.addEventListener('scroll', this.syncPostion, { passive: true });
  if (this.fit) {
    addResizeListener(this.$el, this.resizeListener);
  }
},
```

其实 doLayout就是 重新计算一下 每一列的宽度，重新布局。
``` javascript
doLayout() {
  if (this.shouldUpdateHeight) {
    this.layout.updateElsHeight();
  }
  this.layout.updateColumnsWidth();
},
```