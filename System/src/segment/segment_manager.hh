/**
 *  @file    segment_manager.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple segments
 *  @bugs    Currently no bugs known
 *  @todos   - Pass Header information to getNewSegment for creation of Segment()
 *           - Implement:
 *               SegmentManager::storeSegmentManager()
 *               SegmentManager::loadSegmentManager()
 *               SegmentManager::storeSegments()
 *               SegmentManager::loadSegments()
 *  @section TBD
 */

#ifndef SEGMENT_MANAGER_HH
#define SEGMENT_MANAGER_HH

#include "infra/types.hh"
#include "infra/header_structs.hh"
#include "partition/partition_base.hh"
#include "segment_base.hh"
#include "segment.hh"

#include <map>

class SegmentManager
{
	public:
		explicit SegmentManager(PartitionBase& aPartition);
		SegmentManager(const SegmentManager& aSegmentManager) = delete;
		SegmentManager& operator=(const SegmentManager& aSegmentManager) = delete;
		~SegmentManager();	         // delete all segments

	public:
		Segment* createNewSegment(); // create and add new segment (persistent), return it
		// for further segment types... SegmentA* createNewSegmentA();			
		int storeSegmentManager();	 // serialization
		int loadSegmentManager();    // deserialization

	public:
		inline const uint getNoSegments() { return _segments.size(); }				
		SegmentBase* getSegment(const uint aSegmentID);

	private:
		int storeSegments(); // serialize segments? called by storeSegmentManager
		int loadSegments();	 // deserialize segments? called by storeSegmentManager

	private:
		/* ID Counter for Segments */
		uint _counterSegmentID;
		/* Stores all managed Segments */
		std::map<uint, SegmentBase*> _segments;
		/* Indices of Pages in the Partition where the SegmentManager itself is spread; Default is Page 1 */
		std::vector<uint32_t> _ownPages;		
		/* Number of Pages that can be managed on one SegmentManager Page */
		uint32_t _maxSegmentsPerPage;
		/* Partition the SegmentManager belongs to */
		PartitionBase& _partition;
};

#endif