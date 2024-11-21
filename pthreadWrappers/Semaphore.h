#ifndef _SEM_WRAPPER_H
#define _SEM_WRAPPER_H

#include <semaphore.h>

namespace pthreadWrapper {

class Semaphore {
public:
    Semaphore(unsigned int value, bool pShared = false);
    ~Semaphore();

    void Wait();
    void Post();
private:
    sem_t _sem;
};

}

#endif  // _SEM_WRAPPER_H
