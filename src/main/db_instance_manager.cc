#include "db_instance_manager.hh"

/**
 * @brief Construct a new DatabaseInstanceManager::DatabaseInstanceManager object
 * 
 */
DatabaseInstanceManager::DatabaseInstanceManager() :
    _path(),
    _masterPartition(nullptr),
    _partMngr(PartitionManager::getInstance()),
    _segMngr(SegmentManager::getInstance()),
    _partIndex(1), 
    _segIndex(3),
    _cb(nullptr),
    _running(false)
{
    // think of reserver page.. 
    TRACE("'DatabaseInstanceManager' constructed");
}

/**
 * @brief Destroy the DatabaseInstanceManager::DatabaseInstanceManager object
 * 
 */
DatabaseInstanceManager::~DatabaseInstanceManager()
{
    // shutdown();
    TRACE("'DatabaseInstanceManager' destructed");
}

void DatabaseInstanceManager::init(const CB& aControlBlock)
{
    if (!_cb)
    {
        _cb = &aControlBlock;
        TRACE("'DatabaseInstanceManager' initialized.");
    }
}

void DatabaseInstanceManager::install()
{
    TRACE("Installation of the database system starts...");
    Partition_T lMasterPartitionTuple;
    const size_t lMstrPartGrowth = 100;
    _masterPartition = _partMngr.createMasterPartition(_cb->mstrPart(), lMstrPartGrowth, lMasterPartitionTuple);
    _segMngr.createMasterSegments(_masterPartition, _partMngr._masterSegPartName);
    _partMngr.insertMasterPartitionTuple(lMasterPartitionTuple);
    _running = true;
    TRACE("Finished the installation of the database system!");
}

void DatabaseInstanceManager::boot()
{
    TRACE("Booting the database system starts...");
    part_vt lPartitionTuples;
    seg_vt lSegmentTuples;
    load<Partition_T>(lPartitionTuples, _partIndex);
    TRACE("## Boot: All partition tuples have been loaded");
    load<Segment_T>(lSegmentTuples, _segIndex);
    TRACE("## Boot: All segment tuples have been loaded");
    _partMngr.load(lPartitionTuples);
    _segMngr.load(lSegmentTuples);
    _masterPartition = (PartitionFile*)_partMngr.getPartition(_partMngr._masterPartName);
    _running = true;
    TRACE("Finished booting the database system!");
}

void DatabaseInstanceManager::shutdown()
{
    TRACE("Shutting down the database system starts...");
    if (isRunning())
    {
        // stop transactions
        _segMngr.storeSegments();     
        BufferManager::getInstance().flushAll();
        _running = false;
    }
    TRACE("Finished shutting down the database system!");
}
