---
title: nodeJS 与 npm(node pagkage manager) 配置
date: 2015-06-06 12:16:22
tags:
description: nodejs安装。
---
一直准备开始学习nodejs,但是不得不说nodejs在Windows上的安装与配置是个坑。
尤其是npm的中模块位置的配置。
好了废话不多说，首先下载nodejs版本。
然后一路next即可，即可安装成功，其中你可以选择自己的安装路径。我这里是安装在d:\nodejs下面。
安装完成之后命令行中输入:
```  bash
node -v
npm -v
```
出现如下输出就算是安装成功了。
![](/postimg/20150606122011869.png)

那么接下来存在的问题就是，如果直接使用npm 安装模块，那么它会将你的模块安装到c盘的 appdata下面去，以保证全局可以使用。
这样的话，不方便管理。那么我想把它直接放在Nodejs的安装目录下，且能够保证全局下使用，怎么办呢？
进入cmd命令行，输入如下命令
``` bash
npm config set prefix "D:\nodejs\node_global"
npm config set cache "D:\nodejs\node_cache"
```
注意：这里需要注意一点，如果npm 之前由于使用set prefix出错，导致 输入npm 之后，光标一直在闪，但是没有任何反应的情况。那么可以去到　c盘  C:\Users\Administrator 目录下（我这里是admin用户），修改 .npmrc 文件，或者将其删除，则一切正常。

当然首先需要在 nodejs的下面手动建立 node_global 和node_cache目录。
node_global用来存放你下载的模块，node_cache 目录自然是存放cache。
![](/postimg/20150606122339638.png)

之后你在使用 npm install 模块名 -g
那么你下载的模块将会保存在 node_global中，而不会再保存在 c盘的appdata下面去了。

接下来还有个问题需要处理。
现在模块是保存好了，但是当我们需要用到的时候require的时候，却找不到模块。
这里就需要配置环境变量NODE_PATH.

在我的电脑->属性->高级系统配置 ->环境变量->系统变量中添加 NODE_PATH.
然后将你的node_global 路径输入进去。
主要这里的路径需要带上node_modules.
如：我的global 目录为  `d:\nodejs\node_global`
那么这里我的NODE_PATH 配置为 
``` bash
d:\nodejs\node_global\node_mudules
```
![](/postimg/20150606122016010.png)

这里可以先安装express 模块测试
``` bash
npm install express -g
```
等待光标转动完毕后，可以在node_global 中的 node_mudules 查看到express 目录，
这证明exprss安装成功，且位置正确。
配置完NODE_PATH毕之后再cmd 命令行中，输入命令，出现如下界面，这表示成功获取到 express 模块，配置正确。

![](/postimg/20150606121756430.png)

NodeJs踩过的坑，以记录之，谨防忘记。