# LALI RPC FRAME

整体架构图：

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/RPCFRAME.JPG" alt="image-20210101143941368" style="zoom: 30%;" />

1. **基本介绍**

- 异步多线程的 RPC 框架。采用 json 格式的序列化 / 反序列化方案，程序自动生成 service/client stub 程序，用户 include 相应 stub 即可接收/发起 RPC ；客户端支持异步 RPC；服务端支持多线程 RPC, 即用户可以将 RPC 请求交给另一个线程 (或者线程池) 去执行，这样 IO 线程就可以立刻开始等待下一个请求。
- 基于 NON-Blocking IO + IO multiplexing（epoll 水平触发） 的 Reactor 事件驱动模型的网络服务器。使用多线程充分利用多核 CPU，并使用了线程池避免了频繁创建销毁线程的开销，主线程只 accept 请求，以 Round Robin 的方式分发给其它 IO 线程，采用 eventfd 实现线程异步唤醒

2. **RPC **

- 通过![[公式]](https://www.zhihu.com/equation?tex=service.method) 唯一定义一个 procedure. 一个 TCP 端口可以对外提供多个 service, 一个 service 可以有多个 method；
- 利用 stub 存根文件解析器直接解析 json 文件，自动生成存根程序；
- 存根文件 ServerStub ：可以将 json 请求消息里面的参数提取出来转化成普通的函数调用，然后把函数的返回值包装成一个 json 消息发出去；
- 存根文件 ClientStub ：可以将函数的参数包装到 json 请求消息中，通过网络库发出，然后把收到的 json 回复消息的返回值、错误信息提取出来给用户回调；

3. **设计细节**

- 异步日志系统。使用双缓冲区及批处理方式
- 使用 timerfd 将定时器当作 IO 事件处理，并使用红黑树进行定时器的关闭超时请求
- 使用智能指针的 RAII 机制，减少了内存泄漏的可能
- 支持服务器的优雅关闭
- 具有简单 http 服务可提供 url 访问地址实现简单的服务器压力测试



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
| net/Connector.h&.cc                                          | 用于 TcpClient，实现 tcp 主动连接，重连尝试                  |
| net/TcpClient.h&.cc                                          | 管理 tcp 的主动连接，一个 tcpclient 只有一个线程             |
| net/Acceptor.h&.cc                                           | 用于接受 TCP 连接                                            |
| net/Tcpconnectino.h&.cc                                      | 单个连接的处理类，处理连接之间的通信及断开，优雅断开连接     |
| net/EventLoopThreadPool.h&.cc                                | 线程池，one EventLoop per thread                             |
| net/HttpServer.h&.cc                                         | http 服务实现，处理 http 协议                                |
| zrpc/server/*                                                | 解析 json 字段，将返回消息包装成 json 字段，调用 TcpServer 进行网络连接及消息传输服务 |
| zrpc/client/*                                                | 调用了 TcpClient ，对服务端发起连接，能够解析服务端的回复 json 字段获取消息，向服务器发送 json 形式的 rpc 请求 |
| zrpc/stub/*                                                  | 能够解析 json 文件实现 RPC Stub 存根文件的自动生成           |
| jackson/*                                                    | 用于实现 json 库的解析                                       |
| test/*                                                       | 测试文件                                                     |



## 测试使用

如下安装本代码

```
git clone http://github.com/LALI_Project
cd LALI_Project
cmake .
make
```

安装完毕后，CmakeLists 中已经写好并构建了 4 个可执行文件，

①stub_test ：输入一个 json 文件会自动生成服务端与客户端的存根头文件

②RpcServer_test ：测试的 RPC 服务器，运行后等待 RPC 客户端的连接

③RpcClient_test ：测试的RPC 客户端，运行后连接服务端进行 RPC 请求

④http_test ：一个简单的 http 服务，运行后浏览器键入 `http://localhost:8001` 可看到网页界面，用于对服务器进行压力测试

成功运行后使用抓包软件 wireshark 会看到 json 形式的 RPC 请求

test 文件夹中含 test.json 文件内容如下：

```json
{
  "name": "RpcTest",
  "rpc": [
    {
      "name": "Add",
      "params": {"lhs": 1.0, "rhs": 1.0},
      "returns": 2.0
    },
    {
      "name": "Sub",
      "params": {"lhs": 1.0, "rhs": 1.0},
      "returns": 0.0
    }
  ]
}
```

在文件目录下使用 stub_test ，输入

```shell
./stub_test -i ./test/test.json -o
```

即可生成 `RpcTestClientStub.h` 和 `RpcTestServiceStub.h` 两个头文件，再编译 `test/RpcClientTest.cpp` 及 `test/RpcServiceStub.cpp`  即可生成可执行文件，在这两个文件中包含了 上述两个头文件。



## 压力测试

由于没有两台机器，只能把服务端和客户端都设置在本地进行服务器测试。

系统 **Ubuntu18.04** ，socket ： 2，Core per Socket ： 2，主频：1800MHZ；

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/cpu.jpg" alt="image-20210101133839722" width="500" height="450" />

### 四线程

关闭日志写入及输出，使用 Webbench 进行测试，http 服务器中发送消息为内存中的 `hello` 加上了必要的 http 头，所以消除了磁盘 IO 的影响。开启 3 线程，包含连接线程共计 4 线程，进行测试。webbench 开启 500 线程，持续 60 s；

以下是测试结果：

**服务器响应：**

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/4THREAD60S.JPG" alt="image-20210101145656741" width="600" height="200" />

**top 查看服务器资源利用：**

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/TOPINFO60S.JPG" alt="image-20210101145411123" width="600" height="250" />

可以看到在 500 线程 持续 60s 的情况下，qps 达到了 **25755**；

由于 load average 系统负载分别为 1 分钟，5 分钟，15 分钟，所以选择了压测时间为 60 s，在到时间后观察系统负载的第一个数值为 **5.39**，说明程序充分使用了 CPU，**us + sy = 49.2 < 70** ,有效的利用了 cpu 。

### 双线程

如果只开 1 个线程（加上连接处理主线程共计 2 线程）进行测试，会发现结果如下：

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/1THREAD60S.JPG" alt="image-20210101150431916" width="600" height="250" />

qps 下降到了 **11478**，**所以本多线程服务器能够良好的利用多核环境**；

### 五线程

如果开 4 线程（加上连接处理主线程共计 5 线程），超过了本机的核心数，结果如下：

<img src="https://gitee.com/lalalilia/images-include/raw/master/project/5THREAD60S.JPG" alt="image-20210101150945653" width="600" height="250" />

qps 为 **17027**，是由于超过了 cpu 核数，发生了额外的上下文切换导致性能下降。

