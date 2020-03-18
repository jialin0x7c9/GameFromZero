#include "InetAddress.h"
#include "TcpServer.h"



TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, bool reusePort):
        loop_(loop),
        ipPort_("x"),
        name(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, reusePort))
    {
        acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
		acceptor_->listen();
    }


void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    printf("New Connection\n");
}


void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connectionCallback_ = cb;
}







