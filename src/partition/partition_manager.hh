/**
 *  @file   partition_manager.hh
 *  @author Nicolas Wipfler (nwipfler@mail.uni-mannheim.de),
 *          Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Singleton class managing all partitions
 *  @bugs   Currently no bugs known
 *  @todos  Currently no todos
 *
 *  @section DESCRIPTION
 *  This class manages all partition objects associated with a physical partition (raw or file) on disk.
 *  If a new partition must be created, this is done through the single instance of this class by calling
 *  createPartition[Type]Instance(...). Because the partitions constructors are private, this is the only
 *  way to create a new partition.
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/tuples.hh"
#include "partition_base.hh"
#include "partition_file.hh"
#include "partition_raw.hh"

#include <unordered_map>
#include <string>

class PartitionManager
{    
    private:
        friend class DatabaseInstanceManager;
        PartitionManager();
        explicit PartitionManager(const PartitionManager&) = delete;
        explicit PartitionManager(PartitionManager&&) = delete;
        PartitionManager& operator=(const PartitionManager&) = delete;
        PartitionManager& operator=(PartitionManager&&) = delete;
        ~PartitionManager(); // delete all partitions

    public:
        /**
         *  @brief  This function is the only way to get access to the PartitionManager instance
         *  @return reference to the only PartitionManager instance
         */
        static PartitionManager& getInstance() noexcept
        {
            static PartitionManager lPartitionManagerInstance;
            return lPartitionManagerInstance;
        }
        void init(const CB& aControlBlock);

    public:
        void load(const part_vt& aTuples);

    public:
        /* creates instance of partition; creation of partition on disk happens in the respective partition class */
        PartitionFile*  createPartitionFileInstance(const std::string& aPath, const std::string& aName, const uint16_t aGrowthIndicator);
        PartitionRaw*   createPartitionRawInstance(const std::string& aPath, const std::string& aName);
        PartitionBase*  getPartition(const uint8_t aID);
        PartitionBase*  getPartition(const std::string& aName);
        Partition_T&    getPartitionT(const std::string& aName);
        uint8_t         getPartitionID(const std::string& aName);
        const string_vt getPartitionNames();
        std::string     getPartitionName(const uint8_t aID);
        
        void            deletePartition(const uint8_t aID);
        void            deletePartition(const std::string& aName);

    public:
        inline size_t   getNoPartitions() { return _partitions.size(); }

    private:
        void            createPartitionSub(const Partition_T& aParT); // has some issues if aParT is a const reference
        PartitionFile*  createMasterPartition(const Partition_T& aPart);
        /* install functionality */
        PartitionFile*  createMasterPartition(const std::string& aPath, const uint aGrowthIndicator, Partition_T& aMasterTuple);
        void            insertMasterPartitionTuple(const Partition_T& aMasterTuple);

    private:
        uint8_t                                     _counterPartitionID;
        std::unordered_map<uint8_t, PartitionBase*> _partitions;
        std::unordered_map<uint8_t, Partition_T>    _partitionsByID;
		std::unordered_map<std::string, uint8_t>    _partitionsByName;

        const std::string _masterPartName;
        const std::string _masterSegPartName;

        const CB*   _cb;
};
