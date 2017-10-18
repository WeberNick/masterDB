#include "segment_base.hh"
#include <iostream>

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
    _pages(),
    _partition(aPartition),
    _index(0)
{
	if(_partition.open() == -1){/*error handling*/}
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	if(_partition.close() == -1){/*error handling*/}
}

SegmentBase::SegmentBase(PartitionBase& aPartition) : 
	_segID(0),
    _pages(),
    _partition(aPartition),
    _index(0)
{}

SegmentBase::~SegmentBase(){}