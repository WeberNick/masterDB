/**
 *  @file    segment_manager.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple segments
 *  @bugs    Currently no bugs known
 *  @todos   -
 *  @section TBD
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/tuples.hh"
#include "../infra/header_structs.hh"
#include "../partition/partition_manager.hh"
#include "../partition/partition_base.hh"
#include "../partition/partition_file.hh"
#include "../buffer/buf_mngr.hh"
#include "../buffer/buf_cntrl_block.hh"
#include "segment_base.hh"
#include "segment_fsm.hh"
#include "segment_fsm_sp.hh"

#include <map>

class SegmentManager
{
	private:
        friend class DatabaseInstanceManager;
		explicit SegmentManager();
		explicit SegmentManager(const SegmentManager&) = delete;
        explicit SegmentManager(SegmentManager&&) = delete;
		SegmentManager& operator=(const SegmentManager&) = delete;
        SegmentManager& operator=(SegmentManager&&) = delete;
		~SegmentManager();	         // delete all segments

	public:
        /**
         *  @brief  This function is the only way to get access to the SegmentManager instance
         *
         *  @return reference to the only SegmentManager instance
         */
        static SegmentManager& getInstance()
        {
            static SegmentManager lSegmentManagerInstance;
            return lSegmentManagerInstance;
        }

        void init(const CB& aControlBlock);

	public:
		void load(const seg_vt& aTuples);

	public:
		SegmentFSM* createNewSegmentFSM(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
		SegmentFSM_SP* createNewSegmentFSM_SP(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
		// for further segment types... SegmentA* createNewSegmentA();
         SegmentFSM_SP* loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex);

		SegmentBase* getSegment(const uint16_t aSegmentID);
		SegmentBase* getSegment(const std::string& aSegmentName);

        void deleteSegment(SegmentBase* aSegment);
		void deleteSegment(const uint16_t aID);
		void deleteSegment(const std::string& aName);
        template<typename Tuple_T>
		void deleteTupelPhysically (const std::string& aMasterName, uint16_t aID);

		void createMasterSegments(PartitionFile* aPartition, const std::string& aName);


	public:
		inline uint getNoSegments() { return _segments.size(); }	


	private:
		void storeSegments();
		void createSegmentSub (const Segment_T& aSegT);


	private:
		/* ID Counter for Segments */
		uint16_t _counterSegmentID;
		/* Stores all managed segment objects by ID */
		std::map<uint16_t, SegmentBase*> _segments;

		//changed storage model


		/* Stores all segment Tuples by ID in map */
		std::map<uint16_t, Segment_T> _segmentsByID;
		//stores Name/ID pair used for lookup in next table
		std::map<std::string, uint16_t> _segmentsByName;
		
		//bool _installed = false; //only true, if installed.
		

		/* Indices of Pages in the Partition where the SegmentManager itself is spread; Default is Page 1 
		TO BE DELETED*/
		uint32_vt _indexPages;		
		/* Number of Pages that can be managed on one SegmentManager Page */
		uint32_t _maxSegmentsPerPage;

		BufferManager& _BufMngr;

		std::string _masterSegSegName; //name of Master segment containing all segments

        const CB*   _cb;
        bool        _init;

};

template<typename Tuple_T>
void SegmentManager::deleteTupelPhysically(const std::string& aMasterName, uint16_t aID){
    //type=0 if segment, type=1 if partition

    //open master Segment by name and load it
    SegmentFSM_SP* lSegments = (SegmentFSM_SP*) getSegment(aMasterName);
    byte* lPage;
    InterpreterSP lInterpreter;

    //search all pages for tuple
    uint j;
    for (size_t i = 0; i < lSegments->getNoPages(); ++i)
    {
      lPage = lSegments->getPage(i, kSHARED);

   	  lInterpreter.attach(lPage);
      j = 0;
   	  while( j < lInterpreter.noRecords())
   	  {
          Tuple_T lTuple;
          lTuple.toMemory(lInterpreter.getRecord(j));
        if(lTuple.ID() == aID){
            //mark deleted
            lSegments->getPage(i, kEXCLUSIVE);
            lInterpreter.deleteRecordSoft(j);
            lSegments->releasePage(i, true);
            TRACE("Tuple deleted successfully.");
            return;
        }
        ++j;
   	  }
    lSegments->releasePage(i);
    }
    const std::string lErrMsg("Deletion of tuple went wrong - tuple not found.");
    TRACE(lErrMsg);
    throw BaseException(FLF, lErrMsg);
}
