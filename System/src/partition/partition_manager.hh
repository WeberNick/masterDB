/**
 *  @file    partition_manager.hh
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   A class for managing partitions stored on the disk
 *  @bugs    Currently no bugs known
 *  @todos   -
 *  @section -
 */

#ifndef PARTITION_MANAGER_HH
#define PARTITION_MANAGER_HH

#include "infra/types.hh"
#include "file_partition.hh"

#include <map>

class PartitionManager {
  public:
    PartitionManager();
    ~PartitionManager();

  public:
    // creates instance of partition; creation of partition on disk happens in class FilePartition
    FilePartition* createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator);
    void addPartitionInstance(FilePartition* partition);
  
  public:
    int getNoPartitions();
    FilePartition* getPartition(const uint aID);
  
  private:
    uint _counterPartitionID;
    std::map <uint, FilePartition*> _partitions;
};

#endif