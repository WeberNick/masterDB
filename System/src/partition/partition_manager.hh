/**
 *  @file    partition_manager.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   A class for managing partitions stored on the disk
 *  @bugs    Currently no bugs known
 *  @todos   TBD
 *  @section TBD
 */

#ifndef PARTITION_MANAGER_HH
#define PARTITION_MANAGER_HH

#include "infra/types.hh"
#include "partition_file.hh"

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
        PartitionFile* createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator);
        void addPartitionInstance(PartitionFile* partition);
  
    public:
        int getNoPartitions();
        PartitionFile* getPartition(const uint aID);
  
    private:
        uint _counterPartitionID;
        std::map <uint, PartitionFile*> _partitions;
};

#endif