#include "InetAddress.h"
#include "TcpServer.h"



TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, bool reusePort):
        loop_(loop),
        ipPort_("x"),
        name(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, reusePort)),
        nextConnId_(1)
    {
        acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
		acceptor_->listen();
    }


void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    loop_->assertInLoopThread();
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof localAddr);
    ::getsockname(sockfd, static_cast<struct sockaddr*>(static_cast<void*>(&localAddr)), &addrlen);
    InetAddress localAddrInet(localAddr);

    //Get TcpConnectio a name;
    char buf[64];
    snprintf(buf, sizeof(buf), "NewConn:%d", nextConnId_);
    ++nextConnId_;
    std::string connName = buf;
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddrInet, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    //conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    loop_->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}


void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{

}


void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connectionCallback_ = cb;
}







