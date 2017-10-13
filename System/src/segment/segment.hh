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
#include "segment_base.hh"
#include "partition/partition_file.hh"
#include <vector>

class Segment : public SegmentBase
{
	private:
		friend class SegmentManager;
		explicit Segment(const uint aSegID, PartitionBase& aPartition);
		Segment(const Segment& aSegment) = delete;
		Segment& operator=(const Segment& aSegment) = delete;
		~Segment();

	public:
		/* getNewPage -> getPage -> loadPage */
		int getNewPage();                                           // alloc free page, add it to managing vector and return its index in the partition
		int loadPage(byte* aPageBuffer, const uint aPageNo);        // load page from the partition into main memory
		int storePage(const byte* aPageBuffer, const uint aPageNo); // store page from main memory into the partition

	public:
		int storeSegment();                                          // serialization
		int loadSegment(const uint32_t aPageIndex, uint aSegID);     // deserialization

	private:
		/* The maximum number of pages a segment can manage */
		uint16_t _maxSize;
        /* Page Header of Segment */
		segment_page_header_t _header;
};

#endif