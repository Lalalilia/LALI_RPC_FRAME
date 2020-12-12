//
// Created by lalalilia
//

#include <zrpc/client/RpcClient.h>
#include <jackson/Document.h>
#include <jackson/StringWriteStream.h>
#include <jackson/Writer.h>
#include <base/Exception.h>
#include <base/Logging.h>


const size_t kMaxMessageLen = 65536;


void RpcClient::sendCall(const TcpConnectionPtr &conn,
        json::Value &call, const ResponseCallback &cb) {
    call.addMember("id",id_);
    callbacks_[id_] = cb;
    id_ ++;
    sendRequest(conn,call);

}

void RpcClient::sendNotify(const TcpConnectionPtr &conn, json::Value &notify) {
    sendRequest(conn,notify);
}

void RpcClient::sendRequest(const TcpConnectionPtr &conn, json::Value &request) {
    json::StringWriteStream os;
    json::Writer writer(os);
    request.writeTo(writer);

    auto message = std::to_string(os.get().length() + 2)
            .append("\r\n")
            .append(os.get())
            .append("\r\n");
    conn->send(message);
}


void RpcClient::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, ssize_t len) {
    try{
        handleMessage(buffer);
    }
    catch (Exception& e){
        LOG << "response error is " << e.what();
    }

}

void RpcClient::handleMessage(Buffer *buffer) {
    while(1){
        const char* crlf = buffer->findCRLF();
        if(crlf == nullptr)
            break;

        size_t headerLen = crlf - buffer->peek() + 2;

        json::Document header;
        auto err = header.parse(buffer->peek(),headerLen);
        if(err != json::PARSE_OK ||
            !header.isInt32() ||
            header.getInt32() <= 0){
            buffer->retrieve(headerLen);
            throw Exception("invalid message length");
        }

        auto bodyLen = static_cast<uint32_t >(header.getInt32());
        if(bodyLen >= kMaxMessageLen)
            throw Exception("message is too long");

        if(buffer->readableBytes() < headerLen + bodyLen)
            break;

        buffer->retrieve(headerLen);
        auto json = buffer->retrieveAsString();
        handleResponse(json);
    }
}

void RpcClient::handleResponse(std::string &json) {
    json::Document response;
    json::ParseError err = response.parse(json);
    if (err != json::PARSE_OK)
        throw Exception("response parse error");

    switch(response.getType()){
        case json::TYPE_OBJECT:
            handleSingleResponse(response);
            break;
        case json::TYPE_ARRAY:{
            size_t n = response.getSize();
            if(n == 0)
                throw Exception("batcj response is empty");
            for(int i = 0; i < n; i++){
                handleSingleResponse(response[i]);
            }
            break;
        }

        default:
            throw Exception("response should be json object or array");
    }

}

void RpcClient::handleSingleResponse(json::Value &response) {
    validateResponse(response);
    auto id = response["id"].getInt32();

    auto it = callbacks_.find(id);
    if(it == callbacks_.end()){
        LOG << "response not found in stub";
        return;
    }

    auto result = response.findMember("result");
    if(result != response.memberEnd()){
        it->second(result->value,false,false);
    }
    else{
        auto error = response.findMember("error");
        assert(error != response.memberEnd());
        it->second(error->value,true,false);
    }
    callbacks_.erase(it);
}

json::Value& findValue(json::Value& value,const char* key,json::ValueType type){
    auto it = value.findMember(key);
    if(it == value.memberEnd()){
        throw Exception("missing at least one field");
    }
    if(it->value.getType() != type){
        throw Exception("bad type of at least one field");
    }
    return it->value;
}

void RpcClient::validateResponse(json::Value &response) {
    if(response.getSize() != 3){
        throw Exception("response should have 3 field");
    }

    auto id = findValue(response,
            "id",json::TYPE_INT32).getInt32();

    auto version = findValue(response,"jsonrpc",
                         json::TYPE_STRING).getStringView();

    if(version != "2.0")
        throw Exception("unkonwn json rpc version");

    if(response.findMember("result") != response.memberEnd())
        return ;

    findValue(response,"error",json::TYPE_OBJECT);
}
