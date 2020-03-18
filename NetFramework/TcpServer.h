#pragma once
#include "noncopyable.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include <memory>
#include <string.h>
#include <map>

using std::placeholders::_1;
using std::placeholders::_2;

class TcpServer : noncopyable
{
public:
    typedef std::function<void()> ConnectionCallback;

    TcpServer(EventLoop *loop, const InetAddress &listenAddr,  const std::string &nameArg, bool reusePort = false);

    void setConnectionCallback(const ConnectionCallback &cb);

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);


private:
    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
};



