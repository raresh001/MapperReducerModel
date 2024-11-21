#include "FileInfo.h"

#include <iostream>

FileInfo::FileInfo(const char* file_name, unsigned int id): id(id) {
    fin = fopen(file_name, "r");

    if (fin == nullptr) {
        throw StdioFileException(std::string("fopen failed for file name ") + file_name);
    }

    if (fseek(fin, 0, SEEK_END) || ((long) (size = ftell(fin))) < 0 || fseek(fin, 0, SEEK_SET)) {
        throw StdioFileException(std::string("Computing size failed for file name ") + file_name);
    }
}

FileInfo::~FileInfo() {
    if (fclose(fin)) {
        std::cerr << "Closing file failed" << std::endl;
    }
}
