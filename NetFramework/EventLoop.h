#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include "Channel.h"

class Poller;
class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    void hasChannel(Channel *channel);

private:
    void handleRead();

private:
    typedef std::vector<Channel*> ChannelList;
    bool looping_;
    bool eventHandling_;
    const pid_t threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;
    std::atomic<bool> quit_;
}; 
