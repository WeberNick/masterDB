#include "partition_manager.hh"

PartitionManager::PartitionManager(): _noPartitions(0) {}

Partition& PartitionManager::createPartition(const uint id){
  Partition& partition = new Partition()
  partitions[id] = partition;
  return partitions.at(id);
}