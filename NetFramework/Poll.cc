#include "Poll.h"
#include "Channel.h"
#include <stdlib.h>

Poller::Poller(EventLoop *loop):ownerLoop_(loop)
{

}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel *channel) const
{
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (getenv("USE_POLL"))
    {
        return new PollPoller(loop);
    }
    else
    {
        return new EPollPoller(loop);
    }
}



