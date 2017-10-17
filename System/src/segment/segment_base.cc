#include "segment_base.hh"
#include <iostream>

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition) : 
	_segID(aSegID),
    _pages(),
    _partition(aPartition),
    _index(0)
{
<<<<<<< HEAD
	if(_partition.open() == -1){/*error handling*/}
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	if(_partition.close() == -1){/*error handling*/}
=======
	bool flag = _partition.isOpen();
	if(!flag){ _partition.open();}
	int lSegmentIndex = _partition.allocPage();
	_index = (lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0;
	if(!flag) {_partition.close();}	
>>>>>>> d1fe2ac2adb3e92c4e73af24d136f8850fa2342e
}

SegmentBase::SegmentBase(PartitionBase& aPartition) : 
	_segID(0),
    _pages(),
    _partition(aPartition),
    _index(0)
{}

SegmentBase::~SegmentBase(){}