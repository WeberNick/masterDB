/**
 *  @file	segment.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), 
			Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief	This class manages multiple pages
 *  @bugs	Currently no bugs known
 *  @todos  Implement storeSegment and loadSegment
 *  @section TBD
 */

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "infra/types.hh"
#include "infra/header_structs.hh"
#include "partition/partition_file.hh"
#include <vector>

class Segment
{
	private:
		friend class SegmentManager;
		explicit Segment(const uint aSegID, PartitionFile& aPartition);
		Segment(const Segment& aSegment) = delete;
		Segment& operator=(const Segment& aSegment) = delete;
		~Segment();

	public:
		/* getNewPage -> getPage -> loadPage */
		int getNewPage();                                           // alloc free page, add it to managing vector and return its index in the partition
		int loadPage(byte* aPageBuffer, const uint aPageNo);        // load page from the partition into main memory
		int storePage(const byte* aPageBuffer, const uint aPageNo); // store page from main memory into the partition

	public:
		int storeSegment();                                         // serialization
		int loadSegment(const uint32_t aPageIndex,uint aSegID);     // deserialization

	public:
		inline uint getID(){ return _segID; }
		inline uint getNoPages(){ return _pages.size(); }
		uint getPageIndex(){return _header._basicHeader._pageIndex;}

	private:
		/* An ID representing this Segment */
		uint _segID;
		/* A vector containing indices to all pages (in the partition) of this segment */
		uint32_vt _pages;
		/* PartitionFile the Segment belongs to */
		PartitionFile& _partition;
		/* PartitionFile the Segment belongs to */
		uint16_t _maxSize;
		/* Position in Partition */
		uint32_t _index;
        /* Page Header of Segment */
		segment_page_header_t _header;
};

#endif