#pragma once

#include <functional>
#include "Channel.h"
#include "Socket.h"
#include <sys/socket.h>

class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb);
    bool listenning() const;
    void listen();

    static int createNonblockingOrDie(sa_family_t family);
private:
    void handleRead();
    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;
};
