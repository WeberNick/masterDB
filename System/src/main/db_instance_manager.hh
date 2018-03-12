/**
 *  @file   db_instance_manager.hh
 *  @author Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Singleton class managing everything needed for the DB instance to boot
 *  @bugs   Currently no bugs known
 *  @todos  Currently no todos
 *
 *  @section TBD
 */

#ifndef DB_INSTANCE_MANAGER_HH
#define DB_INSTANCE_MANAGER_HH

#include "infra/types.hh"
#include "partition/partition_manager.hh"
#include "segment/segment_manager.hh"

#include "segment/segment_fsm_sp.hh"
#include "interpreter/interpreter_sp.hh"

#include <string>

class DatabaseInstanceManager 
{
	private:
		explicit DatabaseInstanceManager(const std::string aPathToMasterPartition);
		DatabaseInstanceManager(const DatabaseInstanceManager& aDatabaseInstanceManager) = delete;
		DatabaseInstanceManager &operator=(const DatabaseInstanceManager &aDatabaseInstanceManager) = delete;
		~DatabaseInstanceManager();

	public:
		/**
		 *  @brief  This function is the only way to get access to the PartitionManager instance
		 *
		 *  @return reference to the only PartitionManager instance
		 */
		static DatabaseInstanceManager& getInstance(const std::string aPathToMasterPartition) {
			static DatabaseInstanceManager lDBIM_Instance(aPathToMasterPartition);
			return lDBIM_Instance;
		}

	public:
		void install();
		void boot();
		void shutdown();

	public:
		inline PartitionManager& getPartMngr() { return _partMngr; }
		inline SegmentManager& getSegMngr() { return _segMngr; }

	private:
		template<typename T_TupleType>
		void load(std::vector<T_TupleType>& aTuples, const uint aIndex);
		template<typename T_TupleType>
		void store(std::vector<T_TupleType>& aTuples, const uint aIndex);
		void loadPartitionManager(); //called in boot, loads the PartMngr from the master part
		void loadSegmentManager(); //called in boot, loads the SegMngr from the master part

	private:
        PartitionFile _masterPartition;
		PartitionManager& _partMngr;
		SegmentManager& _segMngr;
        uint _partIndex; //Index of first segment storing pages with partition tuples, should be 1
        uint _segIndex; //Index of first segment storing pages with segment tuples, should be 2
};

template<typename T_TupleType>
void DatabaseInstanceManager::load(std::vector<T_TupleType>& aTuples, const uint aIndex)
{
	SegmentFSM_SP* lSegments = _segMngr.loadSegmentFSM_SP(_masterPartition, aIndex);
    byte* lPage = new byte[_masterPartition.getPageSize()];
    InterpreterSP lInterpreter;

    for (uint i = 0; i < lSegments->getNoPages(); ++i)
    {
   	  lSegments->readPage(lPage, aIndex);
   	  lInterpreter.attach(lPage);
   	  for (uint j = 0; j < lInterpreter.noRecords(); ++j)
   	  {
        aTuples.push_back((*((T_TupleType*)lInterpreter.getRecord(j))));
   	  }
    }
    delete[] lPage;
    _segMngr.deleteSegment(lSegments);
}

template<typename T_TupleType>
void DatabaseInstanceManager::store(std::vector<T_TupleType>& aTuples, const uint aIndex)
{
    SegmentFSM_SP* lMasterSeg = _segMngr.loadSegmentFSM_SP(_masterPartition, aIndex); 
    int lFreeBytesPerPage = lMasterSeg->getMaxFreeBytes();
    //get size of master segment
    int lCapazIst = lMasterSeg->getNoPages() * ( lFreeBytesPerPage / sizeof(T_TupleType) ) ;
    //get number of segments
    // estimate if size is big enough, if not add new pages
    int lCapazSoll = aTuples.size()/sizeof(T_TupleType);
    if(lCapazIst-lCapazSoll < 0){
        //how many new pages?
        int a =  (int) ceil(( lCapazSoll-lCapazIst) / ( lFreeBytesPerPage /sizeof(T_TupleType) ));
        while(a>0){
            //alloc new pages
            lMasterSeg->getNewPage();
            ++a;
        }

    }
    //write all tuples that were not deleted (condition to be hold in delete segment method)
    //get new tuples (tuples always up to date)
    //delete all content of pages
    //write everything on segment
    byte* lPage = new byte[_masterPartition.getPageSize()];
    InterpreterSP lInterpreter;
    uint lSegCounter = 0;
    byte* lPos;
    for (uint i = 0; i < lMasterSeg->getNoPages(); ++i)
    {
   	    lMasterSeg->readPage(lPage, 0);
        lInterpreter.attach(lPage);
        lInterpreter.initNewPage(lPage);   
        while(true){
           lPos = lInterpreter.addNewRecord(sizeof(T_TupleType));
           if (lPos == 0){
               break;
           }
              *((T_TupleType*) lPos ) =  aTuples.at(lSegCounter);
            ++lSegCounter;
        }   
    }
	delete[] lPage;
        _segMngr.deleteSegment(lMasterSeg);
}

#endif
