/*
 *  @file    types.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de) & Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *
 */

#ifndef INFRA_TYPES_HH
#define INFRA_TYPES_HH

#include <cstddef>
#include <cstdint>
#include <vector>

typedef std::byte byte;
typedef std::vector<byte *> byte_vpt;
typedef unsigned int uint;
typedef std::vector<uint> uint_vt;
typedef std::vector<uint32_t> uint32_vt;

enum SegmentPageStatus {
    kNoType = -1,
    kEMPTY = 0,
    kQUANTILE25 = 1,
    kQUANTILE50 = 2,
    kQUANTILE75 = 3,
    kFULL = 4,
    kEndType = 5
};

#endif