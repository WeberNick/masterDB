#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager() :
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance())
{}

DatabaseInstanceManager::~DatabaseInstanceManager()
{}