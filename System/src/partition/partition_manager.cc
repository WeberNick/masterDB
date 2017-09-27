#include "partition_manager.hh"

PartitionManager::PartitionManager() : _counterPartitionID(0), _partitions() {}

PartitionManager::~PartitionManager(){
  // iterate over map and delete every map item (FilePartition)
  std::map<uint, FilePartition*>::iterator it;
  for(it = _partitions.begin(); it !=_partitions.end(); ++it) {
    delete it->second;
    _partitions.erase(it);
  }
}

FilePartition* PartitionManager::createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator){
  FilePartition* partition = new FilePartition(aPath, aPartitionSize, aPageSize, aGrowthIndicator, _counterPartitionID++);
  _partitions[partition->getPartitionID()] = partition;
  return _partitions.at(partition->getPartitionID());
}

void PartitionManager::addPartitionInstance(FilePartition* aPartition) {
  _partitions[aPartition->getPartitionID()] = aPartition;
}

int PartitionManager::getNoPartitions(){
  return _partitions.size();
}

FilePartition* PartitionManager::getPartition(const uint aID) {
  return _partitions.at(aID);
}