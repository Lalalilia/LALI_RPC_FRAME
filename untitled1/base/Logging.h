//
// Created by lalalilia on 4/16/20.
//

#ifndef UNTITLED1_LOGGING_H
#define UNTITLED1_LOGGING_H

#include "Logstream.h"
#include "Timestamp.h"


class Logger{

public:
    Logger(const char* filename,int line);
    ~Logger();
    Logstream& stream(){return impl_.stream_;}
    typedef void (*OutputFunc)(const char* msg,int len);
    typedef void (*FlushFunc)();

    static void setOutput(OutputFunc);
    static void steFlush(FlushFunc);

private:
    class Impl{
    public:
        Impl(const char* filename,int line);
        void formatTime();
        void finsh();
        Timestamp time_;
        Logstream stream_;

        int line_;
        const char* filename_;
    };

    Impl impl_;

    static const char* logFileName_;

};

#define  LOG  Logger(__FILE__,__LINE__).stream()


#endif //UNTITLED1_LOGGING_H
