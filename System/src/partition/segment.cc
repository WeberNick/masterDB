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

    Segment::Segment(Partition& aPartition) :
    _segID(0),
    _pages(),
    _partition(aPartition),
    _header()
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
    //assuming header stores up to date information

//segment 1 on position 1
//header last

    byte* lPageBuffer = new byte[_partition.getPageSize()];
    size_t lPageSize = _partition.getPageSize();
    for (uint i=0;i<_pages.size();++i){
        *((uint32_t*) (lPageBuffer + i))=_pages.at(i);
    }
    *(segment_page_header_t*) (lPageBuffer + lPageSize - sizeof(segment_page_header_t) )=_header;
    return 1;
}

const int Segment::loadSegment(uint32_t aPageIndex)
{
//to be set beforehand: partition

    byte* lPageBuffer = new byte[_partition.getPageSize()];
    size_t lPageSize = _partition.getPageSize();
    lPageBuffer = _partition.loadPage(aPageIndex);
    _header = *(segment_page_header_t*) (lPageBuffer + lPageSize - sizeof(segment_page_header_t);
    for (uint i=0;i<_header._currSize;++i){
        _pages.push_back( *((uint32_t*) (lPageBuffer + i)) );
    }

//some more variables to be set

    return 1;

}