#include "Socket.h"
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

Socket::~Socket()
{
    close(sockfd_);
}

int Socket::fd() const
{
    return sockfd_;
}


void Socket::bindAddress(const InetAddress &addr)
{
    int ret = bind(sockfd_, addr.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

void Socket::listen()
{
    ::listen(sockfd_, SOMAXCONN);
}

int Socket::accept(InetAddress *peeraddr)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
    int connfd = ::accept(sockfd_, static_cast<struct sockaddr*>(static_cast<void*>(&addr)), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
    if (connfd < 0)
    {
        int savedErrno = errno;
        switch (savedErrno)
        {

        }
    }
    return connfd;
}

void Socket::setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {

    }
}

void Socket::setTcpnoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));

}


void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}












