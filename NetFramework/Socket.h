#pragma once

#include "noncopyable.h"

class InetAddress;

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd):
        sockfd_(sockfd)
    {

    }

    ~Socket();

    int fd() const;
    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);
    void shutdownWrite();
    void setTcpnoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

    void setNonBlockAndCloseOnExec(int sockfd);
private:
    const int sockfd_;
};
