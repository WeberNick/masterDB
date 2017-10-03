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
    _index(),
    _header()
{
	int lPageIndex = _partition.allocPage();
	_index = (lPageIndex > 0) ? lPageIndex : 0;
	_header = {_basicHeader, _maxSize, 0, _version, 0, 0, 0};


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