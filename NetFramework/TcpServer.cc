#include "InetAddress.h"
#include "TcpServer.h"
#include <memory>



TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, bool reusePort):
        loop_(loop),
        ipPort_("x"),
        name_(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, reusePort)),
        nextConnId_(1),
        threadPool_(new EventLoopThreadPool(loop, name_))
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
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    loop_->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}


void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    printf("TcpConnection count before RunInLoop=%ld\n", conn.use_count());
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));	
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    printf("TcpConnection count in removeConnection=%ld\n", conn.use_count());
	size_t n = connections_.erase(conn->name());
	loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void TcpServer::start()
{
    printf("TcpServer start\n");
    if (started_.getAndSet(1) == 0)
    {
        printf("TcpServer start after\n");
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}


void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connectionCallback_ = cb;
}


void TcpServer::setThreadNum(int numThreads)
{
    threadPool_->setThreadNum(numThreads);
}





