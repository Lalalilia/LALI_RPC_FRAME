//
// Created by lalalilia on 4/30/20.
//

#ifndef UNTITLED1_CALLBACK_H
#define UNTITLED1_CALLBACK_H


#include <functional>
#include <memory>


class TcpConnection;
class Buffer;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void ()> TimerCallback;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&,
                            Buffer* data,
                            ssize_t len)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;

#endif //UNTITLED1_CALLBACK_H
