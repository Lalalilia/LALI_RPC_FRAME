//
// Created by lalalilia 
//
#ifndef UNTITLED1_EXCEPTION_H
#define UNTITLED1_EXCEPTION_H


#include <exception>
#include <string>

class Exception:public std::exception{

public:
    explicit Exception(const char* msg);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
    std::string stack_trace() const throw();

private:
    const char* message_;
    std::string stack_trace_;
    void fillStack_trace();

};

#endif //UNTITLED1_EXCEPTION_H
