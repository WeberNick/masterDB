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

#include "infra/types.hh"
#include "infra/exception.hh"
#include "infra/trace.hh"
#include "partition_base.hh"
#include "partition_file.hh"
#include "partition_raw.hh"

#include <map>
#include <string>

class PartitionManager
{    
    private:
        explicit PartitionManager();
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
        static PartitionManager& getInstance()
        {
            static PartitionManager lPartitionManagerInstance;
            return lPartitionManagerInstance;
        }

        void init(const CB& aControlBlock);

    public:
        void load(part_vt& aTuples);

    public:
        /* creates instance of partition; creation of partition on disk happens in the respective partition class */
        PartitionFile*   createPartitionFileInstance(const std::string aPath, const std::string aName, const uint aGrowthIndicator);
        PartitionRaw*    createPartitionRawInstance(const std::string aPath, const std::string aName);
        PartitionBase*   getPartition(const uint8_t aID);
        PartitionBase*   getPartition(const std::string aName);
        void             deletePartition(const uint8_t aID);
        void             deletePartition(const std::string aName);

        PartitionBase*  createMasterPartition(std::string aPath, uint aGrowthIndicator, part_t& aMasterTuple);
        int             insertMasterPartitionTuple(part_t aMasterTuple);

    public:
        inline size_t           getNoPartitions(){ return _partitions.size(); }
		inline const part_vt&   getPartitionTuples(){ return _partitionTuples; }
        inline void             setInstalled(){_installed=2;}
  private:
  void  createPartitionSub(part_t aParT);

    private:
        uint _counterPartitionID;
        std::map<uint8_t, PartitionBase*> _partitions;
        std::map<uint16_t, part_t*> _partitionsByID;
		std::map<std::string, part_t*> _partitionsByName;
        part_vt _partitionTuples;

        std::string _masterSegPart = "partitionMaster";

        uint8_t _installed=0; //counter of installation steps, if 2 completed

        const CB*   _cb;
        bool        _init;

};
