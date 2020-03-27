#pragma once
#include "noncopyable.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include <memory>
#include <string.h>
#include <map>
#include "TcpConnection.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class TcpServer : noncopyable
{
public:

    TcpServer(EventLoop *loop, const InetAddress &listenAddr,  const std::string &nameArg, bool reusePort = false);

    void setConnectionCallback(const ConnectionCallback &cb);
	void setMessageCallback(const MessageCallback &cb)
	{
		messageCallback_ = cb;
	}

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
	void removeConnectionInLoop(const TcpConnectionPtr &conn);


private:
    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;


    int nextConnId_;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
    ConnectionMap connections_;
};



