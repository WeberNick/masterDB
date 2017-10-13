#include "segment_base.hh"

SegmentBase::SegmentBase(const uint aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
    _pages(),
    _partition(aPartition),
    _index(0)
{
	_partition.openPartition();
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	_partition.closePartition();
}

SegmentBase::~SegmentBase(){}