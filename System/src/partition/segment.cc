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
    _maxSize(0),
    _index(0),
    _header()
{
	_maxSize = (_partition.getPageSize() - sizeof(segment_page_header_t)) / sizeof(uint32_t);
	int lPageIndex = _partition.allocPage();
	_index = (lPageIndex > 0) ? (uint32_t)lPageIndex : 0;
	uint lLSN = 0;		//todo
	uint lVersion = 0;	//todo
	uint lUnused = 0;
	basic_header_t lBasicHeader = {lLSN, _index, _partition.getPartitionID(), lVersion, lUnused, lUnused};
	_header = {lBasicHeader, _maxSize, 0, lVersion, lUnused, lUnused, lUnused};
}

Segment::~Segment()
{}

const int Segment::getNewPage()
{
	if (_pages.size() < _maxSize) {
		page = _partition.allocPage();
		if (page != -1) {
			_pages.pushback(page);
			return _pages[_pages.size() - 1];
		}
	}
	return -1;
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
    /* partition open
       read von der _partition
       dem read musst du den file descriptor von der open mitgeben, buffer, pageno, buffersize (von der partition die pagesize) */
}

const int Segment::storePage(const byte* aPageBuffer, const uint aPageNo)
{
	int lFileDescriptor = _partition.openPartition("write");
	if(lFileDescriptor == -1)
	{
		return -1;
	}
	if(writePage(lFileDescriptor, aPageBuffer, aPageNo, _partition.getPageSize()) == -1)
	{
		return -1;
	}
	return 0;
}

const int Segment::storeSegment()
{

}

const int Segment::loadSegment()
{

}