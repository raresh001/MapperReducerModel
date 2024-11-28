#ifndef _THREAD_INTERFACE_H
#define _THREAD_INTERFACE_H

#include <pthread.h>
#include "PthreadException.h"

class Thread {
public:
    Thread(): _active(false) {}
    virtual ~Thread();

    void Start();
    void Join();

protected:
    pthread_t _id;
    virtual void Run() = 0;

private:
    bool _active;

    // this is the actual function for pthread_create
    static void* worker(void* arg) {
        static_cast<Thread*>(arg)->Run();
        pthread_exit(nullptr);
    }
};

#endif  // _THREAD_INTERFACE_H
