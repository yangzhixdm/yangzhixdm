---
title: 前端抓取请求跨域问题
date: 2021-11-25 11:12:52
tags:
description: 关于前端抓取请求的一些手段，这玩意其实挺简单的，就是想记录一而已。
---

### 有言在先
其实这玩意说白了就是人们嘴里常说的抓包，无路是浏览器还是App还是其他什么的终端，需要和服务器进行交互就得发送请求，那么我们希望看到这个请求的过程，然后在合适的时候，出于某些目的修改这些请求，包括请求数据，或是相应的数据，来达到自己的需求。

### 工具
一般来说以前在windows上都是推荐使用 `fiddle` 来进行抓包, 但是小爷近来一直使用的mac版本，`charles` 虽说很出名，但是本人太菜，还是感觉不好用。

### whistle
终极利器好吧， 不用依赖什么环境的问题，直接上 `node` 版本，然后再稍微的利用一下浏览器就可以搞定所有的事情。

#### 关于抓包
安装之后，启动 `whistle`, 并直接在浏览器访问 `127.0.0.1:8899`
``` bash
w2 start -p 8899
```
可以直接在 rules 中进行配置url 拦截，并且是支持各种 正则匹配规则啥的，其实也是挺简单的，如果只是想匹配某一条规则，就直接写死就好了。
比如:
```bash
http://localhost.charlesproxy.com:8080/yicall/bot-audio/remove file://\Users/kevin/work/mock/bot-audio-remove.json
```
需要注意一点就是：前面的是target（也即是你要代理的url), 后面是source(即你给出的请求响应）。
source可以是文件地址，也可以是一个 value, 如果你不想创建文件或是提供url啥的，就直接在Values tab页面创建一个value值，然后像下面一样配置就好了：
```bash
http://local-proxy.com:8080/yicall/bot-intent/node-list {bot-intent-node-list.json}
```
是吧，简单得一批。

#### 跨域问题处理
使用 `whistle` 进行本地代理，会发现某些请求是属于使用 `cors` 方式进行跨域处理的。如果直接使用本地的文件进行代理的话，浏览器会直接将该请求进行拒绝处理。（实际该请求已经发送到后端，只是浏览器对响应进行了屏蔽而已）。如果要解决这问题，就得修改掉 响应头 的`header`, 使其 符合 `cors` 的策略，让浏览器不在进行屏蔽。

操作方式直接在需要进行代理的 url 上在匹配一个 `resHeader` 即可：
你可能需要在 `Values` tab中 添加一个新 value (我这里就叫 cors.json), 或者说新建一个响应文件配置给 对应的url，如下：
``` text
Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, token
Access-Control-Allow-Methods: POST, GET, OPTIONS
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true
content-type: application/json
status: 200
```
需要注意的是这里 `Access-Control-Allow-Origin` 可能在某些情况下，直接写 `*` 会有问题，需要设置对应的域名。比如说你当前域名是啥，就直接写当前域名就好了。

然后在Rules 中添加 rules:
``` text
https://www.host.com/queryAction {actions.json}
https://www.host.com/queryAction resHeaders://{cors.json}

```

如上，抓取跨域的问题应该就没什么问题了。


#### 关于 App 抓包
这玩意其实也挺简单的，需要一些前提条件：首先你得保证你手机，也就是你的`App`和 `whistle` 所在的网络是同一个网络，这样才有接下来的可能。（否则就洗洗睡吧）
原因就是你的`App`得能找到 `whistle` 开启的代理，否则连代理地址都找不到，还代理个啥呢。

ok 一切准备就绪。

在你的手机上，设置wifi 代理，直接使用 `whistle` 页面右上角 的 `online` 那里提示的ip地址，然后端口使用 `8899` 即可。

这样你所有的 请求都会从 `whistle` 的 `network` 中看到。

一切搞定，世界似乎恢复了平静。

#### HTTPS
想得美呢，肯定还要解决`https`的问题呀。使用 `whistle` 呢，就直接 扫描 `whistle` 上面 `https` 二维码，然后使用手机浏览器打开，直接下载证书。
下载下来之后，如果是 IOS 的话（不好意思，我就是IOS),需要在 `设置` > `通用` > `VPN与设备管理` > `配置描述文件` 那里，安装一下。
安装好了之后，在 `设置` > `通用` > `关于本机` > `证书信任设置` 那里设置信任。

接下来，世界就真的平静了。

see you again!