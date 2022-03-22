---
title: Chrome 插件s
date: 2022-03-22 13:51:00
tags: chrome
description: chrome 插件开发中对 background.js 进行调试。
---

### 有言在先
之前为了方便开发，自己整了一个 chrome 右键的小插件，涉及到 chrome extension 的一些问题，稍微记录下。

### 开启开发模式

- 访问 `chrome://extensions/` 或者 右侧的菜单进行 `扩展管理`
{% asset_image chrome-extension-3.png %}

- 开启开发模式
{% asset_image chrome-extension-1.png %}

- 点击 `chrome extension` 中的 `background page` 连接
{% asset_image chrome-extension-2.png %}

### 设置权限
设置chrome 插件权限, `manifest.json` 如下：
``` js
{
  "manifest_version": 2,

  "name": "Yicall-cookie-cli",
  "description": "This extension demonstrates a browser action with kittens.",
  "version": "1.0",

  "permissions": [
    "contextMenus", // 右键菜单权限
    "cookies", // cookie
    "tabs", // 获取tabs
    "http://*/*",
    "https://*/*"
  ],
  "browser_action": {
    "default_icon": "icon.png",
    "default_popup": "popup.html"
  },
  "background": {
    "scripts": ["background.js"], // 指定background.js
    "persistent": false
  },
  "icons": {
    "16": "icon.png" // 设定 icon
  }
}
```

### background.js
这里需要注意一点：`background.js` 和在 `popup.html` 中引用的 js 文件是不同的。`background.js` 会在加载插件的时候就已经执行了，而 `popup.html`中的js 需要打开`poppu.html` 才会被加载。
``` js

function genericOnClick(info, tab) {
  chrome.tabs.getSelected(null, function (tab) {
      var url = new URL(tab.url);
      var hostName = url.hostname;
      var regexp = /^(\d+)\.(\d+).(\d+).(\d+)$/;
      if (!regexp.test(hostName)) {
        hostName = '.wezhuiyi.com'
      }

      var target = '127.0.0.1';
      var port = '8080';
      const targetRediret = `http://${target}:${port}${url.search}`;

      const copyItem = { 'aiforce': true, 'yicall_token': true, 'token': true };

      chrome.cookies.getAll({
        domain: hostName
      },(cookies) => {
        for (let i = 0; i < cookies.length; i++) {
          if (copyItem[cookies[i].name]) {
            chrome.cookies.set({
              url: `http://${target}/`,
              name: cookies[i].name,
              value: cookies[i].value,
              path: cookies.path,
              domain: target,
              secure: false
            })
          }
        }
        window.open(targetRediret)
      })
  });
}

chrome.contextMenus.create({
  type: 'normal',
  title: 'YTranslate',
  id: 'y-translate',
  contexts: ['all'],
}, function () {
  console.log('contextMenus are create.');
});


chrome.contextMenus.onClicked.addListener(genericOnClick);
```

### 注意点
- 每次修改文件需要重新拉取一次项目
- 如果产生错误，`插件管理器`会有提示
