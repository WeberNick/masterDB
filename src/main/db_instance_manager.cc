#include "db_instance_manager.hh"

DatabaseInstanceManager::DatabaseInstanceManager() :
    _path(),
    _masterPartition(nullptr),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(3),
    _cb(nullptr),
    _running(false),
    _init(false)
{
  //think of reserver page.. 
}


DatabaseInstanceManager::~DatabaseInstanceManager()
{
    TRACE("DB Instance manager destructed.");
   // shutdown();
}

void DatabaseInstanceManager::init(const CB& aControlBlock)
{
    if(!_init)
    {
        _cb = &aControlBlock;
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
    TRACE("installing database");
  part_mem_t lMasterPartitionTuple;
  _masterPartition =   _partMngr.createMasterPartition(_cb->mstrPart(), 1000,lMasterPartitionTuple);
  _segMngr.createMasterSegments(_masterPartition, _partMngr._masterSegPartName);
  _partMngr.insertMasterPartitionTuple(lMasterPartitionTuple);
}

void DatabaseInstanceManager::boot()
{
    TRACE("booting");
  part_vt aPartitionTuples;
  seg_vt aSegmentTuples;
  load<part_mem_t>(aPartitionTuples, _partIndex);
  TRACE("partition Tuples loaded");
  load<seg_mem_t>(aSegmentTuples, _segIndex);
    TRACE("Segment Tuples loaded");
  _partMngr.load(aPartitionTuples);
  _segMngr.load(aSegmentTuples);
  _masterPartition = (PartitionFile*)_partMngr.getPartition(_partMngr._masterPartName);
}

void DatabaseInstanceManager::shutdown()
{
    TRACE("storing Database");
  //  if (isRunning()) {
        // stop transactions
        _segMngr.storeSegments();
        BufferManager::getInstance().flushAll();
 //       _running = false;
  //  }
}
