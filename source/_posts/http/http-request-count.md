---
title: http连接请求数
date: 2019-02-27
tags: http
---

#### HTTP1.0 浏览器加载情况
在没有keep-alive的时代，每一个请求都需要单独的建立一个连接，每一个连接都需要与服务器握手与挥手的操作。其中效率可想而知。

#### HTTP1.1 浏览器的加载情况
> 一图解千愁
![http1.1加载情况](https://raw.githubusercontent.com/helloyangzhi/learn/master/http1.1-load-resoure-mum.jpg)

注意的点:
1 HTTP1.1中keep-alive，同一个连接在一定的时间内可以被多次请求数据，而这个时间在WEB服务器中可以进行配置。
2 请求的限制数量是针对于同一域名的，同一个域名下的同时连接数量有一定的限制，目前大部分最大连接数量为6个，旧式IE为2个。 
3 针对不同的资源，放在不同的域名可以，可以是的资源同时并发加载，一定程度可以缓解请求数量的限制。如果所有资源都放在同一个域名下，那么超出数量的资源，将会排队。如static.ak.fbcdn.net，ssl.gstatic.com

以下来自[Google's resource](http://code.google.com/speed/page-speed/docs/rtt.html#ParallelizeDownloads)
> The HTTP 1.1 specification (section 8.1.4) states that browsers should allow at most two concurrent connections per hostname (although newer browsers allow more than that: see Browserscope for a list). If an HTML document contains references to more resources (e.g. CSS, JavaScript, images, etc.) than the maximum allowed on one host, the browser issues requests for that number of resources, and queues the rest. As soon as some of the requests finish, the browser issues requests for the next number of resources in the queue. It repeats the process until it has downloaded all the resources. In other words, if a page references more than X external resources from a single host, where X is the maximum connections allowed per host, the browser must download them sequentially, X at a time, incurring 1 RTT for every X resources. The total round-trip time is N/X, where N is the number of resources to fetch from a host. For example, if a browser allows 4 concurrent connections per hostname, and a page references 100 resources on the same domain, it will incur 1 RTT for every 4 resources, and a total download time of 25 RTTs.

> Balance parallelizable resources across hostnames. Requests for most static resources, including images, CSS, and other binary objects, can be parallelized. Balance requests to all these objects as much as possible across the hostnames. If that's not possible, as a rule of thumb, try to ensure that no one host serves more than 50% more than the average across all hosts. So, for example, if you have 40 resources, and 4 hosts, each host should serve ideally 10 resources; in the worst case, no host should serve more than 15. If you have 100 resources and 4 hosts, each host should serve 25 resources; no one host should serve more than 38.

> Static content, such as images, JS and CSS files, don't need to be accompanied by cookies, as there is no user interaction with these resources. You can decrease request latency by serving static resources from a domain that doesn't serve cookies. This technique is especially useful for pages referencing large volumes of rarely cached static content, such as frequently changing image thumbnails, or infrequently accessed image archives. We recommend this technique for any page that serves more than 5 static resources. (For pages that serve fewer resources than this, it's not worth the cost of setting up an extra domain.)
To reserve a cookieless domain for serving static content, register a new domain name and configure your DNS database with a CNAME record that points the new domain to your existing domain A record. Configure your web server to serve static resources from the new domain, and do not allow any cookies to be set anywhere on this domain. In your web pages, reference the domain name in the URLs for the static resources.
[参考地址](https://webmasters.stackexchange.com/questions/26753/why-do-big-sites-host-their-images-css-on-external-domains)

#### HTTP2 浏览器的加载情况
1 HTPP2中提出了二进制数据帧与流传输，并对帧的顺序进行标记
2 HTTP2中不管加载多少文件，，只建立一个连接，并可以将最大请求数拉到服务的最大值。