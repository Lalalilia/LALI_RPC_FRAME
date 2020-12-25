//
//Created by lalalilia
//

#ifndef UNTITLED1_ADDRESSOPS_H
#define UNTITLED1_ADDRESSOPS_H


#include <cstdint>
#include <string>
#include <netinet/in.h>

class AddressOps{

public:
    explicit AddressOps(uint16_t port);

    AddressOps(const std::string& ip,uint16_t port);

    AddressOps(const struct sockaddr_in& addr)
            :addr_(addr){

    }
    const struct sockaddr_in& getSockAddr() const{
        return addr_;
    }

    void setSockAddr(const struct sockaddr_in& addr){addr_ = addr;}
    void setAddrClient_(const struct sockaddr_in& addr){addrClient_ = addr;}
    std::string toHostPort () const;
private:

    struct sockaddr_in addr_;
    struct sockaddr_in addrClient_;


};

#endif //UNTITLED1_ADDRESSOPS_H
