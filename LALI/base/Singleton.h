//
// Created by lalalilia on 4/13/20.
//

#ifndef UNTITLED1_SINGLETON_H
#define UNTITLED1_SINGLETON_H

#include <pthread.h>
#include <stdlib.h>

template<typename T>
class Singleton {

public:
    static T& instance(){
        pthread_once(&ponce_,&Singleton<T>::init);
        return *value_;
    }

private:
    static void init(){
        value_ = new T();
        ::atexit(destroy);
    }

    static void destroy(){
        typedef char T_must_be_complete_type[sizeof(T)?1:-1];
        delete value_;
    }

    Singleton();
    ~Singleton();

    static pthread_once_t ponce_;
    static T* value_;

};
template <typename T>
pthread_once_t Singleton<T>::ponce_=PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;


#endif //UNTITLED1_SINGLETON_H
