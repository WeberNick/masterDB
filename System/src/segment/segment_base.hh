/**
 *  @file 	segment_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every segment
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#ifndef SEGMENT_BASE_HH
#define SEGMENT_BASE_HH

#include "infra/types.hh"
#include "partition/partition_base.hh"

class SegmentBase
{
	protected:
		friend class SegmentManager;
		explicit SegmentBase(const uint aSegID, PartitionBase& aPartition);
		SegmentBase(const SegmentBase& aSegment) = delete;
		SegmentBase& operator=(const SegmentBase& aSegment) = delete;
		virtual ~SegmentBase() = 0;

	public:
		virtual int getNewPage() = 0;                                       	// alloc free page, add it to managing vector and return its index in the partition
		virtual int loadPage(byte* aPageBuffer, const uint aPageNo) = 0;        // load page from the partition into main memory
		virtual int storePage(const byte* aPageBuffer, const uint aPageNo) = 0; // store page from main memory into the partition

	public:
		inline uint getID(){ return _segID; }
		inline uint getNoPages(){ return _pages.size(); }
		inline PartitionBase& getPartition() { return _partition; }
		inline uint getIndex(){ return _index; }

	protected:
		virtual int storeSegment() = 0;                                         	// serialization
		virtual int loadSegment(const uint32_t aPageIndex, const uint aSegID) = 0;  // deserialization

	protected:
		/* An ID representing this Segment */
		uint _segID;
		/* A vector containing indices to all pages (in the partition) of this segment */
		uint32_vt _pages;
		/* Partition the Segment belongs to */
		PartitionBase& _partition;
		/* Position in Partition */
		uint32_t _index;
};


#endif