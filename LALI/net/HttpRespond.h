//
//Created by lalalilia
//

#ifndef UNTITLED1_HTTPRESPOND_H
#define UNTITLED1_HTTPRESPOND_H

#include <string>
#include <map>
class Buffer;
class HttpRespond{
public:
    enum HttpRespondStatus{
        r200Succeed=200,
        r404Notfound=404,
        r301MovePermanently=301,
        r400BadRequest=400,
        rUnknown,
    };

    explicit HttpRespond(bool close)
    :status_(rUnknown),
    closeConnection_(close){

    }

    void setCloseConnection(bool type){
        closeConnection_= type;
    }
    bool closeConnection() const{
        return closeConnection_;
    }

    void setStatus(HttpRespondStatus s){
        status_=s;
    }
    void setStatusMessage(const std::string& message){
        statusMessage_=message;
    }

    void addHeader(const std::string& headname,const std::string& value){
        headers_[headname]=value;
    }
    void setBody(const std::string& body){
        body_=body;
    }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<std::string,std::string> headers_;
    HttpRespondStatus  status_;
    std::string statusMessage_;
    std::string body_;
    bool closeConnection_;

};
#endif //UNTITLED1_HTTPRESPOND_H
