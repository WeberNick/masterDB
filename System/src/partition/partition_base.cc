#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string aPath, const std::string aName, const uint aNoPages, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_sizeInPages(aNoPages),
	_pageSize(aPageSize),
	_partitionID(aPartitionID),
	_segmentIndexPage(aSegmentIndexPage),
	_isCreated(false),
	_openCount(0)
{}

PartitionBase::~PartitionBase(){}