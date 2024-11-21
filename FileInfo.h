#ifndef _MAPPER_FILE_INFO_H
#define _MAPPER_FILE_INFO_H

#include <cstdio>
#include <exception>
#include <string>

class StdioFileException: public std::exception {
public:
    StdioFileException(const std::string& cause): _cause(cause) {}
    const char* what() const noexcept override { return _cause.c_str(); }

private:
    std::string _cause;
};

class FileInfo {
public:
    struct FileInfoComp {
        bool operator()(FileInfo* first, FileInfo* second) const {
            return first->size > second->size;
        }
    };

    FileInfo(const char* file_name, unsigned int id);
    ~FileInfo();

    unsigned int GetID() const { return id; }
    FILE* GetFile() const { return fin; }
private:
    unsigned int id;
    FILE* fin;
    size_t size;
};

#endif  // _MAPPER_FILE_INFO_H
