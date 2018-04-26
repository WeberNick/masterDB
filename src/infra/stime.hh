#ifndef INFRA_STIME_HH
#define INFRA_STIME_HH

#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <string>


// time in seconds from start of the day

class STime {
  public:
    STime() : _val() {}
    STime(int x) : _val(x) {}
    STime(int h, int m, int s) : _val(((h*60) + m) * 60 + s) {}
    STime(const std::string&, char sep = ':');
    STime(const char*, char sep = ':');
  public:
    inline int val() const { return _val; }
    inline int hour() const { return (val() / 3600); }
    inline int minute() const { return ((val() % 3600) / 60); }
    inline int second() const { return (val() % 60); } 
  public:
    inline bool operator==(const STime& x) const { return (_val == x._val); }
    inline bool operator<(const STime& x) const { return (_val < x._val); }
    inline bool operator<=(const STime& x) const { return (_val <= x._val); }
    inline bool operator>(const STime& x) const { return (_val > x._val); }
    inline bool operator>=(const STime& x) const { return (_val >= x._val); }
  public:
    inline void resetMinSec() { _val = (_val / 3600) * 3600; }
  public:
    inline unsigned int hash() const { return _val; }
  public:
    std::ostream& print(std::ostream&, char sep = ':') const;
  public:
    inline void set(int x) { _val = x; }
    inline void set(int h, int m, int s) { _val = (((h*60) + m) * 60 + s); }
           bool set(const char*& s /* in/out */, char sep = ':');
  private:
    int _val;  // in 1 seconds (since midnight)
  private:
    static const int _secondsPerDay;
  public:
    static inline int secondsPerDay() { return _secondsPerDay; }
};


#endif

