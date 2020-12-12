//
// Created by lalalilia on 4/16/20.
//
#include "Logging.h"
#include "CurrentThread.h"
#include "Timestamp.h"


static void DefaultOutput(const char* msg,int len){
    fwrite(msg,1,len,stdout);
}

static void DefaultFlush(){
    fflush(stdout);
}

Logger::OutputFunc F_output = DefaultOutput;
Logger::FlushFunc F_flush = DefaultFlush;

Logger::Impl::Impl(const char* filename,int line)
        :time_(Timestamp::now()),
         stream_(),
         line_(line),
         filename_(filename){
    formatTime();
    stream_ << CurrentThread::getThreadName() << CurrentThread::tid();
}


void Logger::Impl::formatTime(){
    char buf[64] = {0};
    const int kMicroSecondsPerSecond = 1000*1000;
    auto seconds = static_cast<time_t>( time_.microSecondsSinceEpoch() / kMicroSecondsPerSecond);
    struct tm tm_time{};
    gmtime_r(&seconds, &tm_time);
    int microseconds = static_cast<int>(time_.microSecondsSinceEpoch()  % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
    stream_ << buf;
}

Logger::Logger(const char*filename,int line)
        :impl_(filename,line){
}

void Logger::Impl::finsh() {
    stream_ << "-" << filename_ << ":" << line_ << "\n";
}

Logger::~Logger(){
    impl_.finsh();
    const Logstream::Buffer& buf(stream().getbuffer());
    F_output(buf.data(),buf.length());
}

void Logger::setOutput(Logger::OutputFunc out) {
    F_output = out ;
}

void Logger::steFlush(Logger::FlushFunc flush) {
    F_flush = flush ;
}

