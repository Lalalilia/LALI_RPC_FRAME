//
//Created by lalalilia
//

#ifndef UNTITLED1_LOGFILE_H
#define UNTITLED1_LOGFILE_H

#include <bits/unique_ptr.h>
#include "Mutex.h"
#include "time.h"
#include "FileUtil.h"

class LogFile{
public:

    LogFile(const char* filename,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3 ,

            int checkLine = 1024);
    ~LogFile();
    void append(const char* msg,int len);
    void flush();
    bool rollFile();

private:

    static std::string getLogFileName(const char* filename,time_t* now);
    void append_unlocked(const char* msg,int len);
    const char* filename_;
    const int flushInterval_;
    const int checkLine_;

    /// 用作日志滚动与刷新
    time_t lastRollTime_;
    time_t lastFlushTime_;
    time_t startofPeriod_;

    std::unique_ptr<Mutex> mutex_;
    off_t rollSize_;
    int count_;

    const static int daySeconds = 60*60*24;

    std::unique_ptr<AppendFile> file_;

};


#endif //UNTITLED1_LOGFILE_H
