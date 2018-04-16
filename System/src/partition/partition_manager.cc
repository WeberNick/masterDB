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
    for(auto it = _partitions.begin(); it != _partitions.end(); ++it) {
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
    int pType = 1;
    PartitionFile* lPartition = new PartitionFile(aPath, aName, aGrowthIndicator, _counterPartitionID++, aControlBlock);
    _partitions[lPartition->getID()] = lPartition;

    part_t lp = {lPartition->getID(),aPath,aName,pType,aGrowthIndicator};
    createPartitionSub(lp);

    return (PartitionFile*)_partitions.at(lPartition->getID());
}

PartitionRaw* PartitionManager::createPartitionRawInstance(const std::string aPath, const std::string aName, const control_block_t& aControlBlock)
{
    PartitionRaw* lPartition = new PartitionRaw(aPath, aName, _counterPartitionID++, aControlBlock);
    _partitions[lPartition->getID()] = lPartition;

    int pType = 0;

    part_t lp = {lPartition->getID(),aPath,aName,pType,0};
    createPartitionSub(lp);

    return (PartitionRaw*)_partitions.at(lPartition->getID());
}

void PartitionManager::createPartitionSub(part_t aParT){
   /* SegmentManager& lSegMan = SegmentManager::getInstance();
    _partitionTuples.push_back(aParT);
    _partitionsByID[aParT._pID]=&_partitionTuples[_partitionTuples.size()-1];
    _partitionsByName[aParT._pName]=&_partitionTuples[_partitionTuples.size()-1];


    SegmentFSM_SP* lSeg = (SegmentFSM_SP*) lSegMan.getSegment(_masterSegPart);
    lSeg->insertTuple((byte*) &aParT,sizeof(part_t));*/
}

PartitionBase* PartitionManager::getPartition(const uint8_t aID)
{
    //if the object has not been created before
    if (_partitions.find(aID)==_partitions.end()) {
        //your screwed.
    }
    return _partitions.at(aID);
}

PartitionBase* PartitionManager::getPartition(const std::string aName){
    return getPartition(_partitionsByName[aName]->_pID);
}
