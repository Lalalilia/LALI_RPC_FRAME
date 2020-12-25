//
//Created by lalalilia
//

#ifndef UNTITLED1_HTTPCONTEXT_H
#define UNTITLED1_HTTPCONTEXT_H

#include "HttpRequst.h"

class Buffer;
class HttpContext{
public:
    enum HttpRequstPart{
        RequestLine,
        Headers,
        Body,
        End,
    };
    HttpContext()
    :part_(RequestLine){

    }
    bool parseRequst(Buffer* buf,Timestamp receiveTime);

    bool parseAll() const {
        return part_==End;
    }

    void reset(){
        part_=RequestLine;
        HttpRequest newone;
        request_.swap(newone);
    }

    const HttpRequest& getRequest() const {
        return request_;
    }

    HttpRequest& getRequest(){
        return request_;
    }


private:
    bool partOnRequestLine(const char* start,const char* end);
    HttpRequest request_;
    HttpRequstPart part_;

};

#endif //UNTITLED1_HTTPCONTEXT_H
