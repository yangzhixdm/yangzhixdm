---
title: Node异步libuv机制
date: 2021-01-21 13:06:58
tags: node
description: 最近一直在研究 node 的异步机制的实现，记录其中涉及到的一些问题。
---

#### 背景
最近一段时间不知道为啥，对于node性能机制非常好奇，就开始看各种书籍以及资料。所以就有了这一篇总结。

#### 计算机基础知识
1 中断
2 用户态，内核态
3 保护机制
4 切换

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
3 process.nextTick

#### libuv
这里的poll，应该是存在另外一个进程，进行 epoll_wait（多路复用epoll机制），然后每次去 poll 一下，看fd的队列中是否存在需要进行处理的队列，
如果有的话就进行执行，否则就直接返回。
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
##### timers
主要用于setTimeout和setInterval.
但是timer的执行时机收到poll 阶段的影响。
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

> 猜想： 估计poll 阶段应该有一个最小的执行时间，这里的等待应就是在最小时间还没到达时，就一直等待，时间到达就进入下一阶段了。毕竟存在一个idle空闲时间的机制。

> 一旦poll queue为空，事件循环将检查是否已达到其时间阈值的timers。 如果一个或多个timer时间已经到达，则事件循环将返回到timers阶段以执行这些计时器的回调。

##### check
This phase allows a person to execute callbacks immediately after the poll phase has completed. If the poll phase becomes idle and scripts have been queued with setImmediate(), the event loop may continue to the check phase rather than waiting.

setImmediate() is actually a special timer that runs in a separate phase of the event loop. It uses a libuv API that schedules callbacks to execute after the poll phase has completed.

Generally, as the code is executed, the event loop will eventually hit the poll phase where it will wait for an incoming connection, request, etc. However, if a callback has been scheduled with setImmediate() and the poll phase becomes idle, it will end and continue to the check phase rather than waiting for poll events.

##### close
如果 socket突然意外关闭，将会在此阶段调用close.否则将会在 process.nextTick中进行调用。
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