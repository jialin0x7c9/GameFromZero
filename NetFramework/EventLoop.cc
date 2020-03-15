#include "EventLoop.h"


EventLoop::EventLoop():
    looping_(false),
    eventHandling_(false),
    threadId_(CurrentThread::tid()),
    poller_(Poller::newDefaultPoller(this)),
    currentActiveChannel_(NULL)
{

}

EventLoop::~EventLoop()
{

}


void EventLoop::loop()
{
    looping_ = true;
    while(!quit_)
    {
        activeChannels_.clear();
        int pollReturnTime = poller_->poll(1000, &activeChannels_);
        eventHandling_ = true;
        for (Channel *channel : activeChannels_)
        {
            currentActiveChannel_  = channel;
            currentActiveChannel_->handleEvent(pollReturnTime);
        }
        currentActiveChannel_ = NULL;
        eventHandling_ = false;
    }
}

void EventLoop::updateChannel(Channel * channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}


