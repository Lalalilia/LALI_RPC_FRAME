//
//Created by lalalilia
//

#include "AddressOps.h"
#include <strings.h>
#include "SocketOps.h"
#include "../base/Logging.h"

AddressOps::AddressOps(uint16_t port) {
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = socketOps::hostTONewwork32(INADDR_ANY);
    addr_.sin_port = socketOps::hostToNetwork16(port);
}

AddressOps::AddressOps(const std::string &ip, uint16_t port) {
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = socketOps::hostToNetwork16(port);
    if(::inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr)<0){
        LOG<<" AddressOps inet_pton error";
    }
}

std::string AddressOps::toHostPort() const {
    char buf[32];
    socketOps::toHostPort(buf,sizeof buf,addrClient_);
    return buf;
}

