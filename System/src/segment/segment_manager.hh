/**
 *  @file    segment_manager.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple segments
 *  @bugs    Currently no bugs known
 *  @todos   -
 *  @section TBD
 */

#ifndef SEGMENT_MANAGER_HH
#define SEGMENT_MANAGER_HH

#include "infra/types.hh"
#include "infra/header_structs.hh"
#include "partition/partition_base.hh"
#include "partition/partition_file.hh"
#include "segment_base.hh"
#include "segment.hh"
#include "segment_fsm.hh"
#include "segment_fsm_sp.hh"
#include "partition/partition_manager.hh"

#include <map>

class SegmentManager
{
	private:
		explicit SegmentManager();
		SegmentManager(const SegmentManager& aSegmentManager) = delete;
		SegmentManager& operator=(const SegmentManager& aSegmentManager) = delete;
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

	public:
		void load(seg_vt& aTuples);
		void store(PartitionFile& aMasterPartition, const uint aSegmentIndex);

	public:
		SegmentFSM* createNewSegmentFSM(PartitionBase& aPartition, std::string aName); // create and add new segment (persistent), return it
		SegmentFSM_SP* createNewSegmentFSM_SP(PartitionBase& aPartition, std::string aName); // create and add new segment (persistent), return it
		// for further segment types... SegmentA* createNewSegmentA();
                SegmentFSM_SP* loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex);
                void deleteSegment(SegmentBase* aSegment);
		void deleteSegment(const uint16_t aID);
		void deleteSegment(const std::string aName);
		int storeSegmentManager(PartitionBase& aPartition);	 // serialization
		int loadSegmentManager(PartitionBase& aPartition);    // deserialization

	public:
		inline const uint getNoSegments() { return _segments.size(); }	
		inline const seg_vt& getSegmentTuples(){ return _segmentTuples; }			
		SegmentBase* getSegment(const uint16_t aSegmentID);

	private:
		void storeSegments();
		void loadSegments(uint32_vt& aSegmentPages, PartitionBase& aPartition);

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
		

		/* Indices of Pages in the Partition where the SegmentManager itself is spread; Default is Page 1 
		TO BE DELETED*/
		uint32_vt _indexPages;		
		/* Number of Pages that can be managed on one SegmentManager Page */
		uint32_t _maxSegmentsPerPage;
};

#endif
