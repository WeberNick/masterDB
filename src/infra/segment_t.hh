
/*********************************************************************
 * @file    segment_t.hh
 * @author 	Nick Weber
 * @date    Mai 07, 2018
 * @brief 	Tuple class for segments. Used for transforming to disk and memory representations
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

#include "tuple.hh"
#include "types.hh"
#include "exception.hh"
#include "trace.hh"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <utility>

class Segment_T : public Tuple
{
    public:
        Segment_T();
        Segment_T(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage);
        explicit Segment_T(const Segment_T& aSegmentTuple);
        explicit Segment_T(Segment_T&&) = delete;
        Segment_T& operator=(const Segment_T& aSegmentTuple);
        Segment_T& operator=(Segment_T&&) = delete;
        ~Segment_T() = default;

    public:
        void init(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage) noexcept;
        void toDisk(byte* aPtr) const noexcept override;
        void toDisk(byte* aPtr) noexcept override;
        void toMemory(byte* aPtr) noexcept override;
    
    public:
        inline uint8_t partID() const noexcept { return _sPID; }
        inline uint8_t partID() noexcept { return _sPID; }
        inline uint16_t ID() const noexcept { return _sID; }
        inline uint16_t ID() noexcept { return _sID; }
        inline const std::string& name() const noexcept { return _sName; }
        inline const std::string& name() noexcept { return _sName; }
        inline uint8_t type() const noexcept { return _sType; }
        inline uint8_t type() noexcept { return _sType; }
        inline uint32_t firstPage() const noexcept { return _sFirstPage; } 
        inline uint32_t firstPage() noexcept { return _sFirstPage; } 
        inline std::string to_string() const noexcept override;
        inline std::string to_string() noexcept override;
     
    private:
        /* Tuple content */
        uint8_t     _sPID;       // partition ID
        uint16_t    _sID;        // segment ID
        std::string _sName;      // segment name (unique)
        uint8_t     _sType;      // segment type; 1:= SegmentFSM, 2:= SegmentFSM_SP
        uint32_t    _sFirstPage; // first segment index ((C) Nico) page in order to load segment into memory
};

std::string Segment_T::to_string() const noexcept 
{ 
    return std::string("Size: ") + std::to_string(_size) 
        + std::string(", Partition ID: ") + std::to_string(_sPID) 
        + std::string(", ID: ") + std::to_string(_sID) 
        + std::string(", Name: ") + _sName 
        + std::string(", Type: ") + std::to_string(_sType) 
        + std::string(", First Page: ") + std::to_string(_sFirstPage); }

std::string Segment_T::to_string() noexcept
{ 
    return static_cast<const Segment_T&>(*this).to_string(); 
}

using seg_vt = std::vector<Segment_T>;
std::ostream& operator<< (std::ostream& stream, const Segment_T& aSegmentTuple);
