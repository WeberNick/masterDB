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

class Partition_T
{
    public:
        explicit Partition_T();
        explicit Partition_T(const uint32_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth); 
        explicit Partition_T(const Partition_T&) = delete;
        explicit Partition_T(Partition_T&&) = delete;
        Partition_T& operator=(const Partition_T&) = delete;
        Partition_T& operator=(Partition_T&&) = delete;
        ~Partition_T() = default;

    public:
        inline size_t size() const { return _size; };
        void init(const uint32_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth);
        void toDisk(byte* aPtr);
        void toMemory(byte* aPtr);
    
    public:
        //getter
        inline uint32_t ID() const { return _pID; }
        inline const std::string& name() const { return _pName; }
        inline const std::string& path() const { return _pPath; }
        inline uint8_t type() const { return _pType; }
        inline uint16_t growth() const { return _pGrowth; }
        
        //setter
    
    private:
        //size of class, with all the strings
        size_t _size;
        
    //content of the tuple
    private:
        uint32_t    _pID;
        std::string _pName;
        std::string _pPath;
        uint8_t     _pType;   // 1:= PartitionFile, 2:=partitionRaw
        uint16_t    _pGrowth;
};
std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple);



class Segment_T
{
    public:
        explicit Segment_T() = delete;
        explicit Segment_T(const Segment_T&) = delete;
        explicit Segment_T(Segment_T&&) = delete;
        Segment_T& operator=(const Segment_T&) = delete;
        Segment_T& operator=(Segment_T&&) = delete;
        ~Segment_T();
    
    public:
        //getter
        
        //setter
        

    private:
        
};


