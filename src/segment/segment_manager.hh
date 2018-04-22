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
		void load(seg_vt& aTuples);

	public:
		SegmentFSM* createNewSegmentFSM(PartitionBase& aPartition, std::string aName); // create and add new segment (persistent), return it
		SegmentFSM_SP* createNewSegmentFSM_SP(PartitionBase& aPartition, std::string aName); // create and add new segment (persistent), return it
		// for further segment types... SegmentA* createNewSegmentA();
         SegmentFSM_SP* loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex);

		SegmentBase* getSegment(const uint16_t aSegmentID);
		SegmentBase* getSegment(const std::string aSegmentName);

        void deleteSegment(SegmentBase* aSegment);
		void deleteSegment(const uint16_t aID);
		void deleteSegment(const std::string aName);
		void deleteTupelPhysically (const std::string& aMasterName, uint16_t aID, uint8_t aType);

		void createMasterSegments(PartitionFile* aPartition, const std::string& aName);


	public:
		inline uint getNoSegments() { return _segments.size(); }	
		inline const seg_vt& getSegmentTuples(){ return _segmentTuples; }	
		


	private:
		void storeSegments();
		bool deleteTypeChecker  ( byte* aRecord,uint16_t aID,uint8_t aType);
		void createSegmentSub (seg_t aSegT);


	private:
		/* ID Counter for Segments */
		uint16_t _counterSegmentID;
		/* Stores all managed segment objects by ID */
		std::map<uint16_t, SegmentBase*> _segments;
		/* Stores pointers to all segment Tuples by ID/Name */
		std::map<uint16_t, seg_t*> _segmentsByID;
		std::map<std::string, seg_t*> _segmentsByName;
		/* Stores all segment Tuples*/
		seg_vt _segmentTuples;

		bool _installed = false; //only true, if installed.
		

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
