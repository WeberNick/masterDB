/**
 * @file    partition_t.hh
 * @author 	Nick Weber
 * @date    Mai 07, 2018
 * @brief 	Tuple class for partitions. Used for transforming to disk and memory representations
 * @bugs 	Currently no bugs known
 * @todos 	-
 * 
 * @section	DESCRIPTION
 *     TODO
 */

#pragma once

//#include "../partition/partition_manager.hh"
#include "tuple.hh"
#include "types.hh"
#include "exception.hh"
#include "trace.hh"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <utility>

class Partition_T : public Tuple
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
        /**
         * @brief 
         * 
         * @param aPID 
         * @param aName 
         * @param aPath 
         * @param aType 
         * @param aGrowth 
         */
        void init(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth) noexcept;
        /**
         * @brief 
         * 
         * @param aPtr 
         */
        void toDisk(byte* aPtr) const noexcept override;
        void toDisk(byte* aPtr) noexcept override;
        /**
         * @brief 
         * 
         * @param aPtr 
         */
        void toMemory(byte* aPtr) noexcept override;
    
    public:
        static inline string_vt attributes() noexcept { return {"ID", "Name", "Path", "Type", "Growth"}; }
        // Getter
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
        /**
         * @brief 
         * 
         * @return std::string 
         */
        inline std::string to_string() const noexcept override; 
        inline std::string to_string() noexcept override; 

        inline string_vt values() const { return {std::to_string(_pID), _pName, _pPath, partTypeToString(_pType), std::to_string(_pGrowth)}; }

    private:
        /* Tuple content*/
        uint8_t     _pID;
        std::string _pName;
        std::string _pPath;
        uint8_t     _pType;   // 1:= PartitionFile, 2:=partitionRaw
        uint16_t    _pGrowth;
};

std::string Partition_T::to_string() const noexcept 
{ 
    return std::string("Size: ") + std::to_string(_size) 
        + std::string(", ID: ") + std::to_string(_pID) 
        + std::string(", Name: ") + _pName 
        + std::string(", Path: ") + _pPath 
        + std::string(", Type: ") + std::to_string(_pType) 
        + std::string(", Growth: ") + std::to_string(_pGrowth); 
}

std::string Partition_T::to_string() noexcept 
{ 
    return static_cast<const Partition_T&>(*this).to_string(); 
}

using part_vt = std::vector<Partition_T>;
std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple);
