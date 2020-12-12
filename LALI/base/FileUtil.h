//
// Created by lalalilia on 4/21/20.
//

#ifndef UNTITLED1_FILEUTIL_H
#define UNTITLED1_FILEUTIL_H

#include <cstdio>


class AppendFile{
public:
    explicit AppendFile(const char* filename);
    ~AppendFile();
    void flush();
    void append(const char* msg,int len);
    off_t writtenData() const{return writtenData_;};

private:
    size_t write(const char* msg,int len);
    FILE* fp_;
    char buffer_[64*1024]{};
    off_t writtenData_;

};

#endif //UNTITLED1_FILEUTIL_H
