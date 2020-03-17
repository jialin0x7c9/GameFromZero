#include "InetAddress.h"
#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(std::string ip, unsigned short port)
{
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htobe16(port);
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}


sa_family_t InetAddress::family()const
{
    return addr_.sin_family;
}


