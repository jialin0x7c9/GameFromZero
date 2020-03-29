#pragma once

extern __thread int t_cachedTid;
extern __thread const char * t_threadName;



namespace CurrentThread
{

void cacheTid();

inline int tid()
{
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
        cacheTid();
    }
    return t_cachedTid;
}


inline const char* name()
{
    return t_threadName;
}

}


