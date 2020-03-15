#pragma once
#include "noncopyable.h"
#include <memory>
#include <functional>
class EventLoop;
class Channel : noncopyable
{
public:
    Channel(EventLoop *loop, int fd);
    ~Channel();
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(int)> ReadEventCallback;
    void handleEvent(int timeStamp);
    void handleEventWithGuard(int timeStamp);
    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);
    void tie(const std::shared_ptr<void>&);
    int fd() const;
    int events() const;
    void set_revents(int revt);
    bool isNoneEvent();
    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();
    bool isWriting()const;
    bool isReading()const;
    int index();
    void set_index(int idx);
    void update();
    void remove();


public:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

private:
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
    EventLoop *loop_;
    const int fd_;
    int events_;
    int curEvents_;
    int pollIndex_;
    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;
    bool addToLoop_;
};

