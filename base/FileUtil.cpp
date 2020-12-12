//
// Created by lalalilia on 4/21/20.
//

#include "FileUtil.h"
#include "string.h"

AppendFile::AppendFile(const char* filename)
        :fp_(::fopen(filename,"ae")),
         writtenData_(0)
{
    ::setbuffer(fp_,buffer_,sizeof buffer_);
}

AppendFile::~AppendFile(){
    ::fclose(fp_);
}

void AppendFile::append(const char* msg,int len){
    size_t n = write(msg,len);
    size_t remain = len-n;
    while(remain>0){
        size_t x=write(msg+n,remain);
        if(x==0){
            int err = ferror(fp_);
            if(err){
                char t_errnobuf[512];
                fprintf(stderr, "AppendFile::append() failed%s\n",strerror_r(err,t_errnobuf,sizeof t_errnobuf) );
            }
            break;
        }
        n+=x;
        remain = len-n;
    }
    writtenData_ += len;
}

size_t AppendFile::write(const char* msg,int len){
    return ::fwrite_unlocked(msg,1,len,fp_);
}

void AppendFile::flush(){
    ::fflush(fp_);
}


