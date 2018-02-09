#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager(const std::string aPathToMasterPartition) :
	_pathToMasterPart(aPathToMasterPartition),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance())
{}

DatabaseInstanceManager::~DatabaseInstanceManager()
{}



void DatabaseInstanceManager::boot()
{
	part_vt lPartitionTuples;
	seg_vt lSegmentTuples;
	
	
	//create MasterSegmentNSM, read it, fill vector with tuple
	


	_partMngr.load(lPartitionTuples);
	_segMngr.load(lSegmentTuples);
}
