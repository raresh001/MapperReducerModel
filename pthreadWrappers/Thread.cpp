#include "Thread.h"
#include <iostream>

Thread::~Thread() {
    if (_active) {
        std::cerr << "CAREFUL: ~Thread is called before Join() for thread with id " 
                    << _id 
                    << std::endl;
        if (int ret = pthread_join(_id, nullptr))
            PRINT_PTHREAD_FUNC_ERR("pthread_join", ret);
    }
}

void Thread::Start() {
    if (_active) {
        std::cerr << "CAREFUL: Start() has been called more than once for thread with id " 
                    << _id 
                    << std::endl;
    } else {
        CHECK_PTHREAD_FUNC_RET("pthread_create", pthread_create(&_id, nullptr, worker, this));
        _active = true;
    }
}

void Thread::Join() {
    if (!_active) {
        std::cerr << "CAREFUL: Join() has been called on an inactive thread with id " 
                    << _id 
                    << std::endl;
    } else {
        CHECK_PTHREAD_FUNC_RET("pthread_join", pthread_join(_id, nullptr));
        _active = false;
    }
}
