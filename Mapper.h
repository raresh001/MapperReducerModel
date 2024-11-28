#ifndef _MAPPER_THREAD_H
#define _MAPPER_THREAD_H

#include "pthreadWrappers/Thread.h"
#include "pthreadWrappers/Barrier.h"
#include "pthreadWrappers/Mutex.h"

#include "SynchronizedBuffer.h"
#include "FileInfo.h"
#include "AggregatedList.h"

#include <list>
#include <set>

class Mapper: public Thread {
public:
    Mapper(SyncBuffer<FileInfo*>& buff
            , pthreadWrapper::Barrier& barrier
            , AggregatedList& mapperResult
            , pthreadWrapper::Mutex& aggregatedListsMutex)
        : _buff(buff)
            , _barrier(barrier)
            , _mapperResult(mapperResult)
            , _aggregatedListsMutex(aggregatedListsMutex) {}

protected:
    virtual void Run() override;

private:
    SyncBuffer<FileInfo*>& _buff;
    pthreadWrapper::Barrier& _barrier;
    AggregatedList& _mapperResult;
    pthreadWrapper::Mutex& _aggregatedListsMutex;
    AggregatedList _localAggregatedList;

    void parseLine(std::set<std::string>& fileWords, char* line, unsigned int fileid);
};

#endif  // _MAPPER_THREAD_H
