#ifndef _MAPPER_THREAD_H
#define _MAPPER_THREAD_H

#include "pthreadWrappers/Thread.h"
#include "pthreadWrappers/Barrier.h"

#include "SynchronizedBuffer.h"
#include "FileInfo.h"
#include "AggregatedList.h"

#include <list>
#include <set>

class Mapper: public Thread {
public:
    Mapper(SyncBuffer<FileInfo*>& buff
            , pthreadWrapper::Barrier& barrier
            , AggregatedList* aggregatedLists
            , pthreadWrapper::Mutex& aggregatedListsMutex)
        : _id(_mapper_id_counter++)
            , _buff(buff)
            , _barrier(barrier)
            , _aggregatedLists(aggregatedLists)
            , _aggregatedListsMutex(aggregatedListsMutex) {}

protected:
    virtual void Run() override;

private:
    static unsigned int _mapper_id_counter;

    // _id is used for debugging exception catches (each mapper has its own error file)
    const unsigned int _id;
    SyncBuffer<FileInfo*>& _buff;
    pthreadWrapper::Barrier& _barrier;
    AggregatedList* _aggregatedLists;
    pthreadWrapper::Mutex& _aggregatedListsMutex;
    AggregatedList _localAggregatedLists['z' - 'a' + 1];

    void parseLine(std::set<std::string>& fileWords, char* line, unsigned int fileid);
};

#endif  // _MAPPER_THREAD_H
