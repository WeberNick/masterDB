/*********************************************************************
 * @file    tuples.hh
 * @author 	Nick Weber
 * @date    Mai 07, 2018
 * @brief 	Declaration of various tuple classes. This is implemented to
 *          distinguish between disk and memory representations of tuple
 * @bugs 	TBD
 * @todos 	TBD
 * 
 * @section	DESCRIPTION
 * TBD
 * 
 * @section USE
 * TBD
 ********************************************************************/
#pragma once

#include "types.hh"
#include "exception.hh"
#include "trace.hh"

#include <cstring>
#include <string>
#include <iostream>
#include <bitset>
#include <vector>
#include <utility>

class Partition_T
{
    public:
        explicit Partition_T();
        explicit Partition_T(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth); 
        explicit Partition_T(const Partition_T& aPartTuple);
        explicit Partition_T(Partition_T&&) = delete;
        Partition_T& operator=(const Partition_T& aPartitionTuple);
        Partition_T& operator=(Partition_T&&) = delete;
        ~Partition_T() = default;

    public:
        void init(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth);
        void toDisk(byte* aPtr) const;
        void toDisk(byte* aPtr);
        void toMemory(byte* aPtr);
    
    public:
        //getter
        inline size_t size() const { return _size; };
        inline size_t size() { return _size; }
        inline uint8_t ID() const { return _pID; }
        inline uint8_t ID() { return _pID; }
        inline const std::string& name() const { return _pName; }
        inline const std::string& name() { return _pName; }
        inline const std::string& path() const { return _pPath; }
        inline const std::string& path() { return _pPath; }
        inline uint8_t type() const { return _pType; }
        inline uint8_t type() { return _pType; }
        inline uint16_t growth() const { return _pGrowth; }
        inline uint16_t growth() { return _pGrowth; }
        inline std::string to_string() const { return std::string("Size: ") + std::to_string(_size) + std::string(", ID: ") + std::to_string(_pID) + std::string(", Name: ") + _pName + std::string(", Path: ") + _pPath + std::string(", Type: ") + std::to_string(_pType) + std::string(", Growth: ") + std::to_string(_pGrowth); }
        inline std::string to_string() { return static_cast<const Partition_T&>(*this).to_string(); }

    private:
        size_t      _size; // size of class, with all the strings
        /* Tuple content*/
        uint8_t     _pID;
        std::string _pName;
        std::string _pPath;
        uint8_t     _pType;   // 1:= PartitionFile, 2:=partitionRaw
        uint16_t    _pGrowth;
};
std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple);
using part_vt = std::vector<Partition_T>;

class Segment_T
{
    public:
        explicit Segment_T();
        explicit Segment_T(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage);
        explicit Segment_T(const Segment_T& aSegmentTuple);
        explicit Segment_T(Segment_T&&) = delete;
        Segment_T& operator=(const Segment_T& aSegmentTuple);
        Segment_T& operator=(Segment_T&&) = delete;
        ~Segment_T() = default;

    public:
        void init(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage);
        void toDisk(byte* aPtr) const;
        void toDisk(byte* aPtr);
        void toMemory(byte* aPtr);
    
    public:
        inline size_t size() const { return _size; };
        inline size_t size() { return _size; };
        inline uint8_t partID() const { return _sPID; }
        inline uint8_t partID() { return _sPID; }
        inline uint16_t ID() const { return _sID; }
        inline uint16_t ID() { return _sID; }
        inline const std::string& name() const { return _sName; }
        inline const std::string& name() { return _sName; }
        inline uint8_t type() const { return _sType; }
        inline uint8_t type() { return _sType; }
        inline uint32_t firstPage() const { return _sFirstPage; } 
        inline uint32_t firstPage() { return _sFirstPage; } 
        inline std::string to_string() const { return std::string("Size: ") + std::to_string(_size) + std::string(", Partition ID: ") + std::to_string(_sPID) + std::string(", ID: ") + std::to_string(_sID) + std::string(", Name: ") + _sName + std::string(", Type: ") + std::to_string(_sType) + std::string(", First Page: ") + std::to_string(_sFirstPage); }
        inline std::string to_string() { return static_cast<const Segment_T&>(*this).to_string(); }
     
    private:
        size_t      _size; // size of class, with all the strings
        /* Tuple content */
        uint8_t     _sPID;       // partition ID
        uint16_t    _sID;        // segment ID
        std::string _sName;      // segment name (unique)
        uint8_t     _sType;      // segment type; 1:= SegmentFSM, 2:= SegmentFSM_SP
        uint32_t    _sFirstPage; // first segment index ((C) Nico) page in order to load segment into memory
};
using seg_vt = std::vector<Segment_T>;

std::ostream& operator<< (std::ostream& stream, const Segment_T& aSegmentTuple);