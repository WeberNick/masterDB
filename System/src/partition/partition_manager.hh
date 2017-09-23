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

#include <map>
#include "infra/types.hh"
#include "partition.hh"

class PartitionManager {
  public:
    PartitionManager();
    ~PartitionManager();

  public:
    void createPartition();
    Partition& getPartition(const uint id);
  
  public:
    static int getNoPartitions();
  
  private:
    std::map <uint, Partition&> partitions;

  private:
    static uint _noPartitions;
};

#endif