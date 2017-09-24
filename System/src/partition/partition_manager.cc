#include "partition_manager.hh"

PartitionManager::PartitionManager(): _partitions(NULL) {}

PartitionManager::~PartitionManager(): {
  // iterate over map and delete every map item (Partition)
}

int PartitionManager::createPartitionInstance(){
  Partition* partition = new Partition();
  _partitions[partition->getPartitionID()] = partition;
  return _partitions.at(id);
}

int PartitionManager::getNoPartitions(){
  return _partitions.size();
}