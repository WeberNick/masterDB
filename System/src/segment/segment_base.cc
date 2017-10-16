#include "segment_base.hh"

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
    _pages(),
    _partition(aPartition),
    _index(0)
{
	_partition.open();
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	_partition.close();
}

SegmentBase::~SegmentBase(){}