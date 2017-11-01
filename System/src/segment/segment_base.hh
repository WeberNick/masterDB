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
		explicit SegmentBase(const uint16_t aSegID, PartitionBase& aPartition);
		explicit SegmentBase(PartitionBase& aPartition);
		SegmentBase(const SegmentBase& aSegment) = delete;
		SegmentBase& operator=(const SegmentBase& aSegment) = delete;
		virtual ~SegmentBase() = 0;

	public:
		virtual int getNewPage() = 0; // alloc free page, add it to managing vector and return its index in the partition
		int open();
		int close();
		int readPage(byte* aPageBuffer, const uint aPageNo);        // load page from the partition into main memory
		int writePage(const byte* aPageBuffer, const uint aPageNo); // store page from main memory into the partition

	public:
		inline uint16_t getID(){ return _segID; }
		inline uint32_vt getIndexPages(){ return _indexPages; }
		inline size_t getNoPages(){ return _pages.size(); }
		inline PartitionBase& getPartition() { return _partition; }

	protected:
		virtual int storeSegment() = 0;                          // serialization
		virtual int loadSegment(const uint32_t aPageIndex) = 0;  // deserialization

	protected:
		/* An ID representing this Segment */
		uint16_t _segID;
		/* Contains page addresses of pages belonging to the segment for serialization purposes. First element is considered as masterPageIndex */
		uint32_vt _indexPages;
		/* A vector containing indices to all pages (in the partition) of this segment */
		uint32_vt _pages;
		/* Partition the Segment belongs to */
		PartitionBase& _partition;
};


#endif