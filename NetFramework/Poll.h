#pragma once

#include "noncopyable.h"
#include <map>
#include <vector>

class Channel;
class EventLoop;

class Poller : noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop *loop);
    virtual ~Poller();
    virtual int poll(int timeoutMs, ChannelList *activeChannel) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;
    virtual bool hasChannel(Channel *channel) const;
    static Poller *newDefaultPoller(EventLoop *loop);


protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;

};
