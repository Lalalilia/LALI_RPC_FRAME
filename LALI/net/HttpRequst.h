//
// Created by lalalilia on 6/8/20.
//

#ifndef UNTITLED1_HTTPREQUST_H
#define UNTITLED1_HTTPREQUST_H

#include <string>
#include <map>
#include <stdio.h>
#include <utility>
#include "../base/Timestamp.h"

class HttpRequest{

public:
    enum Method{
        Get,Post,Head,Put,Delete,Invaild
    };
    enum Version{
        Http10,Http11,Unknown
    };
    HttpRequest()
    :method_(Invaild),
    version_ (Unknown){

    }
    void setVersion(Version v){
        version_=v;
    }
    Version getVersion() const{
        return version_;
    }
    bool setMethod(const char*start,const char* end){
        std::string temp(start,end);
        if(temp=="GET"){
            method_=Get;
        }
        else if(temp=="POST"){
            method_=Post;
        }
        else if(temp=="HEAD"){
            method_=Head;
        }
        else if(temp=="PUT"){
            method_=Put;
        }
        else if(temp=="DELETE"){
            method_=Delete;
        }
        else{
            method_=Invaild;
        }
        return method_!=Invaild;

    }

    Method getMethod() const{
        return method_;
    }

    std::string methodToString(){
        std::string back("Invalid");
        switch (method_){
            case Get:
                back="GET";
                break;
            case Post:
                back="POST";
                break;
            case Put:
                back="PUT";
                break;
            case Head:
                back="HEAD";
                break;
            case Delete:
                back="DELETE";
                break;
            default:
                break;

        }
    }


    void setpath(const char* start,const char* end){
        path_.assign(start,end);
    }
    const std::string& getPath() const {
        return path_;
    }
    void setQuery(const char* start,const char* end){
        query_.assign(start,end);
    }
    const std::string& getQuery() const{
        return query_;
    }
    void setReceiveTime(Timestamp time){
        receiveTime_=time;
    }
    Timestamp getReceiveTime() const{
        return receiveTime_;
    }
    void addHeader(const char* start,const char* colon,const char* end){
        std::string headname(start,colon);
        colon++;
        while(colon<end&&isspace(*colon)){
            colon++;
        }
        std::string value(colon,end);
        while(!value.empty()&&isspace(value[value.size()-1])){
            value.resize(value.size()-1);
        }
        headers_[headname]=value;
    }

    string getHeader(const std::string& headname) const {
        std::string ans;
        auto it=headers_.find(headname);
        if(it!=headers_.end())ans=it->second;
        return ans;
    }

    const std::map<std::string ,std::string>& getHeaders() const {
        return headers_;
    }

    void swap(HttpRequest& newone){
        std::swap(method_,newone.method_);
        std::swap(version_,newone.version_);
        path_.swap(newone.path_);
        query_.swap(newone.query_);
        receiveTime_.swap(newone.receiveTime_);
        headers_.swap(newone.headers_);
    }


private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    Timestamp receiveTime_;
    std::map<std::string ,std::string >headers_;

};



#endif //UNTITLED1_HTTPREQUST_H
