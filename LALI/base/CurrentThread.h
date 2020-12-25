//
// Created by lalalilia on 4/7/20.
//

#ifndef UNTITLED1_CURRENTTHREAD_H
#define UNTITLED1_CURRENTTHREAD_H

// 用于标定线程 id ，使用 tid
namespace CurrentThread{
    extern __thread  int t_cachedTid ;
    extern __thread  char t_stringTid[32];
    extern __thread const char* t_threadName;

    void CacheTid();

    inline int tid(){
        if(t_cachedTid==0){
            CacheTid();
        }
        return t_cachedTid;
    }


    inline int getCacheTid(){
        return t_cachedTid;
    }

    inline char* getStringTid(){
        return t_stringTid;
    }

    inline const char* getThreadName(){
        return t_threadName;
    }

}

namespace detail{

}




#endif //UNTITLED1_CURRENTTHREAD_H
