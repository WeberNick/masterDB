#include "partition_manager.hh"

PartitionManager::PartitionManager() :
    _counterPartitionID(0),
    _partitions()
{}

PartitionManager::~PartitionManager()
{
    // iterate over map and delete every map item (PartitionBase)
    std::map<uint, PartitionBase*>::iterator it;
    for(it = _partitions.begin(); it != _partitions.end(); ++it) {
        delete it->second;
    }
}

PartitionFile* PartitionManager::createPartitionFileInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator)
{
    PartitionFile* lPartition = new PartitionFile(aPath, aPartitionSize, aPageSize, aGrowthIndicator, _counterPartitionID++);
    _partitions[lPartition->getID()] = lPartition;
    return (PartitionFile*)_partitions.at(lPartition->getID());
}

PartitionRaw* PartitionManager::createPartitionRawInstance(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize)
{
    PartitionRaw* lPartition = new PartitionRaw(aPath, aPartitionSize, aPageSize, _counterPartitionID++);
    _partitions[lPartition->getID()] = lPartition;
    return (PartitionRaw*)_partitions.at(lPartition->getID());
}

void PartitionManager::addPartitionInstance(PartitionFile* aPartition)
{
    _partitions[aPartition->getID()] = aPartition;
}

void PartitionManager::addPartitionInstance(PartitionRaw* aPartition)
{
    _partitions[aPartition->getID()] = aPartition;
}

uint PartitionManager::getNoPartitions()
{
    return _partitions.size();
}

PartitionBase* PartitionManager::getPartition(const uint8_t aID)
{
    return _partitions.at(aID);
}