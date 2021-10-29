---
title: 探究 ElementUI 组件 table-body的实现
date: 2021-10-27 15:04:28
tags: source-code, el-table
description: 探究 ElementUI 中 table-body的实现方案。
---

关于 table-body 的实现方案。

### props
接受一个store对象，其实其中渲染列表的数据，以及列表的属性，都保存在里面。
``` javascript
props: {
  store: {
    required: true
  },
  stripe: Boolean,
  context: {},
  rowClassName: [String, Function],
  rowStyle: [Object, Function],
  fixed: String,
  highlight: Boolean
},
```

### computed
从 store 中 提取 `data` 和  `columns` 属性，并绑定响应书。
``` javascript
...mapStates({
  data: 'data',
  columns: 'columns',
  treeIndent: 'indent',
  leftFixedLeafCount: 'fixedLeafColumnsLength',
  rightFixedLeafCount: 'rightFixedLeafColumnsLength',
  columnsCount: states => states.columns.length,
  leftFixedCount: states => states.fixedColumns.length,
  rightFixedCount: states => states.rightFixedColumns.length,
  hasExpandColumn: states => states.columns.some(({ type }) => type === 'expand')
}),
```

### render
1 获取 `data` 也就是 `store` 中的 data。
2 开始渲染table,并设置 colgroup。然后对 data执行 reduce方法，迭代进行渲染。
``` javascript
render(h) {
  const data = this.data || [];
  return (
    <table
      class="el-table__body"
      cellspacing="0"
      cellpadding="0"
      border="0">
      <colgroup>
        {
          this.columns.map(column => <col name={ column.id } key={column.id} />)
        }
      </colgroup>
      <tbody>
        {
          data.reduce((acc, row) => {
            return acc.concat(this.wrappedRowRender(row, acc.length)); //渲染每一行
          }, [])
        }
        <el-tooltip effect={ this.table.tooltipEffect } placement="top" ref="tooltip" content={ this.tooltipContent }></el-tooltip>
      </tbody>
    </table>
  );
},
```
对每一行数据，z执行 wrappedRowRender 方法。先判断数据是否涉及展开，以及是否需要按照 tree(树形)渲染。
``` javascript
wrappedRowRender(row, $index) {
  const store = this.store;
  const { isRowExpanded, assertRowKey } = store;
  const { treeData, lazyTreeNodeMap, childrenColumnName, rowKey } = store.states;
  if (this.hasExpandColumn && isRowExpanded(row)) {
    const renderExpanded = this.table.renderExpanded;
    const tr = this.rowRender(row, $index);
    if (!renderExpanded) {
      console.error('[Element Error]renderExpanded is required.');
      return tr;
    }
    // 使用二维数组，避免修改 $index
    return [[
      tr,
      <tr key={'expanded-row__' + tr.key}>
        <td colspan={ this.columnsCount } class="el-table__cell el-table__expanded-cell">
          { renderExpanded(this.$createElement, { row, $index, store: this.store }) }
        </td>
      </tr>]];
  } else if (Object.keys(treeData).length) {
    assertRowKey();
    // TreeTable 时，rowKey 必须由用户设定，不使用 getKeyOfRow 计算
    // 在调用 rowRender 函数时，仍然会计算 rowKey，不太好的操作
    const key = getRowIdentity(row, rowKey);
    let cur = treeData[key];
    let treeRowData = null;
    if (cur) {
      treeRowData = {
        expanded: cur.expanded,
        level: cur.level,
        display: true
      };
      if (typeof cur.lazy === 'boolean') {
        if (typeof cur.loaded === 'boolean' && cur.loaded) {
          treeRowData.noLazyChildren = !(cur.children && cur.children.length);
        }
        treeRowData.loading = cur.loading;
      }
    }
    const tmp = [this.rowRender(row, $index, treeRowData)];
    // 渲染嵌套数据
    if (cur) {
      // currentRow 记录的是 index，所以还需主动增加 TreeTable 的 index
      let i = 0;
      const traverse = (children, parent) => {
        if (!(children && children.length && parent)) return;
        children.forEach(node => {
          // 父节点的 display 状态影响子节点的显示状态
          const innerTreeRowData = {
            display: parent.display && parent.expanded,
            level: parent.level + 1
          };
          const childKey = getRowIdentity(node, rowKey);
          if (childKey === undefined || childKey === null) {
            throw new Error('for nested data item, row-key is required.');
          }
          cur = { ...treeData[childKey] };
          // 对于当前节点，分成有无子节点两种情况。
          // 如果包含子节点的，设置 expanded 属性。
          // 对于它子节点的 display 属性由它本身的 expanded 与 display 共同决定。
          if (cur) {
            innerTreeRowData.expanded = cur.expanded;
            // 懒加载的某些节点，level 未知
            cur.level = cur.level || innerTreeRowData.level;
            cur.display = !!(cur.expanded && innerTreeRowData.display);
            if (typeof cur.lazy === 'boolean') {
              if (typeof cur.loaded === 'boolean' && cur.loaded) {
                innerTreeRowData.noLazyChildren = !(cur.children && cur.children.length);
              }
              innerTreeRowData.loading = cur.loading;
            }
          }
          i++;
          tmp.push(this.rowRender(node, $index + i, innerTreeRowData));
          if (cur) {
            const nodes = lazyTreeNodeMap[childKey] || node[childrenColumnName];
            traverse(nodes, cur);
          }
        });
      };
      // 对于 root 节点，display 一定为 true
      cur.display = true;
      const nodes = lazyTreeNodeMap[key] || row[childrenColumnName];
      traverse(nodes, cur);
    }
    return tmp;
  } else {
    return this.rowRender(row, $index);
  }
}
```
### 渲染 tr和 td
在这里 开始进行 td 渲染，调用 column中的 `renderCell`（在 el-table-column 中的`setColumnRenders`方法进行指定) 方法。
``` javascript
rowRender(row, $index, treeRowData) {
  const { treeIndent, columns, firstDefaultColumnIndex } = this;
  const columnsHidden = columns.map((column, index) => this.isColumnHidden(index));
  const rowClasses = this.getRowClass(row, $index);
  let display = true;
  if (treeRowData) {
    rowClasses.push('el-table__row--level-' + treeRowData.level);
    display = treeRowData.display;
  }
  // 指令 v-show 会覆盖 row-style 中 display
  // 使用 :style 代替 v-show https://github.com/ElemeFE/element/issues/16995
  let displayStyle = display ? null : {
    display: 'none'
  };
  return (<tr
    style={ [displayStyle, this.getRowStyle(row, $index)] }
    class={ rowClasses }
    key={ this.getKeyOfRow(row, $index) }
    on-dblclick={ ($event) => this.handleDoubleClick($event, row) }
    on-click={ ($event) => this.handleClick($event, row) }
    on-contextmenu={ ($event) => this.handleContextMenu($event, row) }
    on-mouseenter={ _ => this.handleMouseEnter($index) }
    on-mouseleave={ this.handleMouseLeave }>
    {
      columns.map((column, cellIndex) => {
        const { rowspan, colspan } = this.getSpan(row, column, $index, cellIndex);
        if (!rowspan || !colspan) {
          return null;
        }
        const columnData = { ...column };
        columnData.realWidth = this.getColspanRealWidth(columns, colspan, cellIndex);
        const data = {
          store: this.store,
          _self: this.context || this.table.$vnode.context,
          column: columnData,
          row,
          $index
        };
        if (cellIndex === firstDefaultColumnIndex && treeRowData) {
          data.treeNode = {
            indent: treeRowData.level * treeIndent,
            level: treeRowData.level
          };
          if (typeof treeRowData.expanded === 'boolean') {
            data.treeNode.expanded = treeRowData.expanded;
            // 表明是懒加载
            if ('loading' in treeRowData) {
              data.treeNode.loading = treeRowData.loading;
            }
            if ('noLazyChildren' in treeRowData) {
              data.treeNode.noLazyChildren = treeRowData.noLazyChildren;
            }
          }
        }
        return (
          <td
            style={ this.getCellStyle($index, cellIndex, row, column) }
            class={ this.getCellClass($index, cellIndex, row, column) }
            rowspan={ rowspan }
            colspan={ colspan }
            on-mouseenter={ ($event) => this.handleCellMouseEnter($event, row) }
            on-mouseleave={ this.handleCellMouseLeave }>
            {
              column.renderCell.call(
                this._renderProxy,
                this.$createElement,
                data,
                columnsHidden[cellIndex]
              )
            }
          </td>
        );
      })
    }
  </tr>);
}
```
渲染到此结束。