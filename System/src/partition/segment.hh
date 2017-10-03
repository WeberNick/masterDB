/**
 *  @file	segment.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), 
			Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief	This class manages multiple pages
 *  @bugs	Currently no bugs known
 *  @todos	 TBD
 *  @section TBD
 */

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "infra/types.hh"
#include "file_partition.hh"
#include "infra/header_structs.hh"
#include <vector>

class Segment
{
	public:
		explicit Segment(const uint aSegID, FilePartition& aPartition);
		Segment(const Segment& aSegment) = delete;
		Segment& operator=(const Segment& aSegment) = delete;
		~Segment();

	public:
		/* getNewPage -> getPage -> loadPage */
		const int getNewPage();                                           // alloc free page, add it to managing vector and return its index in the partition
		const int getPage(const uint aIndex);                            // index for vector/id for map. return index where this page is in the partition
		const int loadPage(byte* aPageBuffer, const uint aPageNo);        // load page from the partition into main memory
		const int storePage(const byte* aPageBuffer, const uint aPageNo); // store page from main memory into the partition
		const int storeSegment();                                         // serialization
		const int loadSegment();                                          // deserialization

	public:
		inline const uint getNoPages(){ return _pages.size(); }
		inline const uint getSegmentID(){ return _segID; }

	private:
		/* An ID representing this Segment */
		uint _segID;
		/* A vector containing indices to all pages (in the partition) of this segment */
		uint_vt _pages;
		/* FilePartition the Segment belongs to */
		FilePartition& _partition;
		/* Position in Partition */
		uint _index;
        /* Page Header of Segment */
		segment_page_header_t _header;
};

#endif