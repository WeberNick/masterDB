#include "datejd.hh"

// DateJd::DateJd() : _jd(0) {}

// DateJd::DateJd(uint x, bool) : _jd(x) {}


// x is date of the form ddmmyy
DateJd::DateJd(uint x) : _jd(0) {
  uint y = x % 100;
  uint m = ((x - y) / 100) % 100;
  uint d = x / 10000;
  y += 2000;
  _jd = ymd2jd(y,m,d);
}

DateJd::DateJd(int y, int m, int d) : _jd(ymd2jd(y,m,d)) {}

DateJd::DateJd(const std::string& s, bool aYearHigh, char sep) : _jd(0) {
  const char* x = s.c_str();
  set(x, aYearHigh, sep);
}

DateJd::uint
DateJd::jdOfToday() {
  time_t c;
  time(&c);
  struct tm* today = localtime(&c);
  return ymd2jd(today->tm_year + 1900, today->tm_mon + 1, today->tm_mday);
}

bool
DateJd::set(const char*& aStringPtr, bool aYearHigh, char sep) {
  const char* x = aStringPtr;
  char* end;
 
  uint y = 0;
  uint m = 0;
  uint d = 0;
  if(aYearHigh) {
    y = strtol(x, &end, 10);
    if(end == x) { return false; }
    ++end;
    x = end;
    m = strtol(x, &end, 10);
    if(end == x) { return false; }
    ++end;
    x = end;
    d = strtol(x, &end, 10);
    if(end == x) { return false; }
    x = end;
  } else {
    d = strtol(x, &end, 10);
    if(end == x) { return false; }
    ++end;
    x = end;
    m = strtol(x, &end, 10);
    if(end == x) { return false; }
    ++end;
    x = end;
    y = strtol(x, &end, 10);
    if(end == x) { return false; }
    x = end;
  }
  aStringPtr = x;
  _jd = ymd2jd(y,m,d);
  return true;
}

DateJd&
DateJd::set(const uint y, const uint m, const uint d) {
  _jd = ymd2jd(y,m,d);
  return (*this);
}

void
DateJd::set(const uint aYYYYMMDD) {
  uint x = aYYYYMMDD;
  const uint y = (x / 10000);
  x = (x % 10000);
  const uint m = x / 100;
  const uint d = x % 100;
  set(y,m,d);
}

void
DateJd::set(std::istream& is, const char aSep) {
  char lSep;
  uint y,m,d;
  is >> y;
  if(is.eof()) { return; }
  is >> lSep;
  if(is.eof()) { return; }
  if(aSep != lSep) {
    std::cerr << "Warning: wrong field separator in date: `" << lSep << "'." << std::endl;
  }
  is >> m;
  if(is.eof()) { return; }
  is >> lSep;
  if(is.eof()) { return; }
  if(aSep != lSep) {
    std::cerr << "Warning: wrong field separator in date: `" << lSep << "'." << std::endl;
  }
  is >> d;
  set(y,m,d);
}

void DateJd::set(const uint aJD, bool)
{
  _jd = aJD;
}

DateJd::uint
DateJd::ymd2jd(const uint ay, const uint am, const uint ad) {
/* tuts
    double Y = (double) y;
    double M = (double) m;
    double D = (double) d;
    if(M < 3.0) {
      Y -= 1.0;
      M += 12.0;
    }
    double A = floor(Y / 100.0);
    // cout.precision(10);
    // cout << "A " << A << endl;
    double B = floor(A / 4);
    // cout << "B " << B << endl;
    double C = 2 - A + B;
    // cout << "C " << C << endl;
    double E = floor(365.25 * (Y + 4716));
    // cout << "E " << E << endl;
    double F = floor(30.6001 * (M + 1.0));
    // cout << "F " << F << endl;
    return (int) round(C + D + E + F - 1524.5);
*/

   const int y = (int) ay;
   const int m = (int) am;
   const int d = (int) ad;
   return ( 1461 * ( y + 4800 + ( m - 14 ) / 12 ) ) / 4 +
          ( 367 * ( m - 2 - 12 * ( ( m - 14 ) / 12 ) ) ) / 12 -
          ( 3 * ( ( y + 4900 + ( m - 14 ) / 12 ) / 100 ) ) / 4 +
          d - 32075;
}

