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

Segment::Segment(Partition& aPartition, segment_page_header_t& aHeader) :
    _segID(0),
    _pages(),
    _partition(aPartition),
    _header(aHeader)
{}

Segment::~Segment()
{}

const uint Segment::getNewPage()
{
    
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