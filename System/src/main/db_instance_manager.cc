#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager(const std::string aPathToMasterPartition) :
	_pathToMasterPart(aPathToMasterPartition),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance())
{}

DatabaseInstanceManager::~DatabaseInstanceManager()
{}


void DatabaseInstanceManager::install()
{
	//todo

}

void DatabaseInstanceManager::boot()
{
	part_vt lPartitionTuples;
	seg_vt lSegmentTuples;
	
	
	//create NSMsegment object for partitions 
	//read it in from page _locationPartSeg,
	//fill vector with tuple all its tuples
	//hand over to partMngr by
	_partMngr.load(lPartitionTuples);

	//create NSMsegment object for segments 
	//read it in from page _locationSegSeg,
	//fill vector with tuple all its tuples
	//hand over to segMngr by
	_segMngr.load(lSegmentTuples);
}

void DatabaseInstanceManager::shutdown()
{
	//todo


}


