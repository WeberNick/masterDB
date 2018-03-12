#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>

void printError(const std::string aErrMsg)
{
    std::cerr << "# ERROR: " << aErrMsg << std::endl;
}

void printError(const std::string aErrMsg, const int aErrNo)
{
    std::cerr << "# ERROR: " << aErrMsg << ": " << std::strerror(aErrNo) << std::endl;
}
