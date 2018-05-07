#include "partition_manager.hh"
#include "../segment/segment_manager.hh"
#include "../segment/segment_fsm_sp.hh"


PartitionManager::PartitionManager() :
    _counterPartitionID(1),
    _partitions(),
    _partitionsByID(),
    _partitionsByName(),
    _masterPartName("MasterPartition"),
    _masterSegPartName("PartitionMasterSegment"),
    _cb(nullptr),
    _init(false)
{}

PartitionManager::~PartitionManager()
{
    // iterate over map and delete every map item (PartitionBase)
    for(auto it = _partitions.begin(); it != _partitions.end(); ++it) {
        delete it->second;
    }
}

void PartitionManager::init(const CB& aControlBlock)
{
    if(!_init)
    {
        _cb = &aControlBlock;
        _init = true;
    }
}

void PartitionManager::load(const part_vt& aTuples)
{
    //fill internal data structure with all relevant info
    for(const auto& partTuple : aTuples)
    {
      _partitionsByID[partTuple.partID()] = partTuple;
      _partitionsByName[partTuple.name()] = partTuple.partID();
    }
}


PartitionFile* PartitionManager::createPartitionFileInstance(const std::string aPath, const std::string aName, const uint aGrowthIndicator)
{
    uint pType = 1;
    PartitionFile* lPartition = new PartitionFile(aPath, aName, aGrowthIndicator, _counterPartitionID++, *_cb);
    _partitions[lPartition->getID()] = lPartition;

    Partition_T t(lPartition->getID(),aName,aPath,pType,aGrowthIndicator);

    createPartitionSub(t);

    TRACE(std::string("File partition instance created."));
    return (PartitionFile*)_partitions.at(lPartition->getID());
}

PartitionRaw* PartitionManager::createPartitionRawInstance(const std::string aPath, const std::string aName)
{
    PartitionRaw* lPartition = new PartitionRaw(aPath, aName, _counterPartitionID++, *_cb);
    _partitions[lPartition->getID()] = lPartition;

    uint pType = 0;

    part_mem_t t = {lPartition->getID(),aName,aPath,pType,MAX32};

    createPartitionSub(t);

    TRACE("Raw partition instance created.");
    return (PartitionRaw*)_partitions.at(lPartition->getID());
}

void PartitionManager::createPartitionSub(const Partition_T& aParT){
    SegmentManager& lSegMan = SegmentManager::getInstance();
    _partitionsByID[aParT.partID()] = aParT;
    _partitionsByName[aParT.name()] = aParT.partID();

    SegmentFSM_SP* lSeg = (SegmentFSM_SP*) lSegMan.getSegment(_masterSegPartName);
    
    lSeg->insertTuple<Partition_T>(aParT);
}

PartitionBase* PartitionManager::getPartition(const uint8_t aID)
{
    //if the object has not been created before
    if (_partitions.find(aID)==_partitions.end()) {
        TRACE("Trying to get Partition from Disk");
        const Partition_T* lTuple = &_partitionsByID[aID];
        PartitionBase* s;
        switch(lTuple->type()){
            case 1://PartitionFile
            s = new PartitionFile(lTuple, *_cb);
            break;
            case 2: //PartitionRaw
            s = new PartitionRaw(lTuple, *_cb);
            break;

            default: return nullptr;
        }
        _partitions[lTuple->partID()]=s;
    }
    TRACE("found Partition, its ID is "+std::to_string(aID));
    return _partitions.at(aID);
}

PartitionBase* PartitionManager::getPartition(const std::string& aName){
    return getPartition(_partitionsByName[aName]);
}

void PartitionManager::deletePartition(const uint8_t aID){
    //delete object if exists
    auto lIter =  _partitions.find(aID);
    if( lIter !=_partitions.end()){
        delete lIter->second;
        _partitions.erase(lIter);
    }
    const Partition_T lpart = _partitionsByID[aID];
    //delete tuple on disk
    SegmentManager& lSegMan = SegmentManager::getInstance();
    lSegMan.deleteTupelPhysically<Partition_T>(_masterSegPartName,aID);

    //delete tuple in memory
    _partitionsByName.erase(lpart.name());
    _partitionsByID.erase(aID);
    TRACE("Partition deleted successfully.");
}

void PartitionManager::deletePartition(const std::string& aName){
    deletePartition(_partitionsByName[aName]);
}

PartitionFile* PartitionManager::createMasterPartition(const Partition_T* aPart)
{
   return new PartitionFile(aPart, *_cb); 
}

PartitionFile* PartitionManager::createMasterPartition(const std::string& aPath, const uint aGrowthIndicator, Partition_T& aMasterTuple){
     uint pType = 1;
    PartitionFile* lPartition = new PartitionFile(aPath, _masterPartName, aGrowthIndicator, _counterPartitionID++, *_cb);
    _partitions[lPartition->getID()] = lPartition;
    TRACE(" ");
    Partition_T* t = new Partition_T(lPartition->getID(),_masterPartName,aPath,pType,aGrowthIndicator);
    aMasterTuple = *t;

    TRACE("Master partition created successfully.");
    return lPartition;
}

void PartitionManager::insertMasterPartitionTuple(const Partitiopn_T& aMasterTuple){
    //insert Tuple in Segment
    createPartitionSub(aMasterTuple);
}
