

#include <iostream>
#include "base/Logging.h"
#include "base/AsyncLogging.h"
#include "base/Logstream.h"
#include "net/EventLoop.h"

using namespace std;
off_t rollSize = 500*1000*1000;
AsyncLogging *asyncout = NULL;
char name[128];

void Asyncoutput(const char* msg,int len){
    asyncout->append(msg,len);
}

void bench(){
    Logger::setOutput(Asyncoutput);
    int count = 0;
    for(int i=0;i<100;i++){
        LOG << "hello" << count;
        cout << "hello" << count;
        count++;
    }
}


int main(int argc,char* argv[]){

    
    strncpy(name,argv[0],sizeof name);
    AsyncLogging logger(name,rollSize);
    logger.start();
    asyncout = &logger;
    bench();
    sleep(1);
    

}


