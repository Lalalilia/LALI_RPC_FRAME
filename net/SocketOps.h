//
// Created by lalalilia on 5/6/20.
//

#ifndef UNTITLED1_SOCKETOPS_H
#define UNTITLED1_SOCKETOPS_H


#include <arpa/inet.h>
#include <cstdint>
#include <endian.h>

namespace socketOps{


    inline __uint32_t hostTONewwork32(__uint32_t host32){
        return htonl(host32);
    }
    inline __uint16_t hostToNetwork16(__uint16_t host16){
        return htons(host16);
    }

    inline __uint16_t netwrokToHost16(__uint16_t net16){
        return ntohs(net16);
    }

    int creatNonBlocking();

    void bind(int socketfd,const struct sockaddr_in& addr);
    void listen(int socketfd);
    int accept(int socketfd, struct sockaddr_in* addr);
    void close(int socketfd);
    void setReuseAddr(int,bool on);
    int getSocketError(int socketfd);
    void shutdown(int socketfd);
    void setTcpNoDelay(int sockfd);
    struct sockaddr_in getLocalAddr(int sockfd);
    struct sockaddr_in getPeerAddr(int sockfd);

    void toHostPort(char* buf,size_t size,const struct sockaddr_in& addr);
    void fromHostPort(const char* ip,__uint16_t port, struct sockaddr_in* addr);

}


#endif //UNTITLED1_SOCKETOPS_H
