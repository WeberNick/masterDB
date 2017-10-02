/**
 *  @file    segment.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   This class manages multiple pages
 *  @bugs    Currently no bugs known
 *  @todos   TBD
 *  @section TBD
 */
#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "infra/types.hh"
#include <vector>

class Segment
{
	public:
		explicit Segment();
		Segment(const Segment& aSegment) = delete;
		Segment& operator=(const Segment& aSegment) = delete;
		~Segment();

	public:
		const uint getNewPage(); //alloc free page, add it to managing vector and return its index in the partition
		const int loadPage(byte*  aPageBuffer, const uint aPageNo); //load page from the partition into main memory
		const int storePage(const byte* aPageBuffer, const uint aPageNo); //store page from main memory into the partition
		const int storeSegment(); //serialization
		const int loadSegment(); //deserialization

	public:
		const uint getNoPages(); //_pages.size()



	private:
		uint _segID;
		uint_vt _pages;		//stores indeces to pages in the partition

};

#endif