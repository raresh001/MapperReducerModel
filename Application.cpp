#include "Application.h"
#include "Mapper.h"
#include "Reducer.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
using namespace pthreadWrapper;

Application::~Application() { 
    for (int iter = 0; iter < M + P; iter++) {
        delete threads[iter];
    }
    delete[] threads;
}

bool Application::Init(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "ERROR: Wrong arguments; expected"
            << argv[0]
            << " <M> <P> <test_file>"
            << endl;
        return false;
    }

    try {
        M = stoi(argv[1]);
        P = stoi(argv[2]);

        if (M <= 0 || P <= 0) {
            throw invalid_argument("M and P must be BOTH positive");
        }
    } catch (invalid_argument& e) {
        cerr << "Exception caugth in App::Init(): " << e.what() << endl;
        return false;
    }

    test_file = argv[3];

    mappersBarrier.Init(P + M + 1);
    reducersBarrier.Init(P);
    threads = new Thread*[M + P];

    int iter = 0;
    for (; iter < M; iter++) {
        threads[iter] = new Mapper(mappersBuff, mappersBarrier, mappersResult, mappersResultMutex);
    }

    for (; iter < P + M; iter++) {
        threads[iter] = new Reducer(iter - M
                                    , mappersResult
                                    , P
                                    , mappersBarrier
                                    , reducersBarrier
                                    , reducersResult
                                    , reducersResultMutex
                                    , letter);
    }

    return true;
}

void Application::Run() {
    try {
        int iter = 0;

        for (iter = 0; iter < P + M; iter++) {
            threads[iter]->Start();
        }

        readTestFile();

        mappersBuff.SetFinished();
        mappersBarrier.Wait();

        for (iter = 0; iter < P + M; iter++) {
            threads[iter]->Join();
        }

        for (auto p : mappersResult) {
            cout << "(" << p.first << " " << p.second << ")\n";
        }

        cout << "\n\n\n\n\n";

        int i = 0;
        for (auto p : reducersResult) {
            cout << i++ << ": (" << p.first << ", [";
            for (auto x : p.second) {
                cout << x << " ";
            }

            cout << "])\n";
        }
    } catch (PthreadLibException& e) {
        cerr << "App::Run: PthreadLibException occured: " << e.what() << endl;
    } catch (exception& e) {
        cerr << "Exception occured: " << e.what() << endl;
    } catch (...) {
        cerr << "Unknown exception occured" << endl;
    }
}

bool Application::readTestFile() {
    ifstream test_in(test_file);
    string filename;

    vector<FileInfo*> fileInfos;

    if (!test_in) {
        cerr << "Error at opening test file " << test_file << endl;
        return false;
    }

    unsigned int id = 1;
    while (getline(test_in, filename)) {
        try {
            fileInfos.push_back(new FileInfo(filename.c_str(), id++));
        } catch (StdioFileException& e) {
            cerr << "Error in FileInfo::FileInfo: " << e.what() << endl;
        }
    }

    // sort files decreasingly by their sizes. This improves the allocation of tasks to mappers
    sort(fileInfos.begin(), fileInfos.end(), FileInfo::FileInfoComp());

    for (FileInfo* file : fileInfos) {
        mappersBuff.insert(file);
    }

    return true;
}
