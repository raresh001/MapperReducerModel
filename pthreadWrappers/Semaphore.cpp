#include "Semaphore.h"
#include "PthreadException.h"

using namespace pthreadWrapper;

Semaphore::Semaphore(unsigned int value, bool pShared) {
    int ret = sem_init(&_sem, pShared, value);
    CHECK_SEM_FUNC_RET("sem_init", ret);
}

Semaphore::~Semaphore() {
    int ret = sem_destroy(&_sem);

    if (ret) {
        PRINT_SEM_FUNC_ERR("sem_destroy", ret);
    }
}

void Semaphore::Wait() {
    int ret = sem_wait(&_sem);
    CHECK_SEM_FUNC_RET("sem_wait", ret);
}

void Semaphore::Post() {
    int ret = sem_post(&_sem);
    CHECK_SEM_FUNC_RET("sem_post", ret);
}