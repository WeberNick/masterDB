/**
 *  @file    segment_manager.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
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
		explicit SegmentManager();
		SegmentManager(const SegmentManager& aSegmentManager) = delete;
		SegmentManager& operator=(const SegmentManager& aSegmentManager) = delete;
		~SegmentManager();	//delete all segments

	public:
		Segment* getNewSegment();				//create and add new segment, return it
		const int storeSegmentManager();		//serialization
		const int loadSegmentManager();			//deserialization

	public:
		const uint getNoSegments();				//_segments.size()
		Segment* getSegment(const uint aIndex);	//return segment at the specified index

	private:
		const int storeSegments();				//serialize segments? called by storeSegmentManager
		const int loadSegments();				//deserialize segments? called by storeSegmentManager


	private:
		std::vector<Segment*> _segments;			//stores all managed segments

};

#endif