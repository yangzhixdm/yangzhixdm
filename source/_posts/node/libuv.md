---
title: Node异步libuv机制
date: 2021-01-21 13:06:58
tags: node
description: 最近一直在研究 node 的异步机制的实现，记录其中涉及到的一些问题。
---

#### 背景
最近一段时间不知道为啥，对于node性能机制非常好奇，就开始看各种书籍以及资料。所以就有了这一篇总结。

#### 计算机基础知识
##### 中断
中断是指计算机运行过程中，出现某些意外情况需主机干预时，机器能自动停止正在运行的程序并转入处理新情况的程序，处理完毕后又返回原被暂停的程序继续运行。
包括硬件事件，用户事件，计算机故障，断电等事件。

这里还涉及了一个 `中断程序入口映射表`，映射表在内核启动时自动加载，其中存储了对应的 中断的执行方法。不同的中断都对应了一个编号比如（0x80)就是常说的80中断，发送给cpu的指令只是中断的编号，
然后从中断映射表中查询出对应的回调策略处理程序。

CPU 通过接收 `中断控制器` 给到的信号进行处理中断请求，其中`中断控制器` 可以进行级联，中断控制器在主板上直接和硬件进行连接，电流发现变化的时候，就说明产生中断了，通过数据总线向CPU 完成中断请求IRQ.

##### DMA
外围设备可以直接通过DMA直接访问内存。中断产生之后，外围设备可以通过DMA将数据复制到内存中，然后将数据信号送给CPU进行处理，发起一个中断IRQ。(一般外围设备都有自己的缓冲区进行存储数据。) CPU接收到 中断请求之后放弃现在当前的执行进程，将当前进程的数据保存到寄存器中，并将当前的寄存器保存到进程描述符汇中。（每一个进程都有自己的进程描述符）

然后 CPU，从用户态切换到内核态，从内存中获取数据进行处理, 执行结束之后，又切换会用户态，从进程描述符中恢复进程，继续执行。

##### 用户态，内核态

*nix 系统在启动的时候，会对内存进行分配，内核kernel使用部分为内核态，之外让出的部分供应用程序使用为用户态。
系统内核kernel中封装了对内存以及硬件操作的api。用户程序需要操纵内核态的数据，则需要通过内核kernel进行处理。这里就需要涉及从用户态切换到内核态。
而切换是需要代价的。

这里当应用程序需要调用内核程序时，会将 用户态保存的数据进行拷贝到内核态中，然后内核将数据放入到 `缓冲区` （输入缓冲区和输出缓冲区）中，然后内核在缓冲区写入完成之后，统一进行处理。
如果 内核 缓冲区 在被内核处理的过程中，用户态发了数据，那么用户态发送的数据将会被阻塞。直到缓冲区的数据被处理完成，才会继续处理新发送过来的数据。

用户应用程序如果在 `输出缓冲区` 数据没有发送完全之前关闭连接，那么数据不会丢失，缓冲区会继续发送，直到数据完成。但是如果是 `输入缓冲区` 的数据还没有发送完成，用户程序已经关闭，那么数据将会丢失。

##### 保护机制
系统对内核进行保护，禁止高级应用程序直接操内核数据。

##### 切换
用户态和内核态的切换。

#### IO 多路复用
1 select/poll
select/poll 存在的问题

2 epoll
``` shell
epoll_create
epoll_ctrl
epoll_wait
```
#### Node 事件循环
1 事件循环
2 浏览器事件循环
nodejs 与浏览器端的 Web API 版本的事件循环最大的不同的是：
在 nodejs 中事件循环不再是由单一个 task queue 和 micro-task queue 组成，而是由多个 阶段 phase 的多个回调函数队列 callbacks queues 组成一次事件循环 tick。 并且在每一个单独的阶段都存在一个单独的 回调函数 FIFO 队列。

3 process.nextTick

#### libuv
这里的poll，应该是存在另外一个进程，进行 epoll_wait（多路复用epoll机制），然后每次去 poll 一下，看fd的队列中是否存在需要进行处理的队列，
如果有的话就进行执行，否则就直接返回。
```
```
    ┌───────────────────────────┐
