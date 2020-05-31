//
// Created by lalalilia on 4/11/20.
//

#ifndef UNTITLED1_THREADLOCALSINGLETON_H
#define UNTITLED1_THREADLOCALSINGLETON_H


#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <stdio.h>
#include <iostream>

template <typename T>
class ThreadLocalSingleton : boost::noncopyable{

public:

    static T& instance(){
        if(!t_value){
            t_value = new T();
            deleter_.set(t_value);
        }
        return *t_value;
    }

    static T* pointer(){
        return *t_value;
    }

private:
    static void destructor(void* obj){
        delete t_value;
        t_value = 0;
    }
    class Deleter{
    public:
        Deleter(){
            pthread_key_create(&pkey_,&ThreadLocalSingleton::destructor);
        }

        ~Deleter(){
            pthread_key_delete(pkey_);
        }

        void set(T* obj){
            pthread_setspecific(pkey_,obj);
        }

    private:
        pthread_key_t pkey_;
    };
    static __thread T* t_value;
    static Deleter deleter_;
};

template <typename T>
__thread T* ThreadLocalSingleton<T>::t_value = NULL;

template <typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

#endif //UNTITLED1_THREADLOCALSINGLETON_H
