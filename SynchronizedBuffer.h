#ifndef _SYNCHRONIZED_BUFFER_H
#define _SYNCHRONIZED_BUFFER_H

#include "pthreadWrappers/Mutex.h"
#include "pthreadWrappers/Semaphore.h"

// assume that only one thread inserts elements and all the rest extract them
template <typename T>
class SyncBuffer {
public:
    SyncBuffer(size_t size = 1024): _size(size), _begin(0), _end(0), _finished(false)
                                , _insertSem(size)
                                , _extractSem(0) { _elems = new T[size]; }
    ~SyncBuffer() { delete[] _elems; }

    void insert(const T& elem) {
        _insertSem.Wait();

        _elems[_end] = elem;
        _end = (_end + 1) % _size;

        _extractSem.Post();
    }

    bool extract(T& elem) {
        _extractSem.Wait();

        _extractMutex.Lock();

        // Don't finish the work until the buffer is empty
        if (_finished && _begin == _end) {
            _extractMutex.Unlock();
            _extractSem.Post();
            return false;
        }

        elem = _elems[_begin];
        _begin = (_begin + 1) % _size;

        _extractMutex.Unlock();

        _insertSem.Post();

        return true;
    }

    void SetFinished() { 
        _finished = true; 
        _extractSem.Post();
    }

private:
    T* _elems;
    size_t _size;

    size_t _begin;
    size_t _end;

    bool _finished;

    pthreadWrapper::Semaphore _insertSem;
    pthreadWrapper::Semaphore _extractSem;
    pthreadWrapper::Mutex _extractMutex;
};

#endif  // _SYNCHRONIZED_BUFFER_H