┌─> │           timers          │
│   └─────────────┬─────────────┘
│   ┌─────────────┴─────────────┐
│   │   pending callbacks (I/O) │
│   └─────────────┬─────────────┘
│   ┌─────────────┴─────────────┐
│   │       idle, prepare       │
│   └─────────────┬─────────────┘      ┌───────────────┐
│   ┌─────────────┴─────────────┐      │   incoming:   │
│   │           poll            │<─────┤  connections, │
│   └─────────────┬─────────────┘      │   data, etc.  │
│   ┌─────────────┴─────────────┐      └───────────────┘
│   │           check           │
│   └─────────────┬─────────────┘
│   ┌─────────────┴─────────────┐
└───┤      close callbacks      │
    └───────────────────────────┘
```
```
    ┌────────────────────────────────────────────────────────────────┐ ┌─────────────────────────────────┐
    │                                                         libuv  | |                      JavaScript |
    │                                                                | |  ┌───────────────────────────┐  |
    │                               ┌───────────────────────────┐    | |  │        setTimeout         │  |
    │ Run expired timers ````````┌─>│           timers          │````|`|``|        setInterval        |  |
    │                            │  └─────────────┬─────────────┘    | |  └───────────────────────────┘  |
    │                            │  ┌─────────────┴─────────────┐    | |  ┌───────────────────────────┐  |
    │ Run completed I/O handlers`│``│   pending I/O callbacks   │````|`|``│   some finished I/O work/ │  |
    │                            │  └─────────────┬─────────────┘    | |  │        I/O Errors         │  |
    │                            │  ┌─────────────┴─────────────┐    | |  └───────────────────────────┘  |
    │                            │  │       idle handlers       │    | |                                 |
    │                            │  └─────────────┬─────────────┘    | |                                 |
    │                            │  ┌─────────────┴─────────────┐    | |                                 |
    │ Some prep-work before `````│``│      prepare handlers     │    | |       ┌───────────────┐         |
    │ polling for I/O            │  └─────────────┬─────────────┘    | |       │   incoming:   │         |
    │                            │  ┌─────────────┴─────────────┐    | |       |  connections, │         |
    │ Wait for I/O to complete ``│``│          I/O Poll         │ <──|─|───────┤   data, etc.  │         |
    │                            │  └─────────────┬─────────────┘    | |       └───────────────┘         |
    │                            │  ┌─────────────┴─────────────┐    | |  ┌───────────────────────────┐  |
    │ Some checking stuff ```````│``│       check handlers      │````|`|``│        setImmediate       │  |
    │ after polling for I/O      │  └─────────────┬─────────────┘    | |  └───────────────────────────┘  |
    │                            │  ┌─────────────┴─────────────┐    | |  ┌───────────────────────────┐  |
    │ Run any close handlers ````└──┤      close callbacks      │````|`|``│ *.on('close'... handlers) │  |
    │                               └───────────────────────────┘    | |  └───────────────────────────┘  |
    |                                                                | |                                 |
    |                                                                | |                                 |
    └────────────────────────────────────────────────────────────────┘ └─────────────────────────────────┘
```
``` c++
int uv_run(uv_loop_t* loop, uv_run_mode mode) {
  int timeout;
  int r;
  int ran_pending;

  r = uv__loop_alive(loop);
  if (!r)
    uv__update_time(loop);

  while (r != 0 && loop->stop_flag == 0) {
    uv__update_time(loop);
    uv__run_timers(loop);
    ran_pending = uv__run_pending(loop);
    uv__run_idle(loop);
    uv__run_prepare(loop);

    timeout = 0;
    if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
      timeout = uv_backend_timeout(loop);

    uv__io_poll(loop, timeout);

    /* Run one final update on the provider_idle_time in case uv__io_poll
     * returned because the timeout expired, but no events were received. This
     * call will be ignored if the provider_entry_time was either never set (if
     * the timeout == 0) or was already updated b/c an event was received.
     */
    uv__metrics_update_idle_time(loop);

    uv__run_check(loop);
    uv__run_closing_handles(loop);

    if (mode == UV_RUN_ONCE) {
      /* UV_RUN_ONCE implies forward progress: at least one callback must have
       * been invoked when it returns. uv__io_poll() can return without doing
       * I/O (meaning: no callbacks) when its timeout expires - which means we
       * have pending timers that satisfy the forward progress constraint.
       *
       * UV_RUN_NOWAIT makes no guarantees about progress so it's omitted from
       * the check.
       */
      uv__update_time(loop);
      uv__run_timers(loop);
    }

    r = uv__loop_alive(loop);
    if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
      break;
  }

  /* The if statement lets gcc compile it to a conditional store. Avoids
   * dirtying a cache line.
   */
  if (loop->stop_flag != 0)
    loop->stop_flag = 0;

  return r;
}
```
##### timers
主要用于setTimeout和setInterval.
但是timer的执行时机收到poll 阶段的影响。
> 无论在 nodejs 还是 浏览器引擎，timer都不保证在到达时间后回调函数一定会被立即执行，它们只能保证在到达时间阈值后，尽快执行。
``` c++
while (r != 0 && loop->stop_flag == 0) {
  // 每次循环迭代开始之前，开始设置开始时间
  uv__update_time(loop);
  // ...
}
```

