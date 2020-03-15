#include "PollPoller.h"
#include "Channel.h"
#include <poll.h>
#include <time.h>

PollPoller::PollPoller(EventLoop *loop):Poller(loop)
{

}


PollPoller::~PollPoller() = default;

int PollPoller::poll(int timeoutMs, ChannelList *activeChannelList)
{
    int numOfActiveEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    int savedErrno = errno;
    time_t now = 0;
    time(&now);
    if (numOfActiveEvents > 0)
    {
        fillActiveChannels(numOfActiveEvents, activeChannelList);
    }
    else if (numOfActiveEvents == 0)
    {

    }
    else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
        }
    }
    return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd)
    {
        if (pfd->revents > 0)
        {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            Channel *channel = ch->second;
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::updateChannel(Channel *channel)
{
    if (channel->index() < 0)
    {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size()) - 1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    }
    else
    {
        int idx = channel->index();
        struct pollfd &pfd = pollfds_[idx];
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent())
        {
            pfd.fd = -channel->fd()-1;
        }
    }
}

void PollPoller::removeChannel(Channel *channel)
{
    int idx = channel->index();
    size_t n = channels_.erase(channel->fd());
    if (static_cast<size_t>(idx) == pollfds_.size()-1)
    {
        pollfds_.pop_back();
    }
    else
    {
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
        if (channelAtEnd < 0)
        {
            channelAtEnd = -channelAtEnd-1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}





