//
//Created by lalalilia
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
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

void defaultConnectionCallback(const TcpConnectionPtr&);
void defaultMessageCallback(const TcpConnectionPtr&,
                            Buffer* data,
                            ssize_t len);

#endif //UNTITLED1_CALLBACK_H