``` c++
UV_UNUSED(static void uv__update_time(uv_loop_t* loop)) {
  /* Use a fast time source if available.  We only need millisecond precision.
   */
  loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000;
}
```

``` c++
void uv__run_timers(uv_loop_t* loop) {
  struct heap_node* heap_node;
  uv_timer_t* handle;

  // 循环遍历timer时间节点遍历的最小堆
  for (;;) {
    // 获取最小堆
    heap_node = heap_min(timer_heap(loop));
    // 不存在
    if (heap_node == NULL)
      break;

    handle = container_of(heap_node, uv_timer_t, heap_node);
    // 最小堆顶点如果 > 当前时间，那么直接可以退出
    if (handle->timeout > loop->time)
      break;
    // 否则开始循环执行
    uv_timer_stop(handle);
    uv_timer_again(handle);
    handle->timer_cb(handle);
  }
}
```

比如下面这个例子：
``` javascript
const fs = require('fs');

function someAsyncOperation(callback) {
  // Assume this takes 95ms to complete
  fs.readFile('/path/to/file', callback);
}

const timeoutScheduled = Date.now();

setTimeout(() => {
  const delay = Date.now() - timeoutScheduled;

  console.log(`${delay}ms have passed since I was scheduled`);
}, 100);

// do someAsyncOperation which takes 95 ms to complete
someAsyncOperation(() => {
  const startCallback = Date.now();

  // do something that will take 10ms...
  while (Date.now() - startCallback < 10) {
    // do nothing
  }
});
```

> 为了避免 event loop 饿死，libuv 内部在pool阶段 有一个最大执行时间，超时之后必须返回去执行其他的阶段。

##### node 内置的定时器
``` c++
const TIMEOUT_MAX = 2 ** 31 - 1

// Timer constructor function.
// The entire prototype is defined in lib/timers.js
function Timeout(callback, after, args, isRepeat, isRefed) {
  after *= 1 // Coalesce to number or NaN
  if (!(after >= 1 && after <= TIMEOUT_MAX)) {
    if (after > TIMEOUT_MAX) {
      process.emitWarning(
        `${after} does not fit into` +
          ' a 32-bit signed integer.' +
          '\nTimeout duration was set to 1.',
        'TimeoutOverflowWarning'
      )
    }
    after = 1 // Schedule on next tick, follows browser behavior
  }

  this._idleTimeout = after
  this._idlePrev = this
  this._idleNext = this
  this._idleStart = null
  // This must be set to null first to avoid function tracking
  // on the hidden class, revisit in V8 versions after 6.2
  this._onTimeout = nullv
  this._onTimeout = callback
  this._timerArgs = args
  this._repeat = isRepeat ? after : null
  this._destroyed = false

  if (isRefed) incRefCount()
  this[kRefed] = isRefed

  initAsyncResource(this, 'Timeout')
}
```
> 时间阈值的取值范围是 1 ~ 231-1 ms，且为整数，如果数据不合法，会被重置为 1，所以 `setTimeout(fn, 0)` ,在node中其实是被重置为 `setTimeout(fn, 1)`

##### pending callbacks
执行一些操作系统的回调（system operations ），比如 TCP errors，比如，如果TCP socket尝试链接时，接收到一个 ECONNREFUSED 状态吗，一些 *nix系统就会等待去记录一个错误。这个步骤将会在pending callbacks阶段执行。

##### poll
The poll phase has two main functions:

- Calculating how long it should block and poll for I/O, then
- Processing events in the poll queue.

个人觉得这里的 计算 io的阻塞和轮训时间，应该只是一个预估的过程，通过poll queue中的事件数量来进行一个大体的预估。

