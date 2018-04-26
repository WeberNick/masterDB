#ifndef INFRA_TYPES_VAL_TT_HH
#define INFRA_TYPES_VAL_TT_HH

#include <iostream>

/*
 * val_tt: simple template for value type with NULL values
 */
typedef unsigned int uint;

template <class T>
class val_tt {
  public:
    val_tt() : _isNull(true), _val() {}
    val_tt(T x, bool n) : _isNull(n), _val(x) {}

  public:
    inline bool isNull() const { return _isNull; }
    inline const T& val() const { return _val; }
    inline T& val() { return _val; }
    inline void setNotNull() { _isNull = false; }
    inline void setNull() { _isNull = true; }
    inline void setVal(T x) {
        _val = x;
        _isNull = false;
    }
    inline void negate() { _val = -_val; }

  public:
    val_tt& operator&=(const val_tt& x); // special assignment: if(x != null) (*this) = x;
  public:
    bool operator==(const T& x) const;
    bool operator<(const T& x) const;
    bool operator<=(const T& x) const;
    bool operator>(const T& x) const;
    bool operator>=(const T& x) const;
    val_tt& operator=(const T& x);
    val_tt& operator+=(const T& x);
    val_tt& operator-=(const T& x);
    val_tt& operator*=(const T& x);

  public:
    bool operator==(const val_tt& x) const;
    bool operator<(const val_tt& x) const;
    bool operator<=(const val_tt& x) const;
    bool operator>(const val_tt& x) const;
    bool operator>=(const val_tt& x) const;
    val_tt operator-(const val_tt<T>& x) const;

  private:
    bool _isNull;
    T _val;
};

typedef val_tt<int> ival_t;
typedef val_tt<uint> uval_t;
typedef val_tt<float> fval_t;
typedef val_tt<double> dval_t;

template <class T>
std::ostream& operator<<(std::ostream& os, const val_tt<T>& x) {
    if (x.isNull()) {
        os << "NULL";
    } else {
        os << x.val();
    }
    return os;
}

template <class T>
bool val_tt<T>::operator==(const T& x) const {
    return (!isNull() && val() == x);
}

template <class T>
bool val_tt<T>::operator<(const T& x) const {
    return (!isNull() && val() < x);
}

template <class T>
bool val_tt<T>::operator<=(const T& x) const {
    return (!isNull() && val() <= x);
}

template <class T>
bool val_tt<T>::operator>(const T& x) const {
    return (!isNull() && val() > x);
}

template <class T>
bool val_tt<T>::operator>=(const T& x) const {
    return (!isNull() && val() >= x);
}

template <class T>
bool val_tt<T>::operator==(const val_tt& x) const {
    return (!isNull() && !x.isNull() && val() == x.val());
}

template <class T>
bool val_tt<T>::operator<(const val_tt& x) const {
    return (!isNull() && !x.isNull() && val() < x.val());
}

template <class T>
bool val_tt<T>::operator<=(const val_tt& x) const {
    return (!isNull() && !x.isNull() && val() <= x.val());
}

template <class T>
bool val_tt<T>::operator>(const val_tt& x) const {
    return (!isNull() && !x.isNull() && val() > x.val());
}

template <class T>
bool val_tt<T>::operator>=(const val_tt& x) const {
    return (!isNull() && !x.isNull() && val() >= x.val());
}

template <class T>
val_tt<T>& val_tt<T>::operator=(const T& x) {
    _isNull = false;
    _val = x;
    return (*this);
}

template <class T>
val_tt<T>& val_tt<T>::operator&=(const val_tt<T>& x) {
    if (!x.isNull()) { (*this) = x; }
    return (*this);
}

template <class T>
val_tt<T>& val_tt<T>::operator+=(const T& x) {
    if (!isNull()) { _val += x; }
    return (*this);
}

template <class T>
val_tt<T>& val_tt<T>::operator-=(const T& x) {
    if (!isNull()) { _val -= x; }
    return (*this);
}

template <class T>
val_tt<T>& val_tt<T>::operator*=(const T& x) {
    if (!isNull()) { _val *= x; }
    return (*this);
}

template <class T>
val_tt<T> val_tt<T>::operator-(const val_tt<T>& x) const {
    const bool lIsNull = (isNull() || x.isNull());
    return val_tt<T>(_val - x._val, lIsNull);
}

#endif
