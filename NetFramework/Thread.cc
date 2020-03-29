#include "Thread.h"
#include "CurrentThread.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

AtomicInt32 Thread::numCreated_;

struct ThreadData
{
    typedef std::function<void()> ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;
    ThreadData(ThreadFunc func, const std::string &name, pid_t *tid, CountDownLatch *latch):
        func_(std::move(func)),
        name_(name),
        tid_(tid),
        latch_(latch)
    {

    }


    void runInThread()
    {
        //set Thread class member data:tid_
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;
        func_();
    }
};


Thread::Thread(ThreadFunc func, const std::string &n):
    started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
    int num = numCreated_.increamentAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}


pid_t gettid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}


void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->runInThread();
    delete data;
    return NULL;
}



void Thread::start()
{
    started_ = true;
    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, NULL, startThread, data))
    {
        started_ = false;
        delete data;
    }
    else
    {
        latch_.wait();
    }
}

int Thread::join()
{
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}


void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = gettid();//tid();
    }
}


