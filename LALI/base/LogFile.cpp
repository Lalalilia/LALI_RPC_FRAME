//
//Created by lalalilia
//
#include "LogFile.h"
#include <cstdlib>
#include <memory>
#include <string>
#include "FileUtil.h"
#include "unistd.h"



LogFile::LogFile(const char* filename,
                 off_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkLine )
        : filename_(filename),
          flushInterval_(flushInterval),
          checkLine_(checkLine),
          rollSize_(rollSize),
          mutex_(threadSafe ? new Mutex(nullptr) : nullptr),
          count_(0),
          lastFlushTime_(0),
          lastRollTime_(0),
          startofPeriod_(0)
          {
    rollFile();
}


LogFile::~LogFile() = default;

bool LogFile::rollFile(){
    time_t now = 0;
    std::string OutputFilename = getLogFileName(filename_,&now);

    time_t startofThePeriod = now/daySeconds*daySeconds;
    /// 如果时间要晚于上次滚动时间，就进行滚动，并更新周期开始时间
    /// 每个周期就是一天，开始时间记为这一天的零点；滚动新建一个新文档；
    if(now > lastRollTime_){
        lastRollTime_ = now;
        lastFlushTime_ = now;
        startofPeriod_ = startofThePeriod;
        file_ = std::make_unique<AppendFile>(OutputFilename.c_str());
        return true;
    }
    return false;
}


std::string LogFile::getLogFileName(const char* filename,time_t* now){
    std::string OutputFilename;

    OutputFilename = std::string(filename);

    char timeBuf[64];
    struct tm tm_time{};
    *now = time(nullptr);
    gmtime_r(now,&tm_time);
    strftime(timeBuf,sizeof timeBuf,".%Y-%m-%d-%H:%M:%S",&tm_time);
    OutputFilename += std::string(timeBuf);

    char pidBuf[32];
    snprintf(pidBuf,sizeof pidBuf,".%d",::getpid());
    OutputFilename += std::string(pidBuf);

    OutputFilename += std::string(".log");

    return OutputFilename;

}

void LogFile::append(const char* msg,int len){
    if(mutex_){
        MutexGuard lock(*mutex_);
        append_unlocked( msg, len);
    }
    else{
        append_unlocked( msg, len);
    }
}

void LogFile::append_unlocked(const char* msg,int len){
    file_->append(msg,len);
    if(file_->writtenData() > rollSize_){
        rollFile();
    }
    else{
        ++count_;
        if(count_>=checkLine_){
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t startofThePeriod = now/daySeconds*daySeconds;
            if(startofThePeriod>startofPeriod_){
                rollFile();
            }
            else if(now - lastFlushTime_>=flushInterval_){
                lastFlushTime_ = now;
                file_->flush();
            }
        }
    }
}

void LogFile::flush(){
    if(mutex_){
        MutexGuard lock(*mutex_);
        file_->flush();
    }
    else{
        file_->flush();
    }
}
