#include "partition_manager.hh"

PartitionManager::PartitionManager() :
    _counterPartitionID(1),
    _partitions(),
    _partitionsByID(),
    _partitionsByName(),
    _partitionTuples()
{}

PartitionManager::~PartitionManager()
{
    // iterate over map and delete every map item (PartitionBase)
    std::map<uint, PartitionBase*>::iterator it;
    for(it = _partitions.begin(); it != _partitions.end(); ++it) {
        delete it->second;
    }
}

void PartitionManager::load(part_vt& aTuples)
{
    _partitionTuples = aTuples;
    //fill internal data structure with all relevant info
    for(auto& partTuple : _partitionTuples)
    {
      _partitionsByID[partTuple._pID] = &partTuple;
      _partitionsByName[partTuple._pName] = &partTuple;
    }
}


PartitionFile* PartitionManager::createPartitionFileInstance(const std::string aPath, const std::string aName, const uint aGrowthIndicator, const control_block_t& aControlBlock)
{
    PartitionFile* lPartition = new PartitionFile(aPath, aName, aGrowthIndicator, _counterPartitionID++, aControlBlock);
    _partitions[lPartition->getID()] = lPartition;
    return (PartitionFile*)_partitions.at(lPartition->getID());
}

PartitionRaw* PartitionManager::createPartitionRawInstance(const std::string aPath, const std::string aName, const control_block_t& aControlBlock)
{
    PartitionRaw* lPartition = new PartitionRaw(aPath, aName, _counterPartitionID++, aControlBlock);
    _partitions[lPartition->getID()] = lPartition;
    return (PartitionRaw*)_partitions.at(lPartition->getID());
}
