/**
 *  @file    partition_manager.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de),
             Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class for managing partitions stored on the disk
 *  @bugs    Currently no bugs known
 *  @todos   TBD
 *  @section TBD
 */

#ifndef PARTITION_MANAGER_HH
#define PARTITION_MANAGER_HH

#include "infra/types.hh"
#include "partition_base.hh"
#include "partition_file.hh"
#include "partition_raw.hh"

#include <map>

class PartitionManager
{    
    public:
        explicit PartitionManager();
        PartitionManager(const PartitionManager& aPartitionManager) = delete;
        PartitionManager& operator=(const PartitionManager& aPartitionManager) = delete;
        ~PartitionManager(); // delete all partitions

    public:
        /* creates instance of partition; creation of partition on disk happens in class File_Partition */
        PartitionFile* createPartitionFileInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator);
        PartitionRaw* createPartitionRawInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize);
        void addPartitionInstance(PartitionFile* aPartition);
        void addPartitionInstance(PartitionRaw* aPartition);
  
    public:
        uint getNoPartitions();
        PartitionBase* getPartition(const uint8_t aID);
  
    private:
        uint _counterPartitionID;
        std::map <uint, PartitionBase*> _partitions;
};

#endif