#include "TcpConnection.h"
#include "Channel.h"
#include "Socket.h"

using std::placeholders::_1;

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr):
    loop_(loop),
    name_(name),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    highWaterMark_(64*1024*1024)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    socket_->setKeepAlive(true);
}


TcpConnection::~TcpConnection()
{

}

void TcpConnection::send(Buffer *buf)
{
    if (state_ == kConnected)
    {
        sendInLoop(buf->peek(), buf->readableBytes());
        buf->retrieveAll();
    }
    else
    {

    }
}
