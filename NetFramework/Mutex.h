#include "noncopyable.h"


class MutexLock : noncopyable
{
public:
	MutexLock():holder_(0)
	{
		pthread_mutex_init(&mutex_, NULL);
	}
	
	~MutexLock()
	{
		pthread_mutex_destroy(&mutex_);
	}
	void lock()
	{
		pthread_mutex_lock(&mutex_);
	}
	
	void unlock()
	{
		pthread_mutex_unlock(&mutex_);
	}
private:
	pthread_mutex_t mutex_;
};


class MutexLockGuard : noncopyable
{
public:
	explicit MutexLock(MutexLock &mutex):mutex_(mutex)
	{
		mutex_.lock();
	}
private:
	MutexLock &mutex_;
};