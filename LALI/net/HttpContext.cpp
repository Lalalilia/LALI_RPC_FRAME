//
//Created by lalalilia
//

#include "Buffer.h"
#include "HttpContext.h"
#include <algorithm>

bool HttpContext::partOnRequestLine(const char *start, const char *end) {
    bool succeed=false;
    const char* begin=start;
    const char* space=std::find(begin,end,' ');
    if(space!=end&&request_.setMethod(start,space)){
        begin=space+1;
        space=std::find(begin,end,' ');
        if(space!=end){
            const char* question=std::find(start,space,'?');
            if(question!=space){
                request_.setpath(begin,question);
                request_.setQuery(question,space);
            }
            else{
                request_.setpath(begin,space);
            }
            begin=space+1;
            succeed=end-begin==8&&std::equal(begin,end-1,"HTTP/1.");
            if(succeed){
                if(*(end-1)=='1')request_.setVersion(HttpRequest::Http11);
                else if(*(end-1)=='0')request_.setVersion(HttpRequest::Http10);
                else succeed= false;
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequst(Buffer *buf, Timestamp receiveTime) {
    bool right=true;
    bool Noend=true;
    while(Noend){
        if(part_==RequestLine){
            const char* crlf=buf->findCRLF();
            if(crlf){
                right=partOnRequestLine(buf->peek(),crlf);
                if(right){
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf+2);
                    part_=Headers;
                }
                else{
                    Noend=false;

                }
            }
            else{
                Noend= false;
            }
        }
        else if(part_==Headers){
            const char* crlf=buf->findCRLF();
            if(crlf){
                const char* colon=std::find(buf->peek(),crlf,':');
                if(colon!=crlf){
                    request_.addHeader(buf->peek(),colon,crlf);
                }
                else{
                    part_=End;
                    Noend= false;
                }
                buf->retrieveUntil(crlf+2);

            }
            else{
                Noend= false;
            }
        }
        else if(part_==Body){

        }
    }
    return right;
}