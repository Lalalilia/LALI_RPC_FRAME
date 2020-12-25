//
//Created by lalalilia
//

#ifndef ZILIPROJECT_SOCKET_H
#define ZILIPROJECT_SOCKET_H


class AddressOps;

class Socket{
private:
    const int sockfd_;

public:
    explicit Socket(int sockfd)
    :sockfd_(sockfd){

    }

    ~Socket();

    int fd() const {return sockfd_;}

    void bindAddress(const AddressOps& localAddr);

    void listen();

    int accept(AddressOps* peerAddr);

    void shutdownWrite();

    void setReuseAddr(bool on);

    void setKeepAlive(bool on);
};


#endif //ZILIPROJECT_SOCKET_H
