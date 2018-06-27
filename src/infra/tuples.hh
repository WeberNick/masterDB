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
        Partition_T();
        Partition_T(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth); 
        explicit Partition_T(const Partition_T& aPartTuple);
        explicit Partition_T(Partition_T&&) = delete;
        Partition_T& operator=(const Partition_T& aPartitionTuple);
        Partition_T& operator=(Partition_T&&) = delete;
        ~Partition_T() = default;

    public:
        void init(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth) noexcept;
        void toDisk(byte* aPtr) const noexcept;
        void toDisk(byte* aPtr) noexcept;
        void toMemory(byte* aPtr) noexcept;
    
    public:
        //getter
        inline size_t size() const noexcept { return _size; };
        inline size_t size() noexcept { return _size; }
        inline uint8_t ID() const noexcept { return _pID; }
        inline uint8_t ID() noexcept { return _pID; }
        inline const std::string& name() const noexcept { return _pName; }
        inline const std::string& name() noexcept { return _pName; }
        inline const std::string& path() const noexcept { return _pPath; }
        inline const std::string& path() noexcept { return _pPath; }
        inline uint8_t type() const noexcept { return _pType; }
        inline uint8_t type() noexcept { return _pType; }
        inline uint16_t growth() const noexcept { return _pGrowth; }
        inline uint16_t growth() noexcept { return _pGrowth; }
        inline std::string to_string() const noexcept { return std::string("Size: ") + std::to_string(_size) + std::string(", ID: ") + std::to_string(_pID) + std::string(", Name: ") + _pName + std::string(", Path: ") + _pPath + std::string(", Type: ") + std::to_string(_pType) + std::string(", Growth: ") + std::to_string(_pGrowth); }
        inline std::string to_string() noexcept { return static_cast<const Partition_T&>(*this).to_string(); }

    private:
        size_t      _size; // size of class, with all the strings
        /* Tuple content*/
        uint8_t     _pID;
        std::string _pName;
        std::string _pPath;
        uint8_t     _pType;   // 1:= PartitionFile, 2:=partitionRaw
        uint16_t    _pGrowth;
};
using part_vt = std::vector<Partition_T>;
std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple);

class Segment_T
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
        void toDisk(byte* aPtr) const noexcept;
        void toDisk(byte* aPtr) noexcept;
        void toMemory(byte* aPtr) noexcept;
    
    public:
        inline size_t size() const noexcept { return _size; };
        inline size_t size() noexcept { return _size; };
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
        inline std::string to_string() const noexcept { return std::string("Size: ") + std::to_string(_size) + std::string(", Partition ID: ") + std::to_string(_sPID) + std::string(", ID: ") + std::to_string(_sID) + std::string(", Name: ") + _sName + std::string(", Type: ") + std::to_string(_sType) + std::string(", First Page: ") + std::to_string(_sFirstPage); }
        inline std::string to_string() noexcept { return static_cast<const Segment_T&>(*this).to_string(); }
     
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

class Employee_T
{
    public:
        Employee_T();
        Employee_T(const std::string& aName, const double aSalary, const uint8_t aAge); 
        Employee_T(const Employee_T& aEmployeeTuple);
        explicit Employee_T(Employee_T&&) = delete;
        Employee_T& operator=(const Employee_T& aEmployeeTuple);
        Employee_T& operator=(Employee_T&&) = delete;
        ~Employee_T() = default;

    public:
        void init(const std::string& aName, const double aSalary, const uint8_t aAge) noexcept;
        void toDisk(byte* aPtr) const noexcept;
        void toDisk(byte* aPtr) noexcept;
        void toMemory(byte* aPtr) noexcept;
    
    public:
        //getter
        inline size_t size() const noexcept { return _size; };
        inline size_t size() noexcept { return _size; }
        inline uint64_t ID() const noexcept { return _id; }
        inline uint64_t ID() noexcept { return _id; }
        inline uint8_t age() const noexcept { return _age; }
        inline uint8_t age() noexcept { return _age; }
        inline const std::string& name() const noexcept { return _name; }
        inline const std::string& name() noexcept { return _name; }
        inline double salary() const noexcept { return _salary; }
        inline double salary() noexcept { return _salary; }
        inline std::string to_string() const noexcept { return std::string("Employee (ID, Name, Age, Salary) : ") + std::to_string(_id) + std::string(", '") + _name + std::string("', ") + std::to_string(_age) + std::string(", ") + std::to_string(_salary); }
        inline std::string to_string() noexcept { return static_cast<const Employee_T&>(*this).to_string(); }
        
    private:
        static uint64_t _idCount;
        //size of class, with all the strings
        size_t _size;
        /* content of the tuple */
        uint64_t    _id;
        double    _salary;
        uint8_t    _age;
        std::string _name;
        
};
using emp_vt = std::vector<Employee_T>;
std::ostream& operator<< (std::ostream& stream, const Employee_T& aEmpTuple);
