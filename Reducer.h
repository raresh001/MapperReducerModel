#ifndef _REDUCER_THREAD_H
#define _REDUCER_THREAD_H

#include "pthreadWrappers/Thread.h"

class Reducer: public Thread {
protected:
    virtual void Run() override {}
};

#endif  // _REDUCER_THREAD_H
