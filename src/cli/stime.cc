#include "stime.hh"

STime::STime(const std::string& x, char sep) {
    const char* s = x.c_str();
    set(s, sep);
}

STime::STime(const char* x, char sep) {
    const char* s = x;
    set(s, sep);
}

bool STime::set(const char*& x, char sep) {
    char* lEnd = 0;
    int v = 0;
    v = strtol(x, &lEnd, 10);
    if (x == lEnd) {
        std::cerr << "I011 STime::set " << *x << std::endl;
        return false;
    }
    _val = v * 3600;
    x = lEnd;
    if (sep != *x) {
        std::cerr << "I012 STime::set " << *x << std::endl;
        return false;
    }
    ++x;
    v = strtol(x, &lEnd, 10);
    if (x == lEnd) {
        std::cerr << "I021 STime::set " << *x << std::endl;
        return false;
    }
    _val += v * 60;
    x = lEnd;
    if (sep == *x) {
        ++x;
        v = strtol(x, &lEnd, 10);
        if (x == lEnd) {
            std::cerr << "I031 STime::set " << *x << std::endl;
            return false;
        }
        _val += v;
        x = lEnd;
    }
    return true;
}

std::ostream& STime::print(std::ostream& os, char sep) const {
    os << hour() << sep << minute(); //  << sep << second();
    const int lSec = second();
    if (0 != lSec) { os << sep << second(); }
    return os;
}
