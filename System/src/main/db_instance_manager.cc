#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager(const control_block_t& aControlBlock) :
    _masterPartition(aControlBlock.mstrPart(), "MasterPartition", 10, 0, aControlBlock),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(2)
{
  //think of reserver page.. 
}


DatabaseInstanceManager::~DatabaseInstanceManager()
{}


void DatabaseInstanceManager::install(std::string aPath, uint aGrowthIndicator, control_block_t& aControlBlock)
{
  part_t lMasterPartitionTuple;
  PartitionBase* lMaster =   _partMngr.createMasterPartition(aPath,aGrowthIndicator,aControlBlock,lMasterPartitionTuple);
  _segMngr.createMasterSegments(aControlBlock,lMaster);
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


