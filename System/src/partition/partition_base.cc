#include "partition_base.hh"

PartitionBase::PartitionBase(const char* aPath, const uint aNoPages, const uint aPageSize, const uint aPartitionID) : 
	_partitionPath(aPath),
	_sizeInPages(aNoPages),
	_pageSize(aPageSize),
	_partitionID(aPartitionID),
	_segmentIndexPage(0),
	_isCreated(false),
	_isOpen(false)
{
	_segmentIndexPage = c_SegmentIndexPagePos();
}

PartitionBase::~PartitionBase(){}