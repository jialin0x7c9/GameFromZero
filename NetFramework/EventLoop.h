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
	
	void assertInLoopThread()
	{
		if (!isInLoopThread())
		{
			abortNotInLoopThread();
		}
	}
	bool isInLoopThread() const;
	void runInLoop(Functor cb);
	void queueInLoop(Functor cb);
	void wakeup();
	
	static EventLoop *getEventLoopOfCurrentThread();
	static int tid();
	static void cachedTid();
private:
	void abortNotInLoopThread();
	void doPendingFunctors();

private:
    void handleRead();

private:
    typedef std::vector<Channel*> ChannelList;
    bool looping_;
	std::atomic<bool> quit_;
    bool eventHandling_;
	bool callingPendingFunctors_;
    const pid_t threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;
	mutable MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
	Channel *currentActiveChannel_;
}; 










