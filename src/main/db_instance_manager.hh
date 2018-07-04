/**
 *  @file   db_instance_manager.hh
 *  @author Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Singleton class managing everything needed for the DB instance to boot
 *  @bugs   Currently no bugs known
 *  @todos  Changes correct? Fix reworks
 *
 *  @section TBD
 */
#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/partition_t.hh"
#include "../infra/segment_t.hh"
#include "../buffer/buf_mngr.hh"
#include "../partition/partition_manager.hh"
#include "../segment/segment_manager.hh"
#include "../segment/segment_fsm_sp.hh"
#include "../interpreter/interpreter_sp.hh"

#include <string>

class DatabaseInstanceManager final
{
	private:
		DatabaseInstanceManager();
        explicit DatabaseInstanceManager(const DatabaseInstanceManager&) = delete;
        explicit DatabaseInstanceManager(DatabaseInstanceManager&&) = delete;
		DatabaseInstanceManager &operator=(const DatabaseInstanceManager&) = delete;
		DatabaseInstanceManager &operator=(DatabaseInstanceManager&&) = delete;
		~DatabaseInstanceManager();

	public:
        /**
         * @brief This function is the only way to get access to the PartitionManager instance
         * 
         * @return DatabaseInstanceManager& reference to the only PartitionManager instance
         */
		static DatabaseInstanceManager& getInstance() noexcept
        {
			static DatabaseInstanceManager lDBIM_Instance;
			return lDBIM_Instance;
		}

        /** TODO
         * @brief 
         * 
         * @param aControlBlock 
         */
        void init(const CB& aControlBlock);
        /** TODO
         * @brief 
         * 
         */
		void shutdown();

	private:
        /** TODO
         * @brief 
         * 
         */
		void install();
        /** TODO
         * @brief 
         * 
         */
		void boot();

	public:
		inline PartitionManager& getPartMngr() noexcept { return _partMngr; }
		inline SegmentManager& getSegMngr() noexcept { return _segMngr; }
        inline bool isRunning() noexcept { return _running; }

	private:
		template<typename T_TupleType>
		void load(std::vector<T_TupleType>& aTuples, const uint aIndex);
		template<typename T_TupleType> // probably of no use, physical tuples always up to date. BufMngr.flushall should be enough
		void store(std::vector<T_TupleType>& aTuples, const uint aIndex);
	
	private:
        std::string       _path;
        PartitionFile*    _masterPartition;
        PartitionManager& _partMngr;
        SegmentManager&   _segMngr;
        uint              _partIndex; // Index of first segment storing pages with partition tuples, should be 1
        uint              _segIndex;  // Index of first segment storing pages with segment tuples, should be 3
        const CB*         _cb;
        bool              _running;
};

template <typename T_TupleType>
void DatabaseInstanceManager::load(std::vector<T_TupleType>& aTuples, const uint aIndex)
{
    TRACE("Loading tuples from the master partition starts...");
    /* part_t lMasterPartitionTuple = { 0, _partMngr._masterPartName, _cb->mstrPart(), 1, 20 };
       PartitionManager& lPartMan = PartitionManager::getInstance();
       lPartMan._partitionsByID[0]=lMasterPartitionTuple;
       PartitionFile* lMasterPart = _partMngr.createMasterPartition(lMasterPartitionTuple);
       SegmentFSM_SP* lSegments = _segMngr.loadSegmentFSM_SP(*lMasterPart, aIndex); */

    // Partition_T(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth);
    const uint8_t lPartitionID = 1;
    const std::string& lName = _partMngr.masterPartName();
    const std::string& lPathToMaster = _cb->mstrPart();
    const uint8_t lPartitionType = 1; // file
    const uint16_t lPartitionGrowth = 20;
    Partition_T lMasterPartitionTuple(lPartitionID, lName, lPathToMaster, lPartitionType, lPartitionGrowth);

    PartitionFile* lMasterPart = _partMngr.createMasterPartition(lMasterPartitionTuple);
    SegmentFSM_SP* lSegments = _segMngr.loadSegmentFSM_SP(*lMasterPart, aIndex);
    byte* lPage = new byte[lMasterPart->getPageSize()];
    TRACE(" getPageSize " + std::to_string(lMasterPart->getPageSize()) + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
    InterpreterSP lInterpreter;

    for (size_t i = 0; i < lSegments->getNoPages(); ++i)
    {
        lSegments->readPageUnbuffered(i, lPage, lMasterPart->getPageSize());
        lInterpreter.attach(lPage);
        TRACE("step" + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
        for (uint j = 0; j < lInterpreter.noRecords(); ++j)
        {
            TRACE("step " + std::to_string(j) + std::to_string(lInterpreter.noRecords()) + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
            T_TupleType temp;
            if (lInterpreter.getRecord(j))
            {
                temp.toMemory(lInterpreter.getRecord(j));
                TRACE(" " + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
                aTuples.push_back(temp);
            }
            TRACE(" " + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
        }
    }
    TRACE(" " + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
    _segMngr.deleteSegment(lSegments);
    delete lMasterPart;
    delete[] lPage;
    TRACE("Loading tuples from the master partition completed");
}

/* template<typename T_TupleType>
void DatabaseInstanceManager::store(std::vector<T_TupleType>& aTuples, const uint aIndex)
{
    SegmentFSM_SP* lMasterSeg = _segMngr.loadSegmentFSM_SP(*_masterPartition, aIndex);
    int lFreeBytesPerPage = lMasterSeg->getMaxFreeBytes();
    // get size of master segment
    int lCapazIst = lMasterSeg->getNoPages() * (lFreeBytesPerPage / sizeof(T_TupleType)) ;
    // get number of segments
    // estimate if size is big enough, if not add new pages
    int lCapazSoll = aTuples.size() / sizeof(T_TupleType);
    if(lCapazIst-lCapazSoll < 0){
        // how many new pages?
        int a =  (int) ceil((lCapazSoll-lCapazIst) / (lFreeBytesPerPage / sizeof(T_TupleType)));
        while(a > 0){
            // alloc new pages
            lMasterSeg->getNewPage();
            ++a;
        }

    }
    // write all tuples that were not deleted (condition to be hold in delete segment method)
    // get new tuples (tuples always up to date)
    // delete all content of pages
    // write everything on segment
    byte* lPage = new byte[_masterPartition->getPageSize()];
    InterpreterSP lInterpreter;
    uint lSegCounter = 0;
    byte* lPos;
    for (uint i = 0; i < lMasterSeg->getNoPages(); ++i)
    {
        // lMasterSeg->readPage(lPage, 0); fix this? dont know what it should do
        lInterpreter.attach(lPage);
        lInterpreter.initNewPage(lPage);
        while(true)
        {
            lPos = lInterpreter.addNewRecord(sizeof(T_TupleType));
            if (lPos == 0)
            {
                break;
            }
            *((T_TupleType*) lPos ) =  aTuples.at(lSegCounter);
            ++lSegCounter;
        }
    }
    delete[] lPage;
    _segMngr.deleteSegment(lMasterSeg);
} */
