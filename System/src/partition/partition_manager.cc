#include "partition_manager.hh"

PartitionManager::PartitionManager(): _partitions(NULL) {}

PartitionManager::~PartitionManager(): {
  // iterate over map and delete every map item (Partition)
  map<uint, Partition*>::iterator it;
  for(auto it = _partitions.cbegin(); it !=_partitions.cend();) {
    _partitions.erase(it++);
  }
}

int PartitionManager::createPartitionInstance(Partition* partition) {
  _partitions[partition->getPartitionID()] = partition;
  return _partitions.at(id);
}

int PartitionManager::createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aBlockSize, const uint aGrowthIndicator, const mode_t aAccessRights){
  Partition* partition = new Partition(aPath, aPartitionSize, aBlockSize, aGrowthIndicator, aAccessRights);
  _partitions[partition->getPartitionID()] = partition;
  return _partitions.at(id);
}

int PartitionManager::getNoPartitions(){
  return _partitions.size();
}

Partition* PartitionManager::getPartition(const uint aID) {
  return _partitions.at(aID);
}