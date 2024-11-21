#ifndef _PROGRAM_APPLICATION_H
#define _PROGRAM_APPLICATION_H

#include "pthreadWrappers/Thread.h"
#include "pthreadWrappers/Barrier.h"

#include "SynchronizedBuffer.h"
#include "FileInfo.h"
#include "AggregatedList.h"

#include <fstream>
#include <vector>
#include <list>

class Application {
public:
    Application()
        : threads(nullptr), test_file(nullptr), M(0), P(0) {}
    ~Application();

    bool Init(int argc, char* argv[]);
    void Run();
private:
    Thread** threads;    

    // synchronization tools for mappers
    SyncBuffer<FileInfo*> mappersBuff;
    AggregatedList mappersResult['z' - 'a' + 1];
    pthreadWrapper::Mutex mappersResultMutex;
    pthreadWrapper::Barrier barrier;

    // synchronization tools for reducers

    const char* test_file;
    int M;
    int P;

    bool readTestFile();
};

#endif  // _PROGRAM_APPLICATION_H
