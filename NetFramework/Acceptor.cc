#include "Acceptor.h"
#include "InetAddress.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &cb)
{
    newConnectionCallback_ = cb;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport):
    loop_(loop),
    acceptSocket_(Acceptor::createNonblockingOrDie(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listenning_(false),
    idleFd_(::open("/dev/null", O_RDONLY|O_CLOEXEC))
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

int Acceptor::createNonblockingOrDie(sa_family_t family)
{
    int socketfd = ::socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);
    return socketfd;
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    close(idleFd_);
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (newConnectionCallback_)
        {
            newConnectionCallback_(connfd, peerAddr);
        }
        else
        {
            close(connfd);
        }
    }
    else
    {
        if (errno == EMFILE)
        {
            close(idleFd_);
            idleFd_ = accept(acceptSocket_.fd(), NULL, NULL);
            close(idleFd_);
            idleFd_ = open("/dev/null", O_RDONLY|O_CLOEXEC);
        }
    }
}



