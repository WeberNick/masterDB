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
//	_partMngr.load(lFile, 1);
	_segMngr.load(_masterPartition, 2);
}

void DatabaseInstanceManager::shutdown()
{
	//todo


}


