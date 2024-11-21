#include "Barrier.h"
#include "PthreadException.h"

using namespace pthreadWrapper;

void Barrier::Init(unsigned int count) {
    int ret = pthread_barrier_init(&_barrier, nullptr, count);
    CHECK_PTHREAD_FUNC_RET("pthread_barrier_init", ret);
}

void Barrier::Reset(unsigned int count) {
    int ret = pthread_barrier_destroy(&_barrier);
    CHECK_PTHREAD_FUNC_RET("pthread_barrier_destory", ret);
    ret = pthread_barrier_init(&_barrier, nullptr, count);
    CHECK_PTHREAD_FUNC_RET("pthread_barrier_init", ret);
}

Barrier::~Barrier() {
    int ret = pthread_barrier_destroy(&_barrier);

    if (ret) {
        PRINT_PTHREAD_FUNC_ERR("pthread_barrier_destroy", ret);
    }
}

void Barrier::Wait() {
    int ret = pthread_barrier_wait(&_barrier);
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD) {
        std::stringstream s;
        s << __FILE__", "
            << __LINE__
            << ": pthread_barrier_wait returned error "
            << ret
            << " - "
            << strerror(ret);
        throw PthreadLibException(s.str()); 
    }
}
