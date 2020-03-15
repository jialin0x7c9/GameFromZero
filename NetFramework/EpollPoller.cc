#include "EpollPoller.h"
#include "Channel.h"
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>


const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop):
    Poller(loop),
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{

}


EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

int EpollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    time_t now = 0;
    time(&now);
    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size()*2);
        }
    }
    else if (numEvents == 0)
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

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        //update existing fd;
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::update(int operation, Channel *channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {

    }
}


void EpollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    int index = channel->index();
    channels_.erase(fd);
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}










