#include "Mutex.h"

using namespace pthreadWrapper;

Mutex::Mutex(const pthread_mutexattr_t *attr) {
    int ret = pthread_mutex_init(&_mutex, attr);
    CHECK_PTHREAD_FUNC_RET("pthread_mutex_init", ret);
}

Mutex::~Mutex() {
    int ret = pthread_mutex_destroy(&_mutex);

    if (ret) {
        PRINT_PTHREAD_FUNC_ERR("pthread_mutex_destroy", ret);
    }
}

void Mutex::Lock() {
    int ret = pthread_mutex_lock(&_mutex);
    CHECK_PTHREAD_FUNC_RET("pthread_mutex_lock", ret);
}

void Mutex::Unlock() {
    int ret = pthread_mutex_unlock(&_mutex);
    CHECK_PTHREAD_FUNC_RET("pthread_mutex_unlock", ret);
}