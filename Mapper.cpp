#include "Mapper.h"
#include <fstream>

using namespace std;

unsigned int Mapper::_mapper_id_counter = 0;

void Mapper::Run() {
    try {
        FileInfo* info;
        while (_buff.extract(info)) {
            set<string> fileWords;
            char line[1024];

            while (fgets(line, sizeof(line), info->GetFile())) {
                parseLine(fileWords, line, info->GetID());
            }

            delete info;
        }

        _aggregatedListsMutex.Lock();
        for (int iter = 0; iter < sizeof(_localAggregatedLists) / sizeof(*_localAggregatedLists); iter++) {
            _aggregatedLists[iter].splice(_aggregatedLists[iter].begin(), _localAggregatedLists[iter]);
        }
        _aggregatedListsMutex.Unlock();

        _barrier.Wait();
    } catch (PthreadLibException& e) {
        ofstream("Mapper_" + to_string(_id) + ".err") << "Mapper::Run: PthreadLibException occured: " << e.what();
    } catch (exception& e) {
        ofstream("Mapper_" + to_string(_id) + ".err") << "Exception occured: " << e.what();
    } catch (...) {
        ofstream("Mapper_" + to_string(_id) + ".err") << "Unknown exception occured";
    }
}

void Mapper::parseLine(set<string>& fileWords, char* line, unsigned int fileid) {
    char* word_end = line;

    for (char* line_iter = line; *line_iter; line_iter++) {
        char last_char = *line_iter;

        if (last_char == ' ' || last_char == '\t' || last_char == '\n') {
            *word_end = '\0';
            string word = line;

            if (fileWords.insert(word).second) {
                _localAggregatedLists[word[0] - 'a']
                    .push_back(std::pair<std::string, unsigned int>(word, fileid));
            }

            line = ++word_end;
        } else if (last_char >= 'A' && last_char <= 'Z') {
            *(word_end++) = tolower(last_char);
        } else if ( (last_char >= 'a' && last_char <= 'z') || (last_char >= '0' && last_char <= '9') ) {
            *(word_end++) = last_char;
        }
    }

    if (word_end != line) { // last word is terminated by '\0'
        *word_end = '\0';
        string word = line;

        if (fileWords.insert(word).second) {
            _localAggregatedLists[word[0] - 'a']
                .push_back(std::pair<std::string, unsigned int>(word, fileid));
        }
    }
}
