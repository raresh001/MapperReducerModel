#ifndef _MUTEX_WRAPPER_H
#define _MUTEX_WRAPPER_H

#include "PthreadException.h"
#include <pthread.h>

namespace pthreadWrapper {

class Mutex {
public:
    Mutex(const pthread_mutexattr_t *attr = nullptr);
    ~Mutex();

    void Lock();
    void Unlock();
private:
    pthread_mutex_t _mutex;
};

}

#endif  // _MUTEX_WRAPPER_H
