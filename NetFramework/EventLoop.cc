#include "EventLoop.h"
#include "Poll.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/eventfd.h>

//线程变量
__thread EventLoop *t_loopInThisThread = 0;
__thread int t_cachedTid = 0;

    

int EventLoop::tid()
{
	if (__builtin_expect(t_cachedTid==0, 0))
	{
		cachedTid();
	}
	return t_cachedTid;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::cachedTid()
{
	if (t_cachedTid == 0)
	{
		t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
	}
}

int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
	if (evtfd < 0)
	{
		
	}
	return evtfd;
}

EventLoop::EventLoop():
    looping_(false),
    eventHandling_(false),
	callingPendingFunctors_(false),
    threadId_(EventLoop::tid()),
    poller_(Poller::newDefaultPoller(this)),
	wakeupFd_(createEventfd()),
	wakeupChannel_(new Channel(this, wakeupFd_)),
    currentActiveChannel_(NULL)
{
	//一个线程只能有一个EventLoop对象的判断
	if (t_loopInThisThread)
	{
		//退出
	}
	else
	{
		t_loopInThisThread = this;
	}
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread = NULL;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}


void EventLoop::loop()
{
    looping_ = true;
    while(1)
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
		doPendingFunctors();
    }
}

void EventLoop::runInLoop(Functor cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(std::move(cb));
	}
}

void EventLoop::queueInLoop(Functor cb)
{
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(std::move(cb));
	}
	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof one);
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof one);
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for (const Functor &functor : functors)
	{
		functor();
	}
	callingPendingFunctors_ = false;
}


void EventLoop::updateChannel(Channel * channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
	
}





