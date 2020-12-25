//
//Created by lalalilia
//
#include "Exception.h"
#include <execinfo.h>
#include <stdlib.h>
#include <string>
#include <cstdio>

void Exception::fillStack_trace(){
    const int len = 200;
    void* buffer[len];
    char** strings;
    int traces_number = ::backtrace(buffer, len);
    strings = ::backtrace_symbols(buffer, traces_number);

    if(strings){
        for(int i=0;i<traces_number;i++){
            stack_trace_.append(strings[i]);
            stack_trace_.append("\n");
        }
    }
    free(strings);
}

const char* Exception::what() const throw(){
    return message_;
}

std::string Exception::stack_trace() const throw(){
    return stack_trace_;
}
Exception::Exception(const char* msg)
        :message_(msg){
    fillStack_trace();
}
Exception::~Exception() throw(){

}
