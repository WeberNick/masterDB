#include "stimestamp.hh"

bool
STimestamp::operator<(const STimestamp& x) const {
  return (get_date() < x.get_date() ||
          (get_date() == x.get_date() && get_time() < x.get_time()));
}

bool
STimestamp::operator<=(const STimestamp& x) const {
  return (get_date() < x.get_date() ||
          (get_date() == x.get_date() && get_time() <= x.get_time()));
}

bool
STimestamp::operator>(const STimestamp& x) const {
  return (get_date() > x.get_date() ||
          (get_date() == x.get_date() && get_time() > x.get_time()));
}

bool
STimestamp::operator>=(const STimestamp& x) const {
  return (get_date() > x.get_date() ||
          (get_date() == x.get_date() && get_time() >= x.get_time()));
}

bool STimestamp::set(const char*& x, char sep) {
    // std::cerr << "I000 STimestamp::set" << std::endl;

    if (!(_date.set(x, true, sep))) {
        std::cerr << "I001 STimestamp::set" << std::endl;
        return false;
    }
    if (sep != *x) {
        std::cerr << "I002 STimestamp::set" << std::endl;
        return false;
    }
    ++x;
    // std::cerr << "I003 STimestamp::set" << std::endl;
    return _time.set(x, sep);
}

void STimestamp::setCurrentUTC() {
    time_t lTimeTics = time(0);
    tm lTimeRes;
    gmtime_r(&lTimeTics, &lTimeRes);

    // std::cerr << "UTC: " << asctime(&lTimeRes) << std::endl;
    // std::cerr << "UTC date: "
    //           << lTimeRes.tm_year << ':'
    //           << lTimeRes.tm_mon  << ':'
    //           << lTimeRes.tm_mday << std::endl;

    _date.set(lTimeRes.tm_year + 1900, lTimeRes.tm_mon + 1, lTimeRes.tm_mday);
    _time.set(lTimeRes.tm_hour, lTimeRes.tm_min, lTimeRes.tm_sec);
}

void STimestamp::setCurrentLocal() {
    time_t lTimeTics = time(0);
    tm lTimeRes;
    localtime_r(&lTimeTics, &lTimeRes);

    // std::cerr << "LOC: " << asctime(&lTimeRes) << std::endl;
    // std::cerr << "LOC date: "
    //           << lTimeRes.tm_year << ':'
    //           << lTimeRes.tm_mon  << ':'
    //           << lTimeRes.tm_mday << std::endl;

    _date.set(lTimeRes.tm_year + 1900, lTimeRes.tm_mon + 1, lTimeRes.tm_mday);
    _time.set(lTimeRes.tm_hour, lTimeRes.tm_min, lTimeRes.tm_sec);
}

std::ostream& STimestamp::print(std::ostream& os, char sep) const {
    get_date().print(os, sep);
    os << sep;
    get_time().print(os, sep);
    return os;
}
