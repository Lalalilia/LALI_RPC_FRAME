cmake_minimum_required(VERSION 3.15)

project(untitled1)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        untitled1
        main.cpp
        base/Thread.cc
        base/Exception.cpp
        base/CurrentThread.h
        base/ThreadLocal.h
        base/Mutex.h
        base/Condition.h
        base/CountDownLatch.h base/CountDownLatch.cpp
        base/BlockingQueue.h
        base/BoundBlockingQueue.h
        base/ThreadLocalSingleton.h
        base/ThreadPool.h base/ThreadPool.cpp
        base/Singleton.h
        base/Timestamp.h base/Timestamp.cpp
        base/Logstream.h base/Logstream.cpp
        base/Logging.h base/Logging.cpp
        base/AsyncLogging.h base/AsyncLogging.cpp
        base/LogFile.h base/LogFile.cpp
        base/FileUtil.h base/FileUtil.cpp
        net/EventLoop.h net/EventLoop.cpp
        net/Channel.h net/Channel.cpp
        net/Poller.h net/Poller.cpp
        net/TimerQueue.h net/TimerQueue.cpp
        net/Timer.h net/Timer.cpp
        net/TImerId.h
        net/Callback.h
        net/ThreadEventLoop.h net/ThreadEventLoop.cpp
        net/SocketOps.h net/SocketOps.cpp
        net/AddressOps.h net/AddressOps.cpp
        net/Acceptor.h net/Acceptor.cpp
        net/TcpConnection.h net/TcpConnection.cpp
        net/TcpServer.h net/TcpServer.cpp
        net/Buffer.h net/Buffer.cpp net/EventLoopThreadPool.h net/EventLoopThreadPool.cpp)

find_package(Threads REQUIRED)

target_link_libraries (untitled1 Threads::Threads)




