//
// Created by lalalilia on 4/5/20.
//
#ifndef UNTITLED1_EXCEPTION_H
#define UNTITLED1_EXCEPTION_H
/*
	我想写一个属于自己的错误提示类，继承这个 excception 来做；
	首先需要干嘛？
	数据成员：
		首先需要把 throw 的信息打印出来把，用一个string来接收 就叫 message_ 把
		然后得有个string来存储一下 stack trace 栈调用的历史 就叫 stack_trace_ 吧；
	构造函数&析构函数：
		构造的时候要先初始化一下类，首先得用 catch 到的东东来初始化给 message_ 赋值；
		析构的时候得干嘛？什么都不干，因为exception已经是个虚函数，那么这个析构函数也是个虚函数了；
	成员函数：
		需要一个函数来看抛出的错误信息是什么，也就是 message_;
		需要一个函数来看函数的调用栈也就是 stack_trace_;
		需要一个函数来填满我的 stack_trace_ ，这个函数就叫 fillStack_trace 吧；

	不准构造函数隐式转换，避免不必要的错误；
	what 是一个虚函数，是为了保证Exception基类的多态性；
	把会调用到的函数都加上 const throw 关键字，为避免内部数据被改变，避免抛出错误是程序阻塞；

*/

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
