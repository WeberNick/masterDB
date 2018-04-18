#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager() :
    _masterPartition(nullptr),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(2),
    _cb(nullptr),
    _init(false)
{
  //think of reserver page.. 
}


DatabaseInstanceManager::~DatabaseInstanceManager()
{}

void DatabaseInstanceManager::init(const bool aInstall, const CB& aControlBlock)
{
    if(!_init)
    {
        if(aInstall)
        {
            //install...
        }
        _masterPartition = nullptr; //change to actual master partition file
        _cb = &aControlBlock;
        _init = true;
    }
}


void DatabaseInstanceManager::install(std::string aPath, uint aGrowthIndicator)
{
  part_t lMasterPartitionTuple;
  PartitionBase* lMaster =   _partMngr.createMasterPartition(aPath,aGrowthIndicator,lMasterPartitionTuple);
  _segMngr.createMasterSegments(lMaster);
  _partMngr.insertMasterPartitionTuple(lMasterPartitionTuple);
}

void DatabaseInstanceManager::boot()
{
  part_vt aPartitionTuples;
  seg_vt aSegmentTuples;
  load<part_t>(aPartitionTuples, _partIndex);
  load<seg_t>(aSegmentTuples, _segIndex);

  _partMngr.load(aPartitionTuples);
  _segMngr.load(aSegmentTuples);

  //set installed
  _partMngr.setInstalled();
  _segMngr.setInstalled();

}

void DatabaseInstanceManager::shutdown()
{
  //stop transactions
  //SegMan.storeSegemnts()
  //BufMan.flushAll()

}


