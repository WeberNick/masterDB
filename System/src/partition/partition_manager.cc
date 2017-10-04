#include "partition_manager.hh"

PartitionManager::PartitionManager() :
    _counterPartitionID(0),
    _partitions()
{}

PartitionManager::~PartitionManager()
{
    // iterate over map and delete every map item (PartitionFile)
    std::map<uint, PartitionFile*>::iterator it;
    for(it = _partitions.begin(); it != _partitions.end(); ++it) {
        delete it->second;
    }
}

PartitionFile* PartitionManager::createPartitionInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator)
{
    PartitionFile* partition = new PartitionFile(aPath, aPartitionSize, aPageSize, aGrowthIndicator, _counterPartitionID++);
    _partitions[partition->getID()] = partition;
    return _partitions.at(partition->getID());
}

void PartitionManager::addPartitionInstance(PartitionFile* aPartition)
{
    _partitions[aPartition->getID()] = aPartition;
}

int PartitionManager::getNoPartitions()
{
    return _partitions.size();
}

PartitionFile* PartitionManager::getPartition(const uint aID)
{
    return _partitions.at(aID);
}