进入poll 阶段之后：
- 如果 poll queue队列不为空，直接开始同步执行poll queue中回调，直到全部执行完或者触发最大的系统限制时间。
- 如果poll queue为空，则判断是否存在是否有被setImmediate绑定的回调，如果有则直接执行，否则event loop 会等待回调进行到当前poll queue中，并立即执行他们。

在这之前 系统会去获取 timeout的值，
``` c++
  timeout = 0
  if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
    timeout = uv_backend_timeout(loop);
```
默认值为0，这个值会在之后被传入到epoll_wait中，0表示立即返回， -1 表示永久阻塞，> 0 表示阻塞时间。
然后通过 uv_backend_timeout 进行计算得来。
``` c++
int uv_backend_timeout(const uv_loop_t* loop) {
  // https://github.com/libuv/libuv/blob/v1.35.0/src/uv-common.c#L521-L523
  // http://docs.libuv.org/en/v1.x/guide/eventloops.html#stopping-an-event-loop
  if (loop->stop_flag != 0)
    return 0;

  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;

  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;

  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  if (loop->closing_handles)
    return 0;

  return uv__next_timeout(loop);
}
```
- 当事件循环 tick 被 uv_stop() 函数标记为停止#时，返回 0，即不阻塞。
- 当事件循环 tick 不处于活动状态时且不存在活动的 request 时返回 0，即不阻塞。
- 当 idle 句柄队列不为空时，返回 0，即不阻塞。
- 当 pending callbacks 的回调队列不为空时，返回 0，即不阻塞。
- 当存在 closing 句柄，即存在 close 事件回调时，返回 0，即不阻塞。

如以上条件都不满足，则通过 `uv__next_timeout` 方法获取。
``` c++
int uv__next_timeout(const uv_loop_t* loop) {
  const struct heap_node* heap_node;
  const uv_timer_t* handle;
  uint64_t diff;

  // libuv 计时器二叉最小堆的根节点为所有计时器中距离当前时间节点最近的计时器
  heap_node = heap_min(timer_heap(loop));

  // 此处 true 条件为无限制的阻塞当前 poll 阶段
  if (heap_node == NULL)
    return -1; /* block indefinitely */

  handle = container_of(heap_node, uv_timer_t, heap_node);

  // 若最近时间节点的计时器小于等于当前事件循环 `tick` 开始的时间节点
  // 那么不阻塞，并进入下一阶段，直至进入下一 `tick` 的 `timer` 阶段执行回调函数
  if (handle->timeout <= loop->time)
    return 0;

  // 如 nodejs 文档中对 poll 阶段计算阻塞时间的描述
  // 以下语句用于计算当前 poll 阶段应该阻塞的时间
  diff = handle->timeout - loop->time;
  // INT_MAX 在 limits.h 头文件中声明
  if (diff > INT_MAX)
    diff = INT_MAX;

  return (int) diff;
}
```
这里要注意的是，以下的条件都是给予上面所属的5个条件都不成立的情况下，才会考虑一下的情况,(大体条件即为：不存在其他的事件阶段):
- 若不存在任何计时器，那么当前事件循环 tick 中的 poll 阶段将 无限制阻塞。以实现一旦存在 I/O 回调函数加入到 poll queue 中即可立即得到执行。
- 若最近计时器时间节点小于等于开始时间，则表明在计时器二叉最小堆中 至少存在一个 过期的计时器，那么当前 poll 阶段的超时时间将被设置为 0，即表示 poll 阶段不发生阻塞。这是为了尽可能快的进入下一阶段，即尽可能快地结束当前事件循环 tick。在进入下一事件循环 tick 时，在 timer 阶段，上一 tick 中过期的计时器回调函数得以执行。
- 若最近计时器时间节点大于开始时间，则计算两个计时器之前的差值，且不大于 int 类型最大值。poll 将根据此差值来阻塞当前阶段，这么做是为了在轮询阶段，尽可能快的处理异步 I/O 事件。此时我们也可以理解为 事件循环 tick 始终有一种维持在 poll 阶段的倾向。

> 一旦poll queue为空，事件循环将检查是否已达到其时间阈值的timers。 如果一个或多个timer时间已经到达，则事件循环将返回到timers阶段以执行这些计时器的回调。

##### check
This phase allows a person to execute callbacks immediately after the poll phase has completed. If the poll phase becomes idle and scripts have been queued with setImmediate(), the event loop may continue to the check phase rather than waiting.

setImmediate() is actually a special timer that runs in a separate phase of the event loop. It uses a libuv API that schedules callbacks to execute after the poll phase has completed.

Generally, as the code is executed, the event loop will eventually hit the poll phase where it will wait for an incoming connection, request, etc. However, if a callback has been scheduled with setImmediate() and the poll phase becomes idle, it will end and continue to the check phase rather than waiting for poll events.

##### close
如果 socket突然意外关闭，将会在此阶段调用close.否则将会在 process.nextTick中进行调用。

##### setTimeout和setImmediate
- setImmediate() is designed to execute a script once the current poll phase completes.
- setTimeout() schedules a script to be run after a minimum threshold in ms has elapsed.

如果 setImmediate和setTimeout中没有涉及到io调用，那么他们的执行顺序取决与进程的调度。执行顺序存在不确定性。
如果涉及到 io 循环，那么 setImmediate将会永远都在setTimeout之前执行。
```
// timeout_vs_immediate.js
const fs = require('fs');

fs.readFile(__filename, () => {
  setTimeout(() => {
    console.log('timeout');
  }, 0);
  setImmediate(() => {
    console.log('immediate');
  });
});
```

##### process.nextTick

process.nextTick() fires immediately on the same phase

> any time you call process.nextTick() in a given phase, all callbacks passed to process.nextTick() will be resolved before the event loop continues.
process.nextTick 会在每个阶段执行结束进行触发。process.nextTick可能会导致 event loop饿死。
``` javascript
let bar;

function someAsyncApiCall(callback) {
  process.nextTick(callback);
}

someAsyncApiCall(() => {
  console.log('bar', bar); // 1
});

bar = 1;
```

##### process.nextTick() vs setImmediate()
We have two calls that are similar as far as users are concerned, but their names are confusing.

process.nextTick() fires immediately on the same phase
setImmediate() fires on the following iteration or 'tick' of the event loop。当前event loop执行完成，在close 阶段之前执行，其实就是poll阶段之后。

#### libuv底层actor模型

#### go协程模型

#### 最后
最后附上一下 uv__io_poll 的方法源码，其实就是调用了epoll的系统方法。
其中涉及对epoll_wait的调用，epoll_wait返回接收到时间的文件描述符fd。需要注意的是如果传入的 timeout为0的话，那么会立刻返回，如果为-1则会永远阻塞。

``` c++
void uv__io_poll(uv_loop_t* loop, int timeout) {
  /* A bug in kernels < 2.6.37 makes timeouts larger than ~30 minutes
   * effectively infinite on 32 bits architectures.  To avoid blocking
   * indefinitely, we cap the timeout and poll again if necessary.
   *
   * Note that "30 minutes" is a simplification because it depends on
   * the value of CONFIG_HZ.  The magic constant assumes CONFIG_HZ=1200,
   * that being the largest value I have seen in the wild (and only once.)
   */
  static const int max_safe_timeout = 1789569;
  static int no_epoll_pwait_cached;
  static int no_epoll_wait_cached;
  int no_epoll_pwait;
  int no_epoll_wait;
  struct epoll_event events[1024];
  struct epoll_event* pe;
  struct epoll_event e;
  int real_timeout;
  QUEUE* q;
  uv__io_t* w;
  sigset_t sigset;
  uint64_t sigmask;
  uint64_t base;
  int have_signals;
  int nevents;
  int count;
  int nfds;
  int fd;
  int op;
  int i;
  int user_timeout;
  int reset_timeout;

  if (loop->nfds == 0) {
    assert(QUEUE_EMPTY(&loop->watcher_queue));
    return;
  }

  memset(&e, 0, sizeof(e));

  while (!QUEUE_EMPTY(&loop->watcher_queue)) {
    q = QUEUE_HEAD(&loop->watcher_queue);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);

    w = QUEUE_DATA(q, uv__io_t, watcher_queue);
    assert(w->pevents != 0);
    assert(w->fd >= 0);
    assert(w->fd < (int) loop->nwatchers);

    e.events = w->pevents;
    e.data.fd = w->fd;

    if (w->events == 0)
      op = EPOLL_CTL_ADD;
    else
      op = EPOLL_CTL_MOD;

    /* XXX Future optimization: do EPOLL_CTL_MOD lazily if we stop watching
     * events, skip the syscall and squelch the events after epoll_wait().
     */
    if (epoll_ctl(loop->backend_fd, op, w->fd, &e)) {
      if (errno != EEXIST)
        abort();

      assert(op == EPOLL_CTL_ADD);

      /* We've reactivated a file descriptor that's been watched before. */
      if (epoll_ctl(loop->backend_fd, EPOLL_CTL_MOD, w->fd, &e))
        abort();
    }

    w->events = w->pevents;
  }

  sigmask = 0;
  if (loop->flags & UV_LOOP_BLOCK_SIGPROF) {
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGPROF);
    sigmask |= 1 << (SIGPROF - 1);
  }

  assert(timeout >= -1);
  base = loop->time;
  count = 48; /* Benchmarks suggest this gives the best throughput. */
  real_timeout = timeout;

  if (uv__get_internal_fields(loop)->flags & UV_METRICS_IDLE_TIME) {
    reset_timeout = 1;
    user_timeout = timeout;
    timeout = 0;
  } else {
    reset_timeout = 0;
    user_timeout = 0;
  }

  /* You could argue there is a dependency between these two but
   * ultimately we don't care about their ordering with respect
   * to one another. Worst case, we make a few system calls that
   * could have been avoided because another thread already knows
   * they fail with ENOSYS. Hardly the end of the world.
   */
  no_epoll_pwait = uv__load_relaxed(&no_epoll_pwait_cached);
  no_epoll_wait = uv__load_relaxed(&no_epoll_wait_cached);

  for (;;) {
    /* Only need to set the provider_entry_time if timeout != 0. The function
     * will return early if the loop isn't configured with UV_METRICS_IDLE_TIME.
     */
    if (timeout != 0)
      uv__metrics_set_provider_entry_time(loop);

    /* See the comment for max_safe_timeout for an explanation of why
     * this is necessary.  Executive summary: kernel bug workaround.
     */
    if (sizeof(int32_t) == sizeof(long) && timeout >= max_safe_timeout)
      timeout = max_safe_timeout;

    if (sigmask != 0 && no_epoll_pwait != 0)
      if (pthread_sigmask(SIG_BLOCK, &sigset, NULL))
        abort();

    if (no_epoll_wait != 0 || (sigmask != 0 && no_epoll_pwait == 0)) {
      nfds = epoll_pwait(loop->backend_fd,
                         events,
                         ARRAY_SIZE(events),
                         timeout,
                         &sigset);
      if (nfds == -1 && errno == ENOSYS) {
        uv__store_relaxed(&no_epoll_pwait_cached, 1);
        no_epoll_pwait = 1;
      }
    } else {
      nfds = epoll_wait(loop->backend_fd,
                        events,
                        ARRAY_SIZE(events),
                        timeout);
      if (nfds == -1 && errno == ENOSYS) {
        uv__store_relaxed(&no_epoll_wait_cached, 1);
        no_epoll_wait = 1;
      }
    }

    if (sigmask != 0 && no_epoll_pwait != 0)
      if (pthread_sigmask(SIG_UNBLOCK, &sigset, NULL))
        abort();

    /* Update loop->time unconditionally. It's tempting to skip the update when
     * timeout == 0 (i.e. non-blocking poll) but there is no guarantee that the
     * operating system didn't reschedule our process while in the syscall.
     */
    SAVE_ERRNO(uv__update_time(loop));

    if (nfds == 0) {
      assert(timeout != -1);

      if (reset_timeout != 0) {
        timeout = user_timeout;
        reset_timeout = 0;
      }

      if (timeout == -1)
        continue;

      if (timeout == 0)
        return;

      /* We may have been inside the system call for longer than |timeout|
       * milliseconds so we need to update the timestamp to avoid drift.
       */
      goto update_timeout;
    }

    if (nfds == -1) {
      if (errno == ENOSYS) {
        /* epoll_wait() or epoll_pwait() failed, try the other system call. */
        assert(no_epoll_wait == 0 || no_epoll_pwait == 0);
        continue;
      }

      if (errno != EINTR)
        abort();

      if (reset_timeout != 0) {
        timeout = user_timeout;
        reset_timeout = 0;
      }

      if (timeout == -1)
        continue;

      if (timeout == 0)
        return;

      /* Interrupted by a signal. Update timeout and poll again. */
      goto update_timeout;
    }

    have_signals = 0;
    nevents = 0;

    {
      /* Squelch a -Waddress-of-packed-member warning with gcc >= 9. */
      union {
        struct epoll_event* events;
        uv__io_t* watchers;
      } x;

      x.events = events;
      assert(loop->watchers != NULL);
      loop->watchers[loop->nwatchers] = x.watchers;
      loop->watchers[loop->nwatchers + 1] = (void*) (uintptr_t) nfds;
    }

    for (i = 0; i < nfds; i++) {
      pe = events + i;
      fd = pe->data.fd;

      /* Skip invalidated events, see uv__platform_invalidate_fd */
      if (fd == -1)
        continue;

      assert(fd >= 0);
      assert((unsigned) fd < loop->nwatchers);

      w = loop->watchers[fd];

      if (w == NULL) {
        /* File descriptor that we've stopped watching, disarm it.
         *
         * Ignore all errors because we may be racing with another thread
         * when the file descriptor is closed.
         */
        epoll_ctl(loop->backend_fd, EPOLL_CTL_DEL, fd, pe);
        continue;
      }

      /* Give users only events they're interested in. Prevents spurious
       * callbacks when previous callback invocation in this loop has stopped
       * the current watcher. Also, filters out events that users has not
       * requested us to watch.
       */
      pe->events &= w->pevents | POLLERR | POLLHUP;

      /* Work around an epoll quirk where it sometimes reports just the
       * EPOLLERR or EPOLLHUP event.  In order to force the event loop to
       * move forward, we merge in the read/write events that the watcher
       * is interested in; uv__read() and uv__write() will then deal with
       * the error or hangup in the usual fashion.
       *
       * Note to self: happens when epoll reports EPOLLIN|EPOLLHUP, the user
       * reads the available data, calls uv_read_stop(), then sometime later
       * calls uv_read_start() again.  By then, libuv has forgotten about the
       * hangup and the kernel won't report EPOLLIN again because there's
       * nothing left to read.  If anything, libuv is to blame here.  The
       * current hack is just a quick bandaid; to properly fix it, libuv
       * needs to remember the error/hangup event.  We should get that for
       * free when we switch over to edge-triggered I/O.
       */
      if (pe->events == POLLERR || pe->events == POLLHUP)
        pe->events |=
          w->pevents & (POLLIN | POLLOUT | UV__POLLRDHUP | UV__POLLPRI);

      if (pe->events != 0) {
        /* Run signal watchers last.  This also affects child process watchers
         * because those are implemented in terms of signal watchers.
         */
        if (w == &loop->signal_io_watcher) {
          have_signals = 1;
        } else {
          uv__metrics_update_idle_time(loop);
          w->cb(loop, w, pe->events);
        }

        nevents++;
      }
    }

    if (reset_timeout != 0) {
      timeout = user_timeout;
      reset_timeout = 0;
    }

    if (have_signals != 0) {
      uv__metrics_update_idle_time(loop);
      loop->signal_io_watcher.cb(loop, &loop->signal_io_watcher, POLLIN);
    }

    loop->watchers[loop->nwatchers] = NULL;
    loop->watchers[loop->nwatchers + 1] = NULL;

    if (have_signals != 0)
      return;  /* Event loop should cycle now so don't poll again. */

    if (nevents != 0) {
      if (nfds == ARRAY_SIZE(events) && --count != 0) {
        /* Poll for more events but don't block this time. */
        timeout = 0;
        continue;
      }
      return;
    }

    if (timeout == 0)
      return;

    if (timeout == -1)
      continue;

update_timeout:
    assert(timeout > 0);

    real_timeout -= (loop->time - base);
    if (real_timeout <= 0)
      return;

    timeout = real_timeout;
  }
}
```

#### 参考文档
[从 libuv 看 nodejs 事件循环](https://set.sh/post/200317-how-nodejs-event-loop-works)
[The Node.js Event Loop, Timers, and process.nextTick()](https://nodejs.org/en/docs/guides/event-loop-timers-and-nexttick/)
[libuv Design overview](http://docs.libuv.org/en/v1.x/design.html)
[2016 Node Interactive talk-compressed](https://drive.google.com/file/d/0B1ENiZwmJ_J2a09DUmZROV9oSGc/view)
[Handling IO](https://blog.insiderattack.net/handling-io-nodejs-event-loop-part-4-418062f917d1)
