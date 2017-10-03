/**
 *  @file    segment_manager.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple segments
 *  @bugs    Currently no bugs known
 *  @todos   TBD
 *  @section TBD
 */

#ifndef SEGMENT_MANAGER_HH
#define SEGMENT_MANAGER_HH

#include "infra/types.hh"
#include "segment.hh"
#include <vector>

class SegmentManager
{
	public:
		explicit SegmentManager(Partition& aPartition);
		SegmentManager(const SegmentManager& aSegmentManager) = delete;
		SegmentManager& operator=(const SegmentManager& aSegmentManager) = delete;
		~SegmentManager();	                    // delete all segments

	public:
		Segment* getNewSegment();				// create and add new segment (persistent), return it
		const int storeSegmentManager();		// serialization
		const int loadSegmentManager();			// deserialization

	public:
		inline const uint getNoSegments() { return _segments.size(); }				
		inline Segment* getSegment(const uint aIndex) { return _segments.at(aIndex); }

	private:
		const int storeSegments();				// serialize segments? called by storeSegmentManager
		const int loadSegments();				// deserialize segments? called by storeSegmentManager

	private:
		/* Stores all managed Segments */
		std::vector<Segment*> _segments;
		/* */
		Partition& _partition;
		/* list of pages where segmentmanager writes */
		// ..
};

#endif