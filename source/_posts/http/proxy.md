---
title: HTTP 代理
date: 2022-03-01 15:58:26
tags: http
description: 关于Whistle前端使用以及实现代理的原理分享。
---
> 以下为在公司给后端兄弟们的分享，关于如何使用`Whistle`工具进行代理，以及代理的一些原理知识。
### 有言在先
问题1 ：
突然发现线上某个资源，或者某个 api接口可能出现问题，但是又无法判定具体问题，现在希望在无侵入代码的情况下，用本地资源或者本地的接口替换排查问题？

问题2：
现在有多个环境，后端在验证过程中，突然Dev环境的前端代码丢失或者报错，但是Test环境的前端的代码是ok的，如何用测试环境的前端代码暂时替换开发环境的代码，保证后端开发的调试过程呢？

{% asset_image http-proxy-1.png %}

### 代理工具
HTTP 请求代理:
nginx: 成本太高，不方便随时切换
Fiddler:  Windows 环境，抓取请求方便，需要引入willow插件，容易出现问题
Charles： MacOs代理工具，Fiddler在mac的替代品，不好用
Whistle: 跨平台，node环境即可，浏览器配置

### 应用场景
1 开发环境代码调试
2 环境切换
3 请求拦截
4 VPN
5 内网渗透

### 如何使用
#### 安装 
安装node环境，然后安装 whistle
``` bash
npm install  whistle -g
```
#### 安装插件
chrome 安装插件 SwitchyOmega
{% asset_image http-proxy-2.png %}

#### 配置代理
设置代理规则，支持 IP ，正则等等
{% asset_image http-proxy-3.png %}

#### 切换代理
{% asset_image http-proxy-4.png %}

> 到这里，所有的HTPP 浏览器请求 基本都可以在netowrk中看到，并且可以查看对应的数据内容

#### HTTPS 代理
##### 下载 CA 证书
{% asset_image http-proxy-5.png %}
##### 信任证书
浏览器设置为 信任的机构颁发，手机端需要在配置描述文件 设置 信任。

#### 手机抓包
##### 网络设置
首先你得保证你手机，也就是你的App和 whistle 所在的网络是同一个网络。（否则洗洗睡吧）
因为App得能找到 whistle 开启的代理。
手机设置wifi 代理，直接使用 whistle 页面右上角 的 online 那里提示的ip地址，然后端口使用 8899 即可。
这样你所有的 请求都会从 whistle 的 network 中看到。
{% asset_image http-proxy-6.png %}

##### 手机设置
扫描 whistle 上面 https 二维码，然后使用手机浏览器打开，下载证书。
下载下来之后，IOS 需要在 `设置` > `通用` > `VPN与设备管理` > `配置描述文件` 那里，安装一下。
安装好了之后，在 `设置` > `通用` > `关于本机` > `证书信任设置` 那里设置信任。

> 然后就可以抓取所有 App 请求的数据了。

### 工具原理
#### 正向代理和反向代理
{% asset_image http-proxy-7.png %}

#### 代理过程
{% asset_image http-proxy-8.png %}

#### 代理种类
{% asset_image http-proxy-9.png %}

不经过代理的请求：客户端和服务端直接建立连接后，即可开始交换数据
经过代理的请求：客户端不与服务端直接建立连接，而是先跟代理建立连接后，将目标服务器的地址发送给代理，通过代理再跟服务端建立连接，这里如果代理服务为HTTP Server，则称为HTTP代理。

那么有 HTTP代理有会有 HTTPS代理，HTTPS代理 解决的问题是为了防止在客户端和代理之间传输数据的过程中由于数据明文导致数据泄漏。上面的代理选择中选择http 就是只浏览器和代理之间传输使用HTTP协议。
除了	http/https，还有 socket4/5 等代理协议。

#### 代理请求过程
正常请求协议
```
GET / HTTP/1.1
Host: google.com
Connection: keep-alive
```
代理请求协议
```
GET http://google.com/ HTTP/1.1
Host: google.com
Proxy-Connection: keep-alive
```
{% asset_image http-proxy-10.png %}

以上的数据传递方式都是使用明文传递，客户端与代理服务器，代理服务器与目标服务器之间都是使用 HTTP 协议进行连接。


#### HTTPS
如果使用上面HTTP的方式进行处理，那么浏览器和代理建立 SSL (安全传输层协议)，代理服务器和目标服务器进行SSL。
> 但是关键问题就在与 代理服务器没有目标服务器的私钥证书。
代理服务器和服务器进行https 认证，客户端和Proxy进行认证。浏览器请求的时候目的服务器，使用的是服务器的公钥证书，而代理服务器没有服务器的私钥证书，所以会导致随机数的校验失败，浏览器和Proxy无法建立连接。

#### HTTP Tunnel
> httptunnel creates a bidirectional virtual data path tunnelled in HTTP requests. The requests can be sent via an HTTP proxy if so desired.

{% asset_image http-proxy-11.png %}
connect请求：一般非HTTP请求，如HTTPS、HTTP/2、WebSocket、TCP等会通过该事件将目标服务器地址传过来，触发该事件的代理请求也叫隧道代理。

{% asset_image http-proxy-12.png %}
> 这里可以看出来，客户端向隧道服务器发送的请求是 `HTTP` 协议(这里其实是conect请求），但是 隧道服务器向目标服务器发送请求是走的 `TCP` 协议。

#### 隧道建立过程
{% asset_image http-proxy-13.png %}

#### 解析数据
##### 首先建立 tunnel 通道
1 首先客户端/浏览器会以 `HTTP代理协议`发送  `connect` 请求和 `tunnel proxy`建立一条双向的tcp连接。
2 然后 `tunel proxy` 会和代理服务器 通过 `TCP`（其实底层使用的是socket）建立连接。

{% asset_image http-proxy-14.png %}
##### 数据解析
通过模拟中间人攻击方式进行解析数据，大致过程如下：
- 客户端通过 `HTTP 代理协议` 发送 `connect` 请求到隧道代理（每个请求都会发送一个connect请求）
- 隧道道理解析`connect` 请求中的数据，然后包装通过 `TCP` 发送到 代理服务器（也就是模拟的中间人代理）
- 代理服务器通过自己的 `私钥证书`解析 客户端发过来的数据。
- 代理服务器使用 `真实服务器` 的 `公钥` 进行加密数据，然后通过`HTTPS` 发送到 `真实服务器`
- 代理服务器获取到`真实服务器`返回的数据，然后使用`公钥`解密
- 将解密的数据使用`代理服务器`自己的`私钥`进行加密，发送给`客户端`
- 客户端使用`代理服务器`的`公钥`进行解密，然后获取到数据内容
{% asset_image http-proxy-15.png %}

### 一些问题
> 上面说到客户端到隧道服务器之间使用 HTTP代理协议发送connect请求。那么还没有其他的代理方式呢？

其实是有的,因为 HTTP 协议的传递的数据都是明文传输，所以导致了传递是的数据都是不安全，可能被劫持的。所以就有了HTTPS 代理协议。
但是如果要使用 HTTPS代理协议，那么将 隧道服务器和客户端走SSL即可。

> 为什么 HTTPS 代理不能直接使用 HTTPS 的中间人攻击方式进行解析数据？

由于 代理服务器根本就不可能有真实服务器的私钥证书，因为浏览器里面保存的是真实服务器的公钥证书。毕竟你在浏览器输入的是真实服务器的域名，所以浏览器请求的是真实服务器的公钥。
如果走隧道的话，那么就可以改变浏览器请求的证书，让它请求的是我们自己给的证书。

> whistle 安装的 ca 证书是干嘛的？

其实 whistle 开启了两个服务器，8899端口对应的是 tunnel 服务器，8900则对应代理服务器。
这个ca 证书就是代理服务器自己用的证书，8899只是起到 TCP 数据转发的作用。

### one more thing
{% asset_image http-proxy-16.png %}

> 所以即使是使用机场、vpn、其他代理软件，其实所有的数据基本都是可以被劫持的，都是不安全的。

### 遗留问题
1 系统如果存在多个代理，会如何处理？
3 每个请求都会发送一个tcp连接，性能会不会有问题？
3 http3 是否同样适用？
4 socket5 和 http tunnel 区别？

