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

Segment::Segment(const uint aSegID, FilePartition& aPartition) :
    _segID(aSegID),
    _pages(),
    _partition(aPartition),
    _maxSize(0),
    _index(0),
    _header()
{
	_maxSize = (_partition.getPageSize() - sizeof(segment_page_header_t)) / sizeof(uint32_t);
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	uint64_t lLSN = 0;		//todo
	uint8_t lVersion = 0;	//todo
	uint8_t lUnused = 0;
	basic_header_t lBasicHeader = {lLSN, _index, _partition.getPartitionID(), lVersion, lUnused, lUnused};
	_header = {lBasicHeader, _maxSize, 0, lVersion, lUnused, lUnused, lUnused};
}

Segment::~Segment()
{}

const int Segment::getNewPage()
{
	if (_pages.size() < _maxSize) {
		int lPageIndex = _partition.allocPage();
		if (lPageIndex != -1) {
			_pages.push_back((uint32_t)lPageIndex);
			return _pages[_pages.size() - 1];
		}
	}
	return -1;
}

const int Segment::getPage(const uint aIndex)
{
	if(!(aIndex < _pages.size()))
	{
		return -1;
	}
	return _pages[aIndex];
}

const int Segment::loadPage(byte* aPageBuffer, const uint aPageNo)
{
	int lFileDescriptor = _partition.openPartition("r");
	if(lFileDescriptor == -1)
	{
		return -1;
	}
	if(_partition.readPage(lFileDescriptor, aPageBuffer, aPageNo, _partition.getPageSize()) == -1)
	{
		return -1;
	}
	return 0;
}

const int Segment::storePage(const byte* aPageBuffer, const uint aPageNo)
{
	int lFileDescriptor = _partition.openPartition("w");
	if(lFileDescriptor == -1)
	{
		return -1;
	}
	if(_partition.writePage(lFileDescriptor, aPageBuffer, aPageNo, _partition.getPageSize()) == -1)
	{
		return -1;
	}
	return 0;
}

const int Segment::storeSegment()
{
	return -1;
}

const int Segment::loadSegment()
{
	return -1;
}