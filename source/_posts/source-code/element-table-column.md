---
title: 探究 ElementUI 组件el-table-colmun的实现方案。
date: 2021-10-26 20:25:42
tags: source-code, el-table
description: 探究 ElementUI 中 el-table-colmun的实现方案。
---

关于 el-table-colmun 的实现方案。

``` javascript
import ElTableColumn from '../table/src/table-column';

/* istanbul ignore next */
ElTableColumn.install = function(Vue) {
  Vue.component(ElTableColumn.name, ElTableColumn);
};

export default ElTableColumn;

```

### 组件
组件通过 props接受 column 配置信息，包括常用的 type, prop, label, formatter等属性等。

`data()` 方法返回返回 `columns` 列表和 `isSubColumn` 字段。 `columns` 其实主要包含当前列的一些属性，包括现实方方式，是否需要过滤，排序等。
``` javascript
  data() {
    return {
      isSubColumn: false,
      columns: []
    };
  },
```

### beforeCreate
组件创建之前会对 `el-table-column` 进行重置。
``` javascript
beforeCreate() {
  this.row = {};
  this.column = {};
  this.$index = 0;
  this.columnId = '';
},
```

### created

先设置 defaults 列表默认属性，然后将 `basicProps`, `sortProps`, `selectProps`,`filterProps` 四个数组属性key 合并成同一个对象，并从 `this` （当前组件）中获取对应的属性进行赋值。
然后将 获取到的对象和 默认的 defaults 进行合并，得到 column 对象。
接下来执行 `compose` 方法 从右往左，一次给  column 对象设置一些属性。包括 render方法， column宽度，以及 forceProps.
然后注册watcher.

``` javascript
created() {
  const parent = this.columnOrTableParent;
  this.isSubColumn = this.owner !== parent;
  this.columnId = (parent.tableId || parent.columnId) + '_column_' + columnIdSeed++;

  const type = this.type || 'default';
  const sortable = this.sortable === '' ? true : this.sortable;
  const defaults = {
    ...cellStarts[type],
    id: this.columnId,
    type: type,
    property: this.prop || this.property,
    align: this.realAlign,
    headerAlign: this.realHeaderAlign,
    showOverflowTooltip: this.showOverflowTooltip || this.showTooltipWhenOverflow,
    // filter 相关属性
    filterable: this.filters || this.filterMethod,
    filteredValue: [],
    filterPlacement: '',
    isColumnGroup: false,
    filterOpened: false,
    // sort 相关属性
    sortable: sortable,
    // index 列
    index: this.index
  };

  const basicProps = ['columnKey', 'label', 'className', 'labelClassName', 'type', 'renderHeader', 'formatter', 'fixed', 'resizable'];
  const sortProps = ['sortMethod', 'sortBy', 'sortOrders'];
  const selectProps = ['selectable', 'reserveSelection'];
  const filterProps = ['filterMethod', 'filters', 'filterMultiple', 'filterOpened', 'filteredValue', 'filterPlacement'];

  let column = this.getPropsData(basicProps, sortProps, selectProps, filterProps);
  column = mergeOptions(defaults, column);

  // 注意 compose 中函数执行的顺序是从右到左
  const chains = compose(this.setColumnRenders, this.setColumnWidth, this.setColumnForcedProps);
  column = chains(column);

  this.columnConfig = column;

  // 注册 watcher
  this.registerNormalWatchers();
  this.registerComplexWatchers();
}
```

这里很重要的一条，会在 `setColumnRenders` 中设置 column 当前列 的 渲染方式, 指定其 `renderCell` 方法。
如果存在 `slots` 就使用 slots 的render 进行渲染，如果没有，则直接使用默认的 `originRenderCell` 进行渲染。
``` javascript
  originRenderCell = originRenderCell || defaultRenderCell;
  // 对 renderCell 进行包装
  column.renderCell = (h, data) => {
    let children = null;
    if (this.$scopedSlots.default) {
      children = this.$scopedSlots.default(data);
    } else {
      children = originRenderCell(h, data);
    }
    const prefix = treeCellPrefix(h, data);
    const props = {
      class: 'cell',
      style: {}
    };
    if (column.showOverflowTooltip) {
      props.class += ' el-tooltip';
      props.style = {width: (data.column.realWidth || data.column.width) - 1 + 'px'};
    }
    return (<div { ...props }>
      { prefix }
      { children }
    </div>);
  };
```
这里的 `defaultRenderCell` 方法就是获取了一下属性，然后根据判断是否需要调用 `formatter`。
``` javascript
export function defaultRenderCell(h, { row, column, $index }) {
  const property = column.property;
  const value = property && getPropByPath(row, property).v;
  if (column && column.formatter) {
    return column.formatter(row, column, value, $index);
  }
  return value;
}
```

### mounted
执行核心操作，给 `store` 中插入数据 `columnConfig` 也就是 上面生成的 `column` 对象。

``` javascript
mounted() {
  const owner = this.owner;
  const parent = this.columnOrTableParent;
  const children = this.isSubColumn ? parent.$el.children : parent.$refs.hiddenColumns.children;
  const columnIndex = this.getColumnElIndex(children, this.$el);

  owner.store.commit('insertColumn', this.columnConfig, columnIndex, this.isSubColumn ? parent.columnConfig : null);
}
```

### render
`el-table-column` 组件会将自身以及子节点用div包裹起来，渲染在el-table 的 slot 插槽下。
``` javascript
  render(h) {
    // slots 也要渲染，需要计算合并表头
    return h('div', this.$slots.default);
  }
```


### tip
其实 `el-tablel-column` 只是起到了一个数据配置的作用，用来配置列表的现实字段，以及现实方式等。然后将数据插入到 store 中。
那么一切准备就绪，接下来开始创建 `table-body` 组件，进行数据渲染。