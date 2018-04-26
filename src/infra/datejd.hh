#ifndef DATE_JULIAN_DAY_HH
#define DATE_JULIAN_DAY_HH

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>

/*
 * DateJd
 * speicher julian day
 * gueltig fuer alle jahre nach 1582
 */

class DateJd {
  public:
  enum Weekday { Sunday    = 0,
                 Monday    = 1,
                 Tuesday   = 2,
                 Wednesday = 3,
                 Thursday  = 4,
                 Friday    = 5,
                 Saturday  = 6 };

  ///
  enum Month { January   = 0,
               February  = 1,
               March     = 2,
               April     = 3,
               May       = 4,
               June      = 5,
               July      = 6,
               August    = 7,
               September = 8,
               October   = 9,
               November  = 10,
               December  = 11 };


  public:
    typedef unsigned int uint;
  public:
    DateJd() : _jd(0) {}
    DateJd(uint x, bool) : _jd(x) {} // julianDay
    DateJd(uint); // ddmmyy for date after year 2000
    DateJd(int,int,int);
    DateJd(const std::string&, bool aYearHigh, char sep = ':');
  public:
    inline uint julianDay() const { return _jd; }
    inline uint jd() const { return _jd; }
    inline void get(uint& y, uint& m, uint& d) const { jd2ymd(_jd, y, m, d); }
    inline DateJd& set_to_current_date() { _jd = jdOfToday(); return (*this); }
    static inline DateJd current_date() { return DateJd(jdOfToday(), true); }
  public:
    inline uint year() const {
                          int l = _jd + 68569;
                          const int n = ( 4 * l ) / 146097;
                                    l = l - ( 146097 * n + 3 ) / 4;
                          const int i = ( 4000 * ( l + 1 ) ) / 1461001;
                                    l = l - ( 1461 * i ) / 4 + 31;
                          const int j = ( 80 * l ) / 2447;
                                    l = j / 11;
                                int y = 100 * ( n - 49 ) + i + l;
                        return y;
                }
    uint month() const;
    uint day() const;
    DateJd& set(const uint y, const uint m, const uint d);
    bool    set(const char*& s /* in/out */, bool aYearHigh, char sep = ':');
    void    set(const uint aYYYYMMDD);
    void    set(std::istream& is, const char aSep);
    void    set(uint aJD, bool);
  public:
    uint day_of_year() const; // starts with first of january = 1
    uint dayOfTheYear() const; // starts with first of january = 0
    Month month_of_year() const;
    Weekday day_of_week() const;
    bool is_leap_year() const;
  public:
    inline DateJd& add_days(uint x) { _jd += x; return (*this); }
    inline DateJd& subtract_days(uint x) { _jd -= x; return (*this); }
    inline int subtract_date(const DateJd& x) const { return (((int) _jd) - ((int) x._jd)); }
  public:
    static uint ymd2jd(const uint y, const uint m, const uint d);
    static void jd2ymd(const uint aJulianDay, uint& y, uint& m, uint& d);
    static uint jdOfToday();
  public:
    inline DateJd& operator=(const DateJd& x) { _jd = x._jd; return (*this); }
    inline bool operator==(const DateJd& x) const { return (_jd == x._jd); }
    inline bool operator!=(const DateJd& x) const { return (_jd != x._jd); }
    inline bool operator<(const DateJd& x) const { return (_jd < x._jd); }
    inline bool operator<=(const DateJd& x) const { return (_jd <= x._jd); }
    inline bool operator>(const DateJd& x) const { return (_jd > x._jd); }
    inline bool operator>=(const DateJd& x) const { return (_jd >= x._jd); }
    inline int operator-(const DateJd& x) const { return (((int) _jd) - ((int) x._jd)); }
    inline DateJd& operator++() { ++_jd; return (*this); } // add one day
    inline DateJd& operator--() { --_jd; return (*this); } // subtract one day
  public:
    inline DateJd operator+(int x) const { return DateJd(jd() + x, true); }
    inline DateJd operator-(int x) const { return DateJd(jd() - x, true); }
    inline DateJd& operator+=(int x) { _jd += x; return (*this); }
    inline DateJd& operator-=(int x) { _jd -= x; return (*this); }
  public:
    static int getMonthNo(const char* aMonthName); // name -> int
  public:
    std::ostream& print(std::ostream&, char sep = ':') const;
    std::ostream& printXml(std::ostream&) const;
  private:
    uint _jd;
};

inline
std::ostream& operator<<(std::ostream& os, const DateJd& x) {
  return x.print(os, ':');
}

inline
std::istream& operator>>(std::istream& is, DateJd& x) {
  int y,m,d;
  char s1, s2;
  is >> y >> s1 >> m >> s2 >> d;
  x.set(y,m,d);
  return is;
}



#endif
