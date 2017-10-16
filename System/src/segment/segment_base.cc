#include "segment_base.hh"
#include <iostream>

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
    _pages(),
    _partition(aPartition),
    _index(0)
{
	bool flag = _partition.isOpen();
	if(!flag){ _partition.open();}
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	if(!flag) {_partition.close();}	
}

SegmentBase::~SegmentBase(){}