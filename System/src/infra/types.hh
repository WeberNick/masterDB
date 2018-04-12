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
#include <limits>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>

typedef std::size_t size_t;
typedef std::byte byte;
typedef std::vector<byte *> byte_vpt;
typedef unsigned int uint;
typedef std::vector<uint> uint_vt;
typedef std::vector<uint32_t> uint32_vt;
typedef std::shared_mutex sMtx;
typedef std::mutex mtx;

constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

struct control_block_t
{
    std::string     _masterPartition;
    std::string     _masterSegmentSegments;
    std::string     _masterSegmentPartitions;
    size_t          _pageSize;
    bool            _trace;

    std::string   mstrPart() const { return _masterPartition; }
    size_t        pageSize() const { return _pageSize; }
    bool          trace() const { return _trace; }
};


struct page_id_t
{
    uint8_t _fileID;
    uint32_t _pageNo; //correct? 

    uint8_t fileID() const { return _fileID; }
    uint32_t pageNo() const { return _pageNo; }

    bool operator==(const page_id_t& aOther) 
    {
        return (_fileID == aOther._fileID && _pageNo == aOther._pageNo);
    }
};
typedef page_id_t pid;

struct part_t
{
	uint _pID;
	std::string _pName;
	std::string _pPath;
	int _pType;//1:= PartitionFile, 2:=partitionRaw
	int _pGrowth;
};
typedef std::vector<part_t> part_vt;

struct seg_t
{
	uint _sPID; //partition ID
	uint _sID; //segment ID
	std::string _sName; //segment name (unique)
	int _sType; //segment type; 1:= SegmentFSM, 2:=SegmentFSM_SP
	uint _sFirstPage; //first segment index ( (C) Nico) page in order to load segment into memory
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
    kNoBuckets = 16
};

enum LOCK_MODE
{
    kNoType = -1,
    kFREE = 0,
    kSHARED = 1,
    kEXCLUSIVE = 2
};


#endif
