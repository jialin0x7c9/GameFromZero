#pragma once
#include "Poll.h"

class EventLoop;
class PollPoller : public Poller
{
public:
    PollPoller(EventLoop *loop);
    ~PollPoller() override;

    int poll(int timeoutMs, ChannelList *activeChannel) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

private:
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
};
