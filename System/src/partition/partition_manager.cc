#include "partition_manager.hh"

PartitionManager::PartitionManager() : _partitions() {}

PartitionManager::~PartitionManager(){
  // iterate over map and delete every map item (Partition)
  std::map<uint, Partition*>::iterator it;
  for(it = _partitions.begin(); it !=_partitions.end(); ++it) {
    delete it->second;
    _partitions.erase(it);
  }
}

// Partition* PartitionManager::createPartitionInstance(Partition* partition) {
//   _partitions[partition->getPartitionID()] = partition;
//   return _partitions.at(partition->getPartitionID());
// }

Partition* PartitionManager::createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aBlockSize, const uint aGrowthIndicator, const mode_t aAccessRights){
  Partition* partition = new Partition(aPath, aPartitionSize, aBlockSize, aGrowthIndicator, aAccessRights);
  _partitions[partition->getPartitionID()] = partition;
  return _partitions.at(partition->getPartitionID());
}

int PartitionManager::getNoPartitions(){
  return _partitions.size();
}

Partition* PartitionManager::getPartition(const uint aID) {
  return _partitions.at(aID);
}