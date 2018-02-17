/*
 *  @file    types.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *           Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *
 */

#ifndef INFRA_TYPES_HH
#define INFRA_TYPES_HH

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

typedef std::byte byte;
typedef std::vector<byte *> byte_vpt;
typedef unsigned int uint;
typedef std::vector<uint> uint_vt;
typedef std::vector<uint32_t> uint32_vt;


struct part_t
{
	uint _pID;
	std::string _pName;
	std::string _pPath;
	int _pType;
	int _pGrowth;
};
typedef std::vector<part_t> part_vt;

struct seg_t
{
	uint _sPID; //partition ID
	uint _sID; //segment ID
	std::string _sName; //segment name (unique)
	int _sType; //segment type
	uint _sFirstPage; //first segment header page in order to load segment into memory
};
typedef std::vector<seg_t> seg_vt;

enum class PageStatus {
    kNoType = -1,
    kBUCKET0 = 0,
    kBUCKET1 = 1,
    kBUCKET2 = 2,
    kBUCKET3 = 3,
    kBUCKET4 = 4,
    kBUCKET5 = 5,
    kBUCKET6 = 6,
    kBUCKET7 = 7,
    kBUCKET8 = 8,
    kBUCKET9 = 9,
    kBUCKET10 = 10,
    kBUCKET11 = 11,
    kBUCKET12 = 12,
    kBUCKET13 = 13,
    kBUCKET14 = 14,
    kBUCKET15 = 15,
    kEndType = 16
};

#endif
