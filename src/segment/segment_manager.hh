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
#include "../infra/segment_t.hh"
#include "../infra/header_structs.hh"
#include "../partition/partition_manager.hh"
#include "../partition/partition_base.hh"
#include "../partition/partition_file.hh"
#include "../buffer/buf_mngr.hh"
#include "../buffer/buf_cntrl_block.hh"
#include "segment_base.hh"
#include "segment_fsm.hh"
#include "segment_fsm_sp.hh"

#include <unordered_map>
#include <algorithm>

class SegmentManager
{
	private:
        friend class DatabaseInstanceManager;
		SegmentManager();
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
        static SegmentManager& getInstance() noexcept
        {
            static SegmentManager lSegmentManagerInstance;
            return lSegmentManagerInstance;
        }

        void init(const CB& aControlBlock) noexcept;

	public:
		void load(const seg_vt& aTuples) noexcept;

	public:
		SegmentFSM*    createNewSegmentFSM(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
		SegmentFSM_SP* createNewSegmentFSM_SP(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
        SegmentFSM_SP* loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex);
		void deleteSegment(const uint16_t aID);
		void deleteSegment(const std::string& aName);
        template<typename Tuple_T>
		void deleteTuplePhysically (const std::string& aMasterName, uint16_t aID);
        void deleteSegements(const uint8_t aPartitionID);

    public:
		SegmentBase*     getSegment(const uint16_t aSegmentID);
		SegmentBase*     getSegment(const std::string& aSegmentName);
        const Segment_T& getSegmentT(const uint16_t aID) const;
        Segment_T&       getSegmentT(const uint16_t aID);
        const Segment_T& getSegmentT(const std::string& aName) const;
        Segment_T&       getSegmentT(const std::string& aName);
        string_vt        getSegmentNames() noexcept;
        string_vt        getSegmentNamesForPartition(uint8_t aPID) noexcept;
		inline uint      getNoSegments() const noexcept { return _segments.size(); }	
		inline uint      getNoSegments() noexcept { return _segments.size(); }	

	private:
		void storeSegments();
		void createSegmentSub (const Segment_T& aSegT);
        //wrapper needed as segment base destructor is private
        void deleteSegment(SegmentFSM_SP*& aSegment);
		void createMasterSegments(PartitionFile* aPartition, const std::string& aName);

	private:
		uint16_t                         _counterSegmentID; // ID Counter for Segments
		std::unordered_map<uint16_t, SegmentBase*> _segments;         // Stores all managed segment objects by ID

		std::unordered_map<uint16_t, Segment_T>    _segmentsByID;     // Stores all segment Tuples by ID in map
		std::unordered_map<std::string, uint16_t>  _segmentsByName;   // Stores Name/ID pair used for lookup in next table

		/* Indices of Pages in the Partition where the SegmentManager itself is spread; Default is Page 1
           TODO TO BE DELETED */
		uint32_vt      _indexPages;		
		uint32_t       _maxSegmentsPerPage; // Number of Pages that can be managed on one SegmentManager Page
        std::string    _masterSegSegName;   // Name of Master segment containing all segments

        BufferManager& _BufMngr;
        const CB*      _cb;
        // bool        _installed = false; // only true, if installed.
};

template<typename Tuple_T>
void SegmentManager::deleteTuplePhysically(const std::string& aMasterName, uint16_t aID)
{
    // type=0 if segment, type=1 if partition

    // open master Segment by name and load it
    SegmentFSM_SP* lSegments = (SegmentFSM_SP*)getSegment(aMasterName);
    byte* lPage;
    InterpreterSP lInterpreter;

    TRACE("trying to delete: "+std::to_string(aID));
    TRACE("from Segment "+std::to_string(lSegments->getID())+" "+_segmentsByID.at(lSegments->getID()).name());
    TRACE("originally searched for "+aMasterName);

    //search all pages for tuple
    uint j;
    byte* lTuplePointer;
    for (size_t i = 0; i < lSegments->getNoPages(); ++i)
    {
      lPage = lSegments->getPage(i, LOCK_MODE::kSHARED);

   	  lInterpreter.attach(lPage);
      j = 0;
   	  while( j < lInterpreter.noRecords())
   	  {
          Tuple_T lTuple;
          lTuplePointer = lInterpreter.getRecord(j);
          if(lTuplePointer){
            lTuple.toMemory(lInterpreter.getRecord(j));
            TRACE(std::to_string(j)+" "+lTuple.to_string());

            if(lTuple.ID() == aID){
                //mark deleted
                lSegments->getPage(i, LOCK_MODE::kEXCLUSIVE);
                lInterpreter.deleteRecordSoft(j);
                lSegments->releasePage(i, true);
                TRACE("Tuple deleted successfully.");
                return;
            }
          }
          else{
              TRACE(std::to_string(j)+ " deleted tuple");
          }
        ++j;
        lSegments->releasePage(i);
    }
    }
    const std::string lErrMsg("Deletion of tuple went wrong - tuple not found.");
    TRACE(lErrMsg);
    throw BaseException(FLF, lErrMsg);
}
