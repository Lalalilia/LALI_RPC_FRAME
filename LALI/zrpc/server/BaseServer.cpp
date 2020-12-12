//
// Created by lalalilia on 12/1/20.
//

#include <zrpc/server/BaseServer.h>
#include <jackson/Document.h>
#include <jackson/Writer.h>
#include <jackson/StringWriteStream.h>
#include <zrpc/server/Rpcserver.h>

const size_t kMaxMessageLen = 100*1024*1024;

template class BaseServer<RpcServer>;

template <typename ProtocolServer>
BaseServer<ProtocolServer>::BaseServer(EventLoop *loop, const AddressOps &listen)
    :server_(loop,listen){
    server_.setThreadNums(3);
    server_.setConnectionCallback(
            std::bind(&BaseServer::onConnection,this,_1));
    server_.setMessageCallback(
           std::bind(&BaseServer::onMessage,this,_1,_2,_3));
}


template <typename ProtocolServer>
void BaseServer<ProtocolServer>::onConnection(const TcpConnectionPtr &conn) {
    if(conn->connected()){
        LOG << "connection "<<
            conn->peerAddr().toHostPort().c_str()
            << " is up";
        //conn->setHighWaterMarkCallback();
    }
}

template <typename ProtocolServer>
void BaseServer<ProtocolServer>::onMessage(const TcpConnectionPtr &conn, Buffer* buffer,ssize_t len) {
    try{
        handleMessage(conn,buffer);
    }
    catch (const char* msg){
        LOG << msg;
        cout << msg;
    }
}


template <typename ProtocolServer>
void BaseServer<ProtocolServer>::handleMessage(const TcpConnectionPtr &conn, Buffer* buffer) {
    while(true){
        const char* crlf = buffer->findCRLF();
        if(crlf == nullptr)break;
        if(crlf == buffer->peek()){
            buffer->retrieve(2);
            break;
        }

        size_t headerLen = crlf - buffer->peek() + 2;

        json::Document header;


        auto err = header.parse(buffer->peek(),headerLen);

        if(err != json::PARSE_OK ||
           !header.isInt32() ||
           header.getInt32() <= 0){
            throw "invalid message length";
        }

        auto jsonLen = static_cast<uint32_t >(header.getInt32());
        if(jsonLen >= kMaxMessageLen)
            throw "message is too long";

        if(buffer->readableBytes() < headerLen + jsonLen)break;

        buffer->retrieve(headerLen);
        auto json = buffer->retrieveAsString();

        convert().handleRequest(json,[conn,this](json::Value response){
            if(!response.isNull()){
                sendResponse(conn,response);
                LOG << "Baserver::handleMessage() "
                    << conn->peerAddr().toHostPort().c_str()
                    << "request success";
            }
            else{
                LOG << "Baserver::handleMessage() "
                    << conn->peerAddr().toHostPort().c_str()
                    << "notify success";
            }
        });
    }
}

template <typename ProtocolServer>
void BaseServer<ProtocolServer>::sendResponse(const TcpConnectionPtr &conn, const json::Value &response) {
    json::StringWriteStream os;
    json::Writer writer(os);
    response.writeTo(writer);

    auto message = std::to_string(os.get().length() + 2)
            .append("\r\n")
            .append(os.get())
            .append("\r\n");
    conn->send(message);

}

template <typename ProtocolServer>
ProtocolServer& BaseServer<ProtocolServer>::convert()
{
    return static_cast<ProtocolServer&>(*this);
}

template <typename ProtocolServer>
const ProtocolServer& BaseServer<ProtocolServer>::convert() const
{
    return static_cast<const ProtocolServer&>(*this);
}