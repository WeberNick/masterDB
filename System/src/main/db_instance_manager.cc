#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager(const std::string aPathToMasterPartition) :
    _masterPartition(aPathToMasterPartition, "MasterPartition", 4096, 10, 0),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(2)
{
  //think of reserver page.. 
}


DatabaseInstanceManager::~DatabaseInstanceManager()
{}


void DatabaseInstanceManager::install()
{
	//todo

}

void DatabaseInstanceManager::boot()
{
  part_vt aPartitionTuples;
  seg_vt aSegmentTuples;
  load<part_t>(aPartitionTuples, _partIndex);
  load<seg_t>(aSegmentTuples, _segIndex);

  _partMngr.load(aPartitionTuples);
  _segMngr.load(aSegmentTuples);
}

void DatabaseInstanceManager::shutdown()
{
	part_vt aPartitionTuples = _partMngr.getPartitionTuples();
  seg_vt aSegmentTuples = _segMngr.getSegmentTuples();

  


}


