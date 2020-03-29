#pragma once

#include "noncopyable.h"
#include <stdint.h>

template<typename T>
class AtomicIntegerT: noncopyable
{
public:
    AtomicIntegerT():value_(0)
    {

    }

    T get()
    {
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndSet(T newValue)
    {
        return __sync_lock_test_and_set(&value_, newValue);
    }

    T getAndAdd(T x)
    {
        return __sync_fetch_and_add(&value_, x);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T increamentAndGet()
    {
        return addAndGet(1);
    }

    void increment()
    {
        increamentAndGet();
    }


private:
    volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;
