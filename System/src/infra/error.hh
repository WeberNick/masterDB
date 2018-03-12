#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>

inline void printErr(const std::string aErrMsg)
{
    std::cerr << "# ERROR: " << aErrMsg << std::endl;
}

inline void printErr(const std::string aErrMsg, const int aErrNo)
{
    std::cerr << "# ERROR: " << aErrMsg << ": " << std::strerror(aErrNo) << std::endl;
}
