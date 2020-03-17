#pragma once
#include "noncopyable.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include <memory>
#include <string.h>
#include <map>

class TcpServer : noncopyable
{
public:
    typedef std::function<void()> ConnectionCallback;

    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &listenAddr, const std::string &nameArg, bool reusePort = false):
        loop_(loop),
        ipPort(listenAddr.toIpPort()),
        name(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, reusePort))
    {
        acceptor_->setNewConnectionCallback(connectionCallback_);
    }

    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectionCallback_ = cb;
    }

private:
    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
};



