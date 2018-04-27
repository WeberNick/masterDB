#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager() :
    _path(),
    _masterPartition(nullptr),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(2),
    _cb(nullptr),
    _running(false),
    _init(false)
{
  //think of reserver page.. 
}


DatabaseInstanceManager::~DatabaseInstanceManager()
{
    shutdown();
}

void DatabaseInstanceManager::init(const CB& aControlBlock)
{
    if(!_init)
    {
        _cb = &aControlBlock;
        _path = _cb->mstrPart() + std::string("MasterPartition");
        if(_cb->install())
        {
            install();
        }
        else
        {
            boot();
        }
        _running = true;
        _init = true;
    }
}


void DatabaseInstanceManager::install()
{
  part_t lMasterPartitionTuple;
  _masterPartition =   _partMngr.createMasterPartition(_path, 1000,lMasterPartitionTuple);
  _segMngr.createMasterSegments(_masterPartition, _partMngr._masterSegPartName);
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
  _masterPartition = (PartitionFile*)_partMngr.getPartition(_partMngr._masterPartName);
}

void DatabaseInstanceManager::shutdown()
{
    if (isRunning()) {
        // stop transactions
        _segMngr.storeSegments();
        BufferManager::getInstance().flushAll();
        _running = false;
    }
}
