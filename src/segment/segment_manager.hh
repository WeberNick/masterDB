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
		~SegmentManager(); // delete all segments

	public:
        /**
         * @brief   used for booting, loads the segment manager
         *
         * @param   aTuples     all segment tuple found in the corresponding master segment.
         */
		void load(const seg_vt& aTuples) noexcept;

        /**
         * @brief   creates a new Segment FSM both on disk and in all data structures
         *
         * @param   aPartition  Partition the segment shall be on
         * @param   aName       name of the new segment. Needs to be globally unique.
         * @return  pointer to this segment.
         */
		SegmentFSM* createNewSegmentFSM(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
		/**
         * @brief   creates a new Segment FSM both on disk and in all data structures
         *
         * @param   aPartition  Partition the segment shall be on
         * @param   aName       name of the new segment. Needs to be globally unique.
         * @return  pointer to this segment.
         */
        SegmentFSM_SP* createNewSegmentFSM_SP(PartitionBase& aPartition, const std::string& aName); // create and add new segment (persistent), return it
        /**
         * @brief   used for startup. Does not insert the segment into any data structures of the manager
         *
         * @param   aPartition  Partition object the segment is on
         * @param   aIndex      page the segment shall be loaded from
         * @return  returns pointer to the segment
         * @see     segment_fsm.hh, db_instance_manager.hh
         */
        SegmentFSM_SP* loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex);
        /**
         * @brief   deletes a Segment by its ID. Deletes the object, cleans it from all data structures and calls erase.
         *
         * @param   aID     ID of the segment to be erased
         */
		void deleteSegment(const uint16_t aID);
		/**
         * @brief   deletes a Segment by its name. Deletes the object, cleans it from all data structures and calls erase.
         *          The method looks up the segments ID and calls deleteSegment(aID)
         * @param   aName     Name of the segment to be erased
         */
        void deleteSegment(const std::string& aName);
        /**
         * @brief   method to delete a tuple physically from disk. 
         *          Scans a segment for the ID stored in the tuple type and deletes it.
         * @param   aMasterName     Name of Segment to delete Tuple from. Is used for deleting either segments or partitions, but works for all.
         * @param   aID             ID of Tuple to be deleted. ID must be part of Tuple_T
         */
        template<typename Tuple_T>
		void deleteTuplePhysically (const std::string& aMasterName, uint16_t aID);
        /**
         * @brief   deletes all segments of a given partition
         *
         * @param   aPartitionID    partition the segments are on.
         */
        void deleteSegments(const uint8_t aPartitionID);

        // Getter
        /**
         * @brief   not just a getter, also loads the segment from disk if it was not loaded before
         *
         * @param   aSegmentID - ID of the segment to be retrieved
         * @return  pointer to the segment object
         */
		SegmentBase* getSegment(const uint16_t aSegmentID);
        /**
         * @brief   not just a getter, also loads the segment from disk if it was not loaded before.
         *          looks up ID to corresponding name and than call getSegment (aSegmentID)
         *
         * @param   aName - Name of the segment to be retrieved
         * @return  pointer to the segment object
         */
		SegmentBase*              getSegment(const std::string& aSegmentName);
        const Segment_T&          getSegmentT(const uint16_t aID) const;
        Segment_T&                getSegmentT(const uint16_t aID);
        const Segment_T&          getSegmentT(const std::string& aName) const;
        Segment_T&                getSegmentT(const std::string& aName);
        string_vt                 getSegmentNames() noexcept;
        string_vt                 getSegmentNamesForPartition(uint8_t aPID) noexcept;
		inline uint               getNoSegments() const noexcept { return _segments.size(); }	
		inline uint               getNoSegments() noexcept { return _segments.size(); }	
        inline const std::string& getMasterSegSegName() { return _masterSegSegName; }

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

        /**
         * @brief Initialize the control block
         * 
         * @param aControlBlock the control block
         */
        void init(const CB& aControlBlock) noexcept;

	private:
        /**
         * @brief stores all Segments currently loaded, used for shutdown.
         * 
         */
		void storeSegments();
        /**
         * @brief subpart which is the same for all different segment types. 
         *        Mainly inserts the tuple on disk and into data structures
         * 
         * @param aSegT the segment to be inserted
         */
		void createSegmentSub(const Segment_T& aSegT);
        /** 
         * @brief delete segment, wrapper needed as segment base destructor is private
         * 
         * @param aSegment the segment to delete
         */
        void deleteSegment(SegmentFSM_SP*& aSegment);
        /**
         * @brief Part of the installation, create master segments for aPartition with aName for master segment for partitions
         * 
         * @param aPartition the partition
         * @param aName the name for the master segment of partitions
         */
		void createMasterSegments(PartitionFile* aPartition, const std::string& aName);

	private:
		uint16_t                                   _counterSegmentID; // ID Counter for Segments
		std::unordered_map<uint16_t, SegmentBase*> _segments;         // Stores all managed segment objects by ID

		std::unordered_map<uint16_t, Segment_T>    _segmentsByID;     // Stores all segment Tuples by ID in map
		std::unordered_map<std::string, uint16_t>  _segmentsByName;   // Stores Name/ID pair used for lookup in next table

		std::string    _masterSegSegName; // Name of Master segment containing all segments

        BufferManager& _BufMngr;
        const CB*      _cb;
};

template<typename Tuple_T>
void SegmentManager::deleteTuplePhysically(const std::string& aMasterName, uint16_t aID)
{
    // type=0 if segment, type=1 if partition
    // open master Segment by name and load it
    SegmentFSM_SP* lSegments = (SegmentFSM_SP*)getSegment(aMasterName);
    byte* lPage;
    InterpreterSP lInterpreter;

    TRACE("Trying to delete: " + std::to_string(aID) + " from Segment " + std::to_string(lSegments->getID()) + " " + _segmentsByID.at(lSegments->getID()).name() + ", originally searched for " + aMasterName);
    // search all pages for tuple
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
            if(lTuplePointer)
            {
                lTuple.toMemory(lInterpreter.getRecord(j));

                if(lTuple.ID() == aID)
                {
                    // mark deleted
                    lSegments->getPage(i, LOCK_MODE::kEXCLUSIVE);
                    lInterpreter.deleteRecordSoft(j);
                    lSegments->releasePage(i, true);
                    TRACE("Tuple deleted successfully.");
                    return;
                }
            }
        ++j;
        lSegments->releasePage(i);
        }
    }
    const std::string lErrMsg("Deletion of tuple went wrong - tuple not found.");
    TRACE(lErrMsg);
    throw BaseException(FLF, lErrMsg);
}
