#ifndef _BARRIER_WRAPPER_H
#define _BARRIER_WRAPPER_H

#include <pthread.h>

namespace pthreadWrapper {

class Barrier {
public:
    Barrier(unsigned int count = 0) { if (count) Init(count); }
    ~Barrier();

    void Init(unsigned int count);
    void Reset(unsigned int count);
    void Wait();
private:
    pthread_barrier_t _barrier;
};

}

#endif  // _BARRIER_WRAPPER_H
