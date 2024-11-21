#ifndef _PTHREAD_LIB_EXCEPTION_H
#define _PTHREAD_LIB_EXCEPTION_H

#include <sstream>
#include <iostream>
#include <exception>
#include <string>
#include <string.h>

class PthreadLibException : public std::exception {
public:
    PthreadLibException(const std::string& cause): _cause(cause) {}
    const char* what() const noexcept override { return _cause.c_str(); }

private:
    const std::string _cause;
};

#define PRINT_PTHREAD_FUNC_ERR(pthreadFunc, ret)  \
    std::cerr << __FILE__", "                     \
        << __LINE__                               \
        << ": " pthreadFunc" returned error "     \
        << ret                                    \
        << " - "                                  \
        << strerror(errno)

#define PRINT_SEM_FUNC_ERR(func, ret)             \
    std::cerr << __FILE__", "                     \
                << __LINE__                       \
                << ": " func" returned error "    \
                << errno                          \
                << " - "                          \
                << strerror(errno)

#define CHECK_PTHREAD_FUNC_RET(pthreadFunc, ret)  \
    if (ret < 0) {                                \
        std::stringstream s;                      \
        s << __FILE__", "                         \
            << __LINE__                           \
            << ": " pthreadFunc" returned error " \
            << ret                                \
            << " - "                              \
            << strerror(errno);                   \
                                                  \
        throw PthreadLibException(s.str());       \
    }

#define CHECK_SEM_FUNC_RET(func, ret)             \
    if (ret < 0) {                                \
        std::stringstream s;                      \
        s << __FILE__", "                         \
            << __LINE__                           \
            << ": " func" returned error "        \
            << errno                              \
            << " - "                              \
            << strerror(errno);                   \
                                                  \
        throw PthreadLibException(s.str());       \
    }

#endif  // _PTHREAD_LIB_EXCEPTION_H
