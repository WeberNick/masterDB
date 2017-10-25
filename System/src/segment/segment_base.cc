#include "segment_base.hh"
#include <iostream>

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
	_indexPages(),
    _pages(),
    _partition(aPartition)
{
	if(_partition.open() == -1){/*error handling*/}
	int lSegmentIndex = _partition.allocPage();
	_indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
	if(_partition.close() == -1){/*error handling*/}
}

SegmentBase::SegmentBase(PartitionBase& aPartition) : 
	_segID(0),
	_indexPages(),
    _pages(),
    _partition(aPartition)
{}

SegmentBase::~SegmentBase(){}