/**
 *  @file    segment.cc
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), 
 			 Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   This class manages multiple pages
 *  @bugs    Currently no bugs known
 *  @todos   Implement storeSegment and loadSegment
 *  @section TBD
 */

#include "segment.hh"

Segment::Segment(const uint aSegID, PartitionBase& aPartition) :
    SegmentBase(aSegID, aPartition),
    _maxSize(0),
    _header()
{
	_partition.openPartition();
	_maxSize = (_partition.getPageSize() - sizeof(segment_page_header_t)) / sizeof(uint32_t);
	uint64_t lLSN = 0;		//todo
	uint8_t lVersion = 0;	//todo
	uint8_t lUnused = 0;
	basic_header_t lBasicHeader = {lLSN, _index, _partition.getID(), lVersion, lUnused, lUnused};
	_header = {_maxSize, 0, lVersion, lUnused, lUnused, lUnused, lBasicHeader};
	_partition.closePartition();
}

Segment::~Segment(){}

int Segment::getNewPage()
{
	if (_pages.size() < _maxSize) {
		int lPageIndex = _partition.allocPage();
		//todo init page
		if (lPageIndex != -1) {
			_pages.push_back((uint32_t)lPageIndex);
			return _pages.size() - 1;
		}
	}
	return -1;
}

int Segment::loadPage(byte* aPageBuffer, const uint aPageNo)
{
	if(_partition.openPartition() == -1)
	{
		return -1;
	}
	if(_partition.readPage(aPageBuffer, _pages[aPageNo], _partition.getPageSize()) == -1)
	{
		return -1;
	}
	return 0;
}

int Segment::storePage(const byte* aPageBuffer, const uint aPageNo)
{
	if(_partition.openPartition() == -1)
	{
		return -1;
	}
	if(_partition.writePage(aPageBuffer, _pages[aPageNo], _partition.getPageSize()) == -1)
	{
		return -1;
	}
	return 0;
}

int Segment::storeSegment()
{
	 //assuming header stores up to date information
	 //segment 1 on position 1
	 //header last
	 //partition needs to be opened beforehand

    byte* lPageBuffer = new byte[_partition.getPageSize()];
     size_t lPageSize = _partition.getPageSize();
     for (uint i=0;i<_pages.size();++i){
         *(((uint32_t*) lPageBuffer) + i)=_pages.at(i);
     }
	 *(segment_page_header_t*) (lPageBuffer + lPageSize - sizeof(segment_page_header_t) ) = _header;
	  _partition.writePage(lPageBuffer,_header._basicHeader._pageIndex,_partition.getPageSize());
	  delete[] lPageBuffer;  
	  return 0;
}

int Segment::loadSegment(const uint32_t aPageIndex, uint aSegID)
{
	 //to be set beforehand: partition, and it has to be opened
	 
     byte* lPageBuffer = new byte[_partition.getPageSize()];
     size_t lPageSize = _partition.getPageSize();
     _partition.readPage(lPageBuffer,aPageIndex,_partition.getPageSize());
     _header = *(segment_page_header_t*) (lPageBuffer + lPageSize - sizeof(segment_page_header_t));
     for (uint i=0;i<_header._currSize;++i){
         _pages.push_back( *(((uint32_t*) lPageBuffer) + i) );
	 }
	 //some more variables to be set
	 _segID = aSegID;
	 _maxSize =_header._maxSize;
	 _index=aPageIndex;
    return 0;
}