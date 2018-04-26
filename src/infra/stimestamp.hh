#ifndef STIMESTAMP_HH
#define STIMESTAMP_HH

#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <time.h>

#include "datejd.hh"
#include "stime.hh"

class STimestamp {
  public:
    STimestamp() : _date(), _time() {}
    STimestamp(const DateJd& d, const STime& t) : _date(d), _time(t) {}
  public:
    inline const DateJd& get_date() const { return _date; }
    inline void  get_date(const DateJd& x) { _date = x; }
    inline const STime& get_time() const { return _time; }
    inline void  get_time(const STime& x) { _time = x; }
  public:
    inline DateJd& get_date() { return _date; }
    inline STime&  get_time() { return _time; }
  public:
    bool operator==(const STimestamp& x) const { 
                   return ((get_date() == x.get_date()) && (get_time() == x.get_time())); 
                 }
    bool operator<(const STimestamp&) const;
    bool operator<=(const STimestamp&) const;
    bool operator>(const STimestamp&) const;
    bool operator>=(const STimestamp&) const;
  public:
    bool set(const char*& x, char sep = ':');
  public:
    inline void resetMinSec() { _time.resetMinSec(); }
  public:
    void setCurrentUTC(); // sets to current time in UTC
    void setCurrentLocal();    // sets to current in local time
  public:
    std::ostream& print(std::ostream& os, char sep = ':') const;
  private:
    DateJd _date;
    STime  _time;
};

inline std::ostream&
operator<<(std::ostream& os, const STimestamp& x) {
  return x.print(os, ':');
}

#endif // STIME_STAMP_HH

