/*
 *  @file    types.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *           Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *
 */
#pragma once

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
    const std::string   _masterPartition;
    const std::string   _tracePath;
    const size_t        _pageSize;
    const size_t        _noBufFrames;
    const bool          _print;
    const bool          _trace;


    const std::string&  mstrPart() const { return _masterPartition; }
    const std::string&  tracePath() const { return _tracePath; }
    size_t              pageSize() const { return _pageSize; }
    size_t              frames() const { return _noBufFrames; }
    bool                print() const { return _print; }
    bool                trace() const { return _trace; }
};
using CB = control_block_t;

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
using PID = page_id_t;
using pid_vt = std::vector<PID>;

struct part_t
{
	uint _pID;
	std::string _pName;
	std::string _pPath;
	int _pType;
	uint _pGrowth;
};
using part_vt = std::vector<part_t>;

struct seg_t
{
	uint _sPID; //partition ID
	uint _sID; //segment ID
	std::string _sName; //segment name (unique)
	int _sType; //segment type
	uint _sFirstPage; //first segment index ( (C) Nico) page in order to load segment into memory
};
using seg_vt = std::vector<seg_t>;

enum class PageStatus 
{
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
    kPageStatusSize = 16
};

enum LOCK_MODE
{
    kNoType = -1,
    kNOLOCK = 0,
    kSHARED = 1,
    kEXCLUSIVE = 2,
    kLockModeSize = 3
};
