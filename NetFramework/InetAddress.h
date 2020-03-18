#pragma once
#include <netinet/in.h>
#include <string>

class InetAddress
{
public:
    InetAddress(std::string ip, unsigned short port);
    InetAddress() = default;
    sa_family_t family() const;

    const struct sockaddr *getSockAddr() const;
private:
    struct sockaddr_in addr_;
};


