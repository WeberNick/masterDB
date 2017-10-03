/**
 *  @file    segment_manager.cc
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple pages
 *  @bugs    Currently no bugs known
 *  @todos   Implement:
 *              Segment::
 *  @section TBD
 */

#include "segment.hh"

Segment::Segment(const uint aSegID, FilePartition& aPartition) :
    _segID(aSegID),
    _pages(),
    _partition(aPartition),
    _header(aHeader)
{}

Segment::~Segment()
{}

const int Segment::getNewPage()
{
    
}

const int Segment::getPage(const uint aIndex)
{
	if(!(aIndex < _pages.size())
	{
		return -1;
	}
	return _pages[aIndex];
}

const int Segment::loadPage(byte* aPageBuffer, const uint aPageNo)
{
	
}

const int Segment::storePage(const byte* aPageBuffer, const uint aPageNo)
{

}

const int Segment::storeSegment()
{

}

const int Segment::loadSegment()
{

}