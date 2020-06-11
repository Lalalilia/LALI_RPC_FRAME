//
// Created by lalalilia on 4/21/20.
//

#include "AsyncLogging.h"

#include <memory>
#include "Timestamp.h"
#include "LogFile.h"

AsyncLogging::AsyncLogging(const char* filename,off_t rollSize,int flushInterval  )
        :filename_(filename),
         flushInterval_(flushInterval),
         rollSize_(rollSize),
         running_(false),
         thread_(std::bind(&AsyncLogging::threadFunc,this),"AsyncLogging"),
         mutex_(nullptr),
         latch_(1),
         cond_(mutex_),
         currentBuffer_(new Buffer),
         nextBuffer_(new Buffer),
         buffers_(){
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* msg,int len){
    MutexGuard lock(mutex_);
    if(currentBuffer_->avail()>len){
        currentBuffer_->append(msg,len);
    }
    else{
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }
        else{
            currentBuffer_  = std::make_unique<Buffer>();
        }
        currentBuffer_->append(msg,len);
        cond_.weakup();
    }
}


void AsyncLogging::threadFunc(){
    assert(running_==true);
    latch_.countDown();
    LogFile output(filename_,rollSize_,false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferptrVector BufferToWrite;
    BufferToWrite.reserve(16);
    while(running_){
        {
            MutexGuard lock(mutex_);
            if(buffers_.empty()){
                cond_.timedWait(flushInterval_);
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            BufferToWrite.swap(buffers_);
            if(!nextBuffer_){
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        if(BufferToWrite.size()>25){
            char buf[256];
            snprintf(buf,sizeof buf,"Destroyed log messafe at %s , SIZE: %z \n",
                     Timestamp::now().toFormattedString(true).c_str(),
                     BufferToWrite.size()-2);
            fputs(buf,stderr);
            output.append(buf,static_cast<int>(strlen(buf)));
            BufferToWrite.erase(BufferToWrite.begin()+2,BufferToWrite.end());
        }

        for(const auto& buffer:BufferToWrite){
            output.append(buffer->data(),buffer->length());
        }

        if(BufferToWrite.size()>2){
            BufferToWrite.resize(2);
        }



        if(!newBuffer1){
            newBuffer1 = std::move(BufferToWrite.back());
            BufferToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2){
            newBuffer2 = std::move(BufferToWrite.back());
            BufferToWrite.pop_back();
            newBuffer2->reset();
        }
        BufferToWrite.clear();
        output.flush();
    }
    output.flush();
}

