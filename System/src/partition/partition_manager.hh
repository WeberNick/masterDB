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
#include "partition.hh"

#include <map>

class PartitionManager {
  public:
    PartitionManager();
    ~PartitionManager();

  public:
    // creates instance of partition; creation of partition on disk happens in class Partition
    int createPartitionInstance();
  
  public:
    int getNoPartitions();
    Partition* getPartition(const uint aID);
  
  private:
    std::map <uint, Partition*> _partitions;
};

#endif