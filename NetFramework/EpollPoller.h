#pragma once

#include "Poll.h"


class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    int poll(int timeoutMs, ChannelList*activeChannels) override;
    void removeChannel(Channel *channel) override;
    void updateChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;
    void fillActiveChannels(int numEvents, ChannelList *activeChannels)const;
    void update(int operation, Channel *channel);

    typedef std::vector<struct epoll_event> EventList;

    int epollfd_;
    EventList events_;
};
