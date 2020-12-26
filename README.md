# LALI RPC FRAME

- 异步多线程的 RPC 框架。采用 json 格式的序列化 / 反序列化方案，程序自动生成 service/client stub 程序，用户 include 相应 stub 即可接收/发起 RPC ；服务端支持多线程 RPC, 即用户可以将 RPC 请求交给另一个线程 (或者线程池) 去执行，这样 IO 线程就可以立刻开始等待下一个请求。
- 基于 NON-Blocking IO + IO multiplexing（epoll 水平触发） 的 Reactor 事件驱动模型的网络服务器。使用多线程充分利用多核 CPU，并使用了线程池避免了频繁创建销毁线程的开销，主线程只 accept 请求，以 Round Robin 的方式分发给其它 IO 线程，采用 eventfd 实现线程异步唤醒
- 异步日志系统。使用双缓冲区及批处理方式
- 使用 timerfd 将定时器当作 IO 事件处理，并使用红黑树进行定时器的关闭超时请求
- 使用智能指针的 RAII 机制，减少了内存泄漏的可能
- 支持服务器的优雅关闭
- 客户端支持异步 RPC, 也可以通过线程同步达到同步 RPC 的效果.
- 服务端支持多线程 RPC, 即用户可以将 RPC 请求交给另一个线程 (或者线程池) 去执行，这样 IO 线程就可以立刻开始等待下一个请求.
- 通过![[公式]](https://www.zhihu.com/equation?tex=service.method) 唯一定义一个 procedure. 一个 TCP 端口可以对外提供多个 service, 一个 service 可以有多个 method.



## 代码介绍

本项目基于 **Ubuntu18.04** 系统，使用了 **pthread** 线程库，使用了 C++ 11（实际上主要代码都是 C++ 11，但为了兼容开源的 json 库，库中使用了 string_view 这一结构，所以使用了 C++ 17 编译）

**zrpc** 中含客户端和服务端的 RPC 协议及自动生成存根文件的实现代码。

**base** 文件中为锁，闭锁，线程，线程池，条件变量，时间等封装类便于使用，并且有日志库LOG的实现。

**net** 文件中包含核心事件驱动模型的类 EventLoop，Channel，Poller 类的实现，处理Tcp连接的 TcpServer，TcpConnecion，Acceptor 类的实现，处理Http请求的 Httpserver，HttpRequest，HttpRespond，HttpContext 类的实现。

**test**  文件中包含一些简单的小程序测试，后续会逐渐完善。

**jackson** 开源的 json 库

| 文件名                                                       | 简介                                                         |
| ------------------------------------------------------------ | :----------------------------------------------------------- |
| base/Mutex.h                                                 | 自行封装的一个锁类，能够将锁的lock和unlock用类的构造和析构函数自动执行，以免忘记将锁unlock； |
| base/thread.h&.cc                                            | 线程类，封装线程相关函数，用于线程创建，启动；               |
| base/Condition.h                                             | 条件变量封装，用于线程同步                                   |
| base/CountDownLatch.h&.cc                                    | 闭锁类，用于线程同步                                         |
| base/ThreadPool.h&.cc                                        | 线程池类                                                     |
| base/Timestamp.h&.cc                                         | 时间戳类，获取Unix时间，进行处理，转化成格式化string，并实现操作符重载支持时间戳的比较； |
| base/ThreadLocal.h                                           | 线程存储类，让一个线程独立拥有数据，不受其他线程影响，而线程安全。 |
| base/Singleton.h                                             | 用pthread_once 实现的单例模式。                              |
| base/Exception.g&.cc                                         | 异常类，继承 std::exception 类，用于错误提示（待完善）       |
| base/Logging.h&.cc<br />Logstream.h&.cc<br />LogFile.h&.cc<br />FileUtil.h&.cc | 实现日志LOG类，logging 为日志实现模块核心，负责接收信息并写入文件，Logstream 是自己实现的一个流类，不使用语言自带的 iostream；LogFile 是用于处理日志文件，并会自动滚动日志文件；FileUtil 将日志写入硬盘 |
| base/AsyncLogging.h&.cc                                      | 异步写日志，利用双缓冲，批处理机制高效将日志写入硬盘         |
| net/EventLoop.h&.cc                                          | 处理事件循环                                                 |
| net/Channel.h&.cc                                            | 通道类用于不同事件类型分发，调用相应的回调函数               |
| net/Poller.h&.cc                                             | 实现 IO 复用，检测活跃事件                                   |
| net/Buffer.h&.cc                                             | buffer 缓存类用于接收信息及发送信息                          |
| net/Socket.h&.cc                                             | 将 socket 套接字封装成类管理                                 |
| net/Tcpserver.h&.cc                                          | 管理 tcp 连接和断开，主线程只负责接受连接，将连接分发给其他线程 |
| net/Acceptor.h&.cc                                           | 用于接受 TCP 连接                                            |
| net/Tcpconnectino.h&.cc                                      | 单个连接的处理类，处理连接之间的通信及断开，优雅断开连接     |
| net/EventLoopThreadPool.h&.cc                                | 线程池，one EventLoop per thread                             |
| net/HttpServer.h&.cc                                         | http 服务实现，处理 http 协议                                |



## 测试使用

如下安装本代码

```
git clone http://github.com/LALI_Project
cd LALI_Project
cmake .
make
```

安装完毕后，CmakeLists 中已经写好并构建了 3 个可执行文件，

①stub_test ：输入一个 json 文件会自动生成服务端与客户端的存根头文件

②RpcServer_test ：测试的 RPC 服务器，运行后等待 RPC 客户端的连接

③RpcClient_test ：测试的RPC 客户端，运行后连接服务端进行 RPC 请求

成功运行后使用抓包软件会看到 json 形式的 RPC 请求





