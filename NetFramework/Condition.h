#pragma once
#include "noncopyable.h"
#include "Mutex.h"
#include <pthread.h>

class Condition : noncopyable
{
public:
    explicit Condition(MutexLock &mutex):mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }
private:
    MutexLock &mutex_;
    pthread_cond_t pcond_;
};
