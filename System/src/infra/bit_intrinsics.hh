#ifndef SRC_INFRA_BIT_INTRINSICS_HH
#define SRC_INFRA_BIT_INTRINSICS_HH

#include <inttypes.h>

#ifdef __x86_64
#include <immintrin.h>
#endif

/*
 * return the bit and set those bit to the complement
 */

 template<class Tuint>
 inline uint32_t
 idx_complement_bit(const Tuint* x, const Tuint y);
 
 
 template<>
 inline unsigned char
 idx_complement_bit<uint32_t>(const uint32_t* x,const uint32_t y) {
    return _bittestandcomplement(x,y);
 }
 
 template<>
 inline unsigned char
 idx_complement_bit<uint64_t>(const uint64_t* x,const uint64_t y) {
    return _bittestandcomplement64(x,y);
 }


/*
 * return the index of the highest bit set
 */

template<class Tuint>
inline uint32_t
idx_highest_bit_set(const Tuint x);


template<>
inline uint32_t
idx_highest_bit_set<uint32_t>(const uint32_t x) {
#if __ICC
  return (_bit_scan_reverse(x));
#elif __GNUG__
  return (31 - __builtin_clz(x));
#endif
}

template<>
inline uint32_t
idx_highest_bit_set<uint64_t>(const uint64_t x) {
#if __ICC
  return (63 - __builtin_clzll(x)); // gibt kein _bit_scan_reverse64
#elif __GNUG__
  return (63 - __builtin_clzll(x));
#endif
}



/*
 * return the index of the lowest bit set
 */

template<class Tuint>
inline uint32_t
idx_lowest_bit_set(const Tuint x);

template<>
inline uint32_t
idx_lowest_bit_set<uint16_t>(const uint16_t x) {
#if __ICC
  return (_bit_scan_forward(x));
#elif __GNUG__
  return (__builtin_ctz(x));
#endif
}


template<>
inline uint32_t
idx_lowest_bit_set<uint32_t>(const uint32_t x) {
#if __ICC
  return (_bit_scan_forward(x));
#elif __GNUG__
  return (__builtin_ctz(x));
#endif
}


template<>
inline uint32_t
idx_lowest_bit_set<uint64_t>(const uint64_t x) {
#if __ICC
  return (__builtin_ctzll(x)); // gibt kein _bit_scan_forward64
#elif __GNUG__
  return (__builtin_ctzll(x));
#endif
}


/*
 * return the number of bits set
 */

template<class Tuint>
inline uint32_t 
number_of_bits_set(const Tuint);


template<>
inline uint32_t
number_of_bits_set<uint32_t>(const uint32_t x) {
#if __ICC
  return (_popcnt32(x));
#elif __GNUG__
  return (__builtin_popcount(x));
#endif
}


template<>
inline uint32_t
number_of_bits_set<uint64_t>(const uint64_t x) {
#if __ICC
  return (_popcnt64(x));
#elif __GNUG__
  return (__builtin_popcountll(x));
#endif
}


/*
 * return true if X \subseteq Y
 */

template<class Tuint>
bool
isSubsetOf(const Tuint X, const Tuint Y) {
  return (Y == (X | Y));
}

/*
 * returns true if X \cap Y = \emptyset
 */

template<class Tuint>
bool
hasEmptyIntersection(const Tuint X, const Tuint Y) {
  return (0 == (X & Y));
}

/*
 * returns true if X \cap Y \neq \emptyset
 */

template<class Tuint>
bool
hasNonEmptyIntersection(const Tuint X, const Tuint Y) {
  return (0 != (X & Y));
}

/*
 *  calculates the set differentce X \setminus Y
 */

template<class Tuint>
Tuint
setDifference(const Tuint X, const Tuint Y) {
  return (X & (~Y));
}

/*
 *  shifting bits to the left/right according to some given mask
 */


#ifdef __BMI2__
template<class Tuint>
inline Tuint
bit_distribute(const Tuint x, const Tuint aMask);

template<>
inline uint32_t
bit_distribute(const uint32_t x, const uint32_t aMask) {
  return _pdep_u32(x, aMask);
}

template<>
inline uint64_t
bit_distribute(const uint64_t x, const uint64_t aMask) {
  return _pdep_u64(x, aMask);
}

#else
template<typename Tuint>
inline Tuint
bit_distribute(const Tuint x, const Tuint aMask) {
  Tuint m = aMask;
  Tuint r = 0;    // result
  Tuint i = 0;    // index
  uint32_t c = 0; // count
  while(0 != m) {
    i = idx_lowest_bit_set(m);
    m ^= (1 << i);
    r |= (((x >> c) & 0x1) << i);
    ++c;
  }
  return r;
}
#endif


#ifdef __BMI2__
template<class Tuint>
inline Tuint
bit_gather(const Tuint x, const Tuint aMask);

template<class Tuint>
inline uint32_t
bit_gather(const uint32_t x, const uint32_t aMask) {
  return _pext_u32(x, aMask);
}

template<class Tuint>
inline uint64_t
bit_gather(const uint64_t x, const uint64_t aMask) {
  return _pext_u64(x, aMask);
}
#else
template<typename Tuint>
inline Tuint
bit_gather(const Tuint x, const Tuint aMask) {
  Tuint m = aMask;
  Tuint r = 0;    // result
  Tuint i = 0;    // index
  uint32_t c = 0; // count
  while(0 != m) {
    i = idx_lowest_bit_set(m);
    m ^= (1 << i);
    r |= (((x >> i) & 0x1) << c);
    ++c;
  }
  return r;
}
#endif

/*
 *  cyclic shift left
 *  x: element to shift
 *  s: shift
 *  c: capacity, i.e., number of bits to consider
 */

template<typename Tuint>
inline Tuint
cyclic_shift_left(const Tuint x, const uint32_t s, const uint32_t c) {
  return (((x >> (c - s)) | ((x << s) & ~(((Tuint) 1 << s) - 1))) & (((Tuint) 1 << c) - 1));
}

template<typename Tuint>
inline Tuint
cyclic_shift_right(const Tuint x, const uint32_t s, const uint32_t c) {
  return (((x << (c - s)) | ((x >> s) & ~(((Tuint) 1 << s) - 1))) & (((Tuint) 1 << c) - 1));
}

/*
 * check whether a given unsigned integer is a power of 2
*/ 

template<typename Tuint>
inline bool
isPow2(const Tuint x) {
  return (0 == (x & (x - 1)));
}

/*
 * round to the nearest power of 2, nearest defined by q
 * works only for unsigned int
 */

template<typename Tuint>
inline Tuint
roundQPow2(const Tuint x) {
  int lHiIdx = idx_highest_bit_set<Tuint>(x);
  Tuint lRes = (((Tuint) 1) << (lHiIdx + (0x3 == (x >> (lHiIdx - 1)))));
  return lRes;
}

/*
 * round to nearest power of 2, nearest defined by l_1
 * works only for unsigned int
 * on equal terms, it prefers the smaller power of 2
 */

template<typename Tuint>
inline Tuint
roundAbsPow2(const Tuint x) {
  const Tuint z1 = roundQPow2(x);
  Tuint lRes = z1;
  if(z1 > x) {
    const Tuint z2 = (z1 >> 1);
    const Tuint d1 = (z1 - x);
    const Tuint d2 = (x - z2);
    lRes = ((d1 < d2) ? z1 : z2);
  }
  return lRes;
}

#endif
