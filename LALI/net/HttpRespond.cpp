//
//Created by lalalilia
//

#include "HttpRespond.h"
#include "Buffer.h"

void HttpRespond::appendToBuffer(Buffer *output) const {
    char buf[32];
    snprintf(buf,sizeof buf,"HTTP/1.1 %d ",status_);
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");
    if(closeConnection_){
        output->append("Connection: close\r\n");
    }
    else{
        snprintf(buf,sizeof buf,"Content-Length: %zd\r\n",body_.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }
    for(const auto& head:headers_){
        output->append(head.first);
        output->append(": ");
        output->append(head.second);
        output->append("\r\n");
    }
    output->append("\r\n");
    output->append(body_);
}