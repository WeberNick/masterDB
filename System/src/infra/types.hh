/*
 *  @file    types.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de) & Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *
 */

#ifndef INFRA_TYPES_HH
#define INFRA_TYPES_HH

#include <cstdint>
#include <cstddef>
#include <vector>

typedef std::byte byte;
typedef std::vector<byte*> byte_vpt;
typedef unsigned int uint;
typedef std::vector <uint> uint_vt;
typedef std::vector <uint32_t> uint32_vt;

enum SegmentPageStatus {
    //by now only 4 steps, which means 2 bits are unused.
    full=4,
    q75=3,
    q50=2,
    q25=1,
    empty=0
};

#endif