/*
 *  @file    types.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *           Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   Implementing typedefs and enums used throughout the project
 *  @bugs    Currently no bugs known
 *  @todos   -
 *  @section TODO
 */
 
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include <string>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <type_traits>
#include <cassert>

using byte = std::byte;
using size_t = std::size_t;
using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;
using int8_t = std::int8_t;
using uint = unsigned int;
using char_vpt = std::vector<char*>;
using uint_vt = std::vector<uint>;
using byte_vpt = std::vector<byte *>;
using uint32_vt = std::vector<uint32_t>;
using string_vt = std::vector<std::string>;
using sMtx = std::shared_mutex;
using mtx = std::mutex;

constexpr size_t INVALID = std::numeric_limits<size_t>::max();
constexpr uint32_t INVALID_32 = std::numeric_limits<uint32_t>::max();
constexpr uint16_t INVALID_16 = std::numeric_limits<uint16_t>::max();

struct control_block_t
{
    std::string _masterPartition;
    const std::string _tracePath;
    const size_t _pageSize;
    const size_t _noBufFrames;
    const bool _trace;

    std::string mstrPart() const noexcept { return _masterPartition; }
    const std::string& tracePath() const noexcept { return _tracePath; }
    size_t pageSize() const noexcept { return _pageSize; }
    size_t frames() const noexcept { return _noBufFrames; }
    bool trace() const noexcept { return _trace; }
};
using CB = control_block_t;

inline std::ostream& operator<<(std::ostream& strm, const CB& cb)
{
    strm << "The following parameters are set:\n"
         << "Master Partition Path: " << cb.mstrPart() << "\n"
         << "Path of Log File: " << cb.tracePath() << "\n"
         << "Page Size: " << cb.pageSize() << "\n"
         << "Buffer Frames: " << cb.frames() << "\n"
         << "Trace: " << cb.trace() << "\n";
    return strm << std::endl;
}

struct page_id_t
{
    uint8_t  _fileID;
    uint32_t _pageNo;

    uint8_t  fileID() const noexcept { return _fileID; }
    uint32_t pageNo() const noexcept { return _pageNo; }

    bool operator==(const page_id_t& aOther) const noexcept
    {
        return (_fileID == aOther._fileID && _pageNo == aOther._pageNo);
    }

    bool operator==(const page_id_t& aOther) noexcept
    {
        return static_cast<const page_id_t&>(*this).operator==(aOther);
    }

    std::string to_string() const noexcept
    {
        return std::string("File ID : '") + std::to_string(_fileID) + std::string("', Page : '") + std::to_string(_pageNo) + std::string("'");
    }

    std::string to_string()
    {
        return static_cast<const page_id_t&>(*this).to_string();
    }

};
using PID = page_id_t;
using pid_vt = std::vector<PID>;

struct tuple_identifier_t
{
    uint32_t _pageNo;
    uint32_t _tupleNo;

    uint32_t pageNo() const noexcept { return _pageNo; }
    uint32_t pageNo() noexcept { return _pageNo; }
    uint32_t tupleNo() const noexcept { return _tupleNo; }
    uint32_t tupleNo() noexcept { return _tupleNo; }

    std::string to_string() const noexcept { return std::string("Page : '") + std::to_string(_pageNo) + std::string("', Tuple No. : '") + std::to_string(_tupleNo) + "'"; }
    std::string to_string() noexcept { return static_cast<const tuple_identifier_t&>(*this).to_string(); }
};
using TID = tuple_identifier_t;
using tid_vt = std::vector<TID>;

enum class PageStatus: int8_t
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
    kPageStatusSize = 16,
    kMAX = 15
};

enum class LOCK_MODE: int8_t
{
    kNoType = -1,
    kNOLOCK = 0, // not in use
    kSHARED = 1,
    kEXCLUSIVE = 2,
    kLockModeSize = 3
};

template<typename E>
constexpr auto toType(E enumerator) noexcept
{
    return static_cast<std::underlying_type_t<E>>(enumerator);
}

inline std::string lockModeToString(LOCK_MODE aMode)
{
    switch(aMode)
    {
        case LOCK_MODE::kNoType: 
            return std::string("kNoType"); 
            break;
        case LOCK_MODE::kNOLOCK: 
            return std::string ("kNOLOCK"); 
            break;
        case LOCK_MODE::kSHARED: 
            return std::string("kSHARED"); 
            break;
        case LOCK_MODE::kEXCLUSIVE: 
            return std::string("kEXCLUSIVE"); 
            break;
        case LOCK_MODE::kLockModeSize: 
            return std::string("Number of lock types: ") + std::to_string(toType(LOCK_MODE::kLockModeSize)); 
            break;
        default: 
            assert (!"Invalid default-case of switch statement reached"); 
            break;
    }
}
/**
 * @brief  Translate type to a readable string representation
 * 
 * @param  aType the type
 * @return const std::string the human-interpretable string
 */
inline std::string segTypeToString(int aType) // For Segments
{
    switch(aType)
    {
        case 1:
            return "FSM";
            break;
        case 2:
            return "FSM_SP";
            break;
        default:
            return "NaN";
    }
}

inline std::string partTypeToString(int aType) // For Partitions
{
    switch(aType)
    {
        case 0:
            return "Raw";
            break;
        case 1:
            return "File";
            break;
        default:
            return "NaN";
    }
}
