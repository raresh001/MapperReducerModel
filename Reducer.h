#ifndef _REDUCER_THREAD_H
#define _REDUCER_THREAD_H

#include "pthreadWrappers/Thread.h"
#include "pthreadWrappers/Barrier.h"
#include "pthreadWrappers/Mutex.h"
#include "AggregatedList.h"

class Reducer: public Thread {
public:
    Reducer(unsigned int reducer_id
            , AggregatedList& mappersResult
            , unsigned int P
            , pthreadWrapper::Barrier& mappersBarrier
            , pthreadWrapper::Barrier& barrier
            , AggregatedReducedList& reducersResult
            , pthreadWrapper::Mutex& reducersResultMutex
            , char& letter)
        : _reducer_id(reducer_id)
        , _mappersResult(mappersResult)
        , _P(P)
        , _mappersBarrier(mappersBarrier)
        , _barrier(barrier)
        , _reducersResult(reducersResult)
        , _reducersResultMutex(reducersResultMutex)
        , _letter(letter) {}

protected:
    virtual void Run() override;

private:
    unsigned int _reducer_id;
    unsigned int _P;
    AggregatedList& _mappersResult;
    pthreadWrapper::Barrier& _mappersBarrier;
    pthreadWrapper::Barrier& _barrier;

    AggregatedReducedList& _reducersResult;
    pthreadWrapper::Mutex& _reducersResultMutex;
    char& _letter;

    AggregatedReducedList _localList;

    /**
     * Parallel sorting, based on merge sort
     * We don't split the problem until the base case (an array with 1 element),
     * but until we have _P elements per thread.
    */
    template < typename T, typename Comparator  = std::less<T> >
    void sortArray(std::vector<T>& array, Comparator c = Comparator());

    template <typename T, typename Comparator>
    static void merge(std::vector<T>& array, size_t first, size_t middle, size_t last, Comparator c);

    template < typename T, typename Comparator  = std::less<T> >
    void shearSort(std::vector<T>& array, Comparator c = Comparator());

    size_t getFirstIndexOf(char firstLetter);
};

#endif  // _REDUCER_THREAD_H