void
DateJd::jd2ymd(const uint jd, uint& y, uint& m, uint& d) {
/* tuts nicht ganz. monat dez ist manchmal 0
    const double JD = (double) jd;
    const double Z = JD + 0.5;
    const double W = floor((Z - 1867216.25) / 36524.25);
    const double X = floor(W / 4);
    const double A = floor(Z + 1 + W - X);
    const double B = floor(A + 1524);
    const double C = floor((B - 122.1) / 365.25);
    const double D = floor(365.25 * C);
    const double E = floor((B - D) / 30.6001);
    const double F = floor(30.6001 * E);
    d = ((uint) (B - D - F));
    const uint e = (uint) E;
    m = ((e <= 12.0) ? (e - 1) : (e - 13));
    const uint c = (uint) C;
    y = ((m <= 2) ? (c - 4715) : c - 4716);
*/
          int l = jd + 68569;
    const int n = ( 4 * l ) / 146097;
              l = l - ( 146097 * n + 3 ) / 4;
    const int i = ( 4000 * ( l + 1 ) ) / 1461001;
              l = l - ( 1461 * i ) / 4 + 31;
    const int j = ( 80 * l ) / 2447;
              d = l - ( 2447 * j ) / 80;
              l = j / 11;
              m = j + 2 - ( 12 * l );
              y = 100 * ( n - 49 ) + i + l;
}



std::ostream&
DateJd::print(std::ostream& os, char sep) const {
  uint y = 0;
  uint m = 0;
  uint d = 0;
  get(y,m,d);
  os << y << sep
     << (m < 10 ? "0" : "") << m << sep
     << (d < 10 ? "0" : "") << d;
  return os;
}

std::ostream&
DateJd::printXml(std::ostream& os) const {
  uint y = 0;
  uint m = 0;
  uint d = 0;
  get(y,m,d);
  if(10 > y) {
    os << '0';
  }
  os << y << '-';
  if(10 > m) {
    os << '0';
  }
  os << m << '-';
  if(10 > d) {
    os << '0';
  }
  os << d;
  return os;
}

/*
DateJd::uint
DateJd::year() const {
  // uint y,m,d;
  // jd2ymd(_jd,y,m,d);
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
*/

DateJd::uint
DateJd::month() const {
  uint y,m,d;
  jd2ymd(_jd,y,m,d);
  return m;
}

DateJd::uint
DateJd::day() const {
  uint y,m,d;
  jd2ymd(_jd,y,m,d);
  return d;
}

DateJd::uint
DateJd::day_of_year() const {
  return (_jd - DateJd(year(),1,1).jd() + 1);
}

DateJd::uint
DateJd::dayOfTheYear() const {
  return (_jd - DateJd(year(),1,1).jd());
}

DateJd::Month
DateJd::month_of_year() const {
  return (Month) (month() - 1);
}

DateJd::Weekday
DateJd::day_of_week() const {
  DateJd someSunday(1997,10,12);
  return (Weekday) ((someSunday <= (*this)) ? 
                      (subtract_date(someSunday) % 7) 
                    : (someSunday.subtract_date(*this) % 7));
}

/** Leap Year rules:
    1. Years evenly divisible by four are to be considered Leap Years.
    2. Century years (double 00) must be evenly divisible by 400
    (2000 is a leap year).
    3. The years 4000, 8000, and 12000
    are considered to be "common years" (not leap years).
    4. Century years divisible by 900 will be leap years
    only if the remainder is 200 or 600 (2000 is a leap year).
*/

bool 
DateJd::is_leap_year() const {
  uint y = year();
  uint c = y % 100;
  uint r = y % 900;
  if(y == 4000 || y == 8000 || y == 12000) { return false; }      // 3
  if((r == 0) && ((y % 200) == 0 || ((y % 600) == 0))) { return true; } // 4
  if(c == 0 && (y % 400) == 0) { return true; }
  return (c != 0) && ((y % 4) == 0);
}

int
DateJd::getMonthNo(const char* aMonth) {
  int lRes = -1;
  if(0 == strcmp(aMonth, "January")) {
    lRes = 0;
  } else
  if(0 == strcmp(aMonth, "February")) {
    lRes = 1;
  } else
  if(0 == strcmp(aMonth, "March")) {
    lRes = 2;
  } else
  if(0 == strcmp(aMonth, "April")) {
    lRes = 3;
  } else
  if(0 == strcmp(aMonth, "May")) {
    lRes = 4;
  } else
  if(0 == strcmp(aMonth, "June")) {
    lRes = 5;
  } else
  if(0 == strcmp(aMonth, "July")) {
    lRes = 6;
  } else
  if(0 == strcmp(aMonth, "August")) {
    lRes = 7;
  } else
  if(0 == strcmp(aMonth, "September")) {
    lRes = 8;
  } else
  if(0 == strcmp(aMonth, "October")) {
    lRes = 9;
  } else
  if(0 == strcmp(aMonth, "November")) {
    lRes = 10;
  } else
  if(0 == strcmp(aMonth, "December")) {
    lRes = 11;
  } else {
    lRes = -1;
  }
  return lRes;
}



