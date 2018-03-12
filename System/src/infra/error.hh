#ifndef ERROR_HH
#define ERROR_HH

#include <cerrno>
#include <cstring>
#include <iostream>

void printErr(const std::string aErrMsg)
{
    std::cerr << "# ERROR: " << aErrMsg << std::endl;
}

void printErr(const std::string aErrMsg, const int aErrNo)
{
    std::cerr << "# ERROR: " << aErrMsg << ": " << std::strerror(aErrNo) << std::endl;
}

#endif
