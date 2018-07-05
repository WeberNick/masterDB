/**
 *  @file   partition_manager.hh
 *  @author Nicolas Wipfler (nwipfler@mail.uni-mannheim.de),
 *          Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *          Jonas Thietke
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
#include "../infra/file_util.hh"
#include "../infra/partition_t.hh"
#include "partition_base.hh"
#include "partition_file.hh"
#include "partition_raw.hh"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <utility>

constexpr uint16_t MIN_GROWTH_INDICATOR = 8;

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
        void init(const CB& aControlBlock) noexcept;

    public:
        void load(const part_vt& aTuples);

    public:
        /* creates instance of partition; creation of partition on disk happens in the respective partition class */
        std::pair<PartitionFile*, bool>  createPartitionFileInstance(const std::string& aPath, const std::string& aName, const uint16_t aGrowthIndicator);
        std::pair<PartitionRaw*, bool>   createPartitionRawInstance(const std::string& aPath, const std::string& aName);

        void                deletePartition(const uint8_t aID);
        void                deletePartition(const std::string& aName);

    public:
        /**
         * @brief   not just a getter, also loads the partition if it was not loaded before
         * @param   aID - ID of the partition to be retrieved
         * @return  pointer to the partition object
         */
        PartitionBase*      getPartition(const uint8_t aID);
        /**
         * @brief   not just a getter, also loads the partition if it was not loaded before.
         *          looks up ID to corresponding name and than call getPartition (aID)
         * @param   aName - Name of the partition to be retrieved
         * @return  pointer to the partition object
         */
        PartitionBase*      getPartition(const std::string& aName);
        
        const Partition_T&  getPartitionT(const uint8_t aID) const;
        Partition_T&        getPartitionT(const uint8_t aID);
        const Partition_T&  getPartitionT(const std::string& aName) const;
        Partition_T&        getPartitionT(const std::string& aName);
        uint8_t             getPartitionID(const std::string& aName);
        string_vt           getPartitionNames() noexcept;
        std::string         getPartitionName(const uint8_t aID);

        inline size_t       getNoPartitions() const noexcept { return _partitions.size(); }
        inline size_t       getNoPartitions() noexcept { return _partitions.size(); }
        inline const std::string& getPathForPartition(const std::string& aName) const { return _partitionsByID.at(_partitionsByName.at(aName)).path(); }
        inline const std::string& getMasterPartName() const noexcept { return _masterPartName; }
        
    private:
        void            createPartitionSub(const Partition_T& aParT); // has some issues if aParT is a const reference
        PartitionFile*  createMasterPartition(const Partition_T& aPart);
        /* install functionality */
        PartitionFile*  createMasterPartition(const std::string& aPath, const uint aGrowthIndicator, Partition_T& aMasterTuple);
        void            insertMasterPartitionTuple(const Partition_T& aMasterTuple);

    private:
        inline const std::string& masterPartName(){ return _masterPartName; }
        inline const std::string& masterSegPartName(){ return _masterSegPartName; }

    private:
        uint8_t                                     _counterPartitionID;
        std::unordered_map<uint8_t, PartitionBase*> _partitions;
        std::unordered_map<uint8_t, Partition_T>    _partitionsByID;
		std::unordered_map<std::string, uint8_t>    _partitionsByName;

        const std::string  _masterPartName;
        const std::string  _masterSegPartName;

        const CB*   _cb;
};
