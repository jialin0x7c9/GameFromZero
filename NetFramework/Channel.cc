#include "Channel.h"
#include "assert.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN|POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd):
    loop_(loop),
    fd_(fd),
    events_(0),
    curEvents_(0),
    pollIndex_(-1),
    tied_(false),
    eventHandling_(false),
    addToLoop_(false)
{
    
}

Channel::~Channel()
{
//    assert(!eventHandling_);
//    assert(!addToLoop_);
    //must be in loop thread
    //if (loop_->isInLoopThread())
    //{
        //loop must has this channel;
    //    assert(!loop_->hasChannel(this));
    //}
}

int Channel::index()
{
    return pollIndex_;
}

int Channel::events() const
{
    return events_;
}

int Channel::fd() const
{
    return fd_;
}

void Channel::set_revents(int revt)
{
    curEvents_ = revt;
}

void Channel::set_index(int index)
{
    pollIndex_ = index;
}

void Channel::setReadCallback(ReadEventCallback cb)
{
    readCallback_ = std::move(cb);
}

void Channel::setWriteCallback(EventCallback cb)
{
    writeCallback_ = std::move(cb);
}

void Channel::setCloseCallback(EventCallback cb)
{
    closeCallback_ = std::move(cb);
}

void Channel::setErrorCallback(EventCallback cb)
{
    errorCallback_ = std::move(cb);
}

bool Channel::isNoneEvent()
{
    return events_ == kNoneEvent;
}

void Channel::enableReading()
{
    events_ |= kReadEvent;
    update();
}

void Channel::disableReading()
{
    events_ &= ~kReadEvent;
    update();
}

void Channel::enableWriting()
{
    events_  |= kWriteEvent;
    update();
}

void Channel::disableWriting()
{
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll()
{
    events_ = kNoneEvent;
    update();
}



void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    addToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    //must not be in poll;
    assert(isNoneEvent());
    addToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(int timeStamp)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        handleEventWithGuard(timeStamp);
    }
    else
    {
        handleEventWithGuard(timeStamp);
    }
}

void Channel::handleEventWithGuard(int timeStamp)
{
    eventHandling_ = true;
    //peer close 
    if ((curEvents_ & POLLHUP) && !(curEvents_ & POLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if (curEvents_ & POLLNVAL)
    {
        printf("POLLNVAL\n");
    }

    //curEvents has POLLERR or POLLNVAL
    if (curEvents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    if (curEvents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
        {
            readCallback_(timeStamp);
        }
    }

    if (curEvents_ & POLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
    eventHandling_ = false;
}


