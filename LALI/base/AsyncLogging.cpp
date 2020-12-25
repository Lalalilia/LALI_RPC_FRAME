//
//Created by lalalilia
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
    //不够就就使用move将next的空间给cur
    else{
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }
        //如果文件实在太多了，预备空间都被填满了，只能再申请一块空间。
        else{
            currentBuffer_  = std::make_unique<Buffer>();
        }
        currentBuffer_->append(msg,len);
        cond_.weakup(); //唤醒后台写入文件到线程开始写入。
    }
}


// 后端写入线程
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
        //在临界区内完成数据到交换，将cur next中到数据都转移到btw缓冲中，然后将buffer1和buffer2的空间给到cur和next
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
        // 使用btw对buffer1和buffer2进行空间填充；
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

