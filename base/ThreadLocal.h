//
// Created by lalalilia on 4/7/20.
//

#ifndef UNTITLED1_THREADLOCAL_H
#define UNTITLED1_THREADLOCAL_H


// 写一个线程存储类。对于线程存储，目的就是让一个全局变量能够给每个线程独立拥有；

#include <pthread.h>
#include "iostream"

/// 声明成一个模板类是因为数据会有很多种类型；
template<typename T>
class ThreadLocal{
public:
    ThreadLocal(){
        pthread_key_create(&pkey_,&destructor);
    }

    ~ThreadLocal(){
        pthread_key_delete(pkey_);

    }

    T& Value(){
        T* pkey_data_ = static_cast<T*>(pthread_getspecific(pkey_));
        if(!pkey_data_){
            T* temp = new T();
            pthread_setspecific(pkey_,temp);
            pkey_data_ = temp;
        }
        return *pkey_data_;
    }

private:
    static void destructor(void* x){

        T* obj = static_cast<T*>(x);
        //std::cout << *static_cast<T*>(x) << std::endl;
        delete obj;

    }
    pthread_key_t pkey_;



};


#endif //UNTITLED1_THREADLOCAL_H
