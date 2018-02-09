#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) :
	PartitionBase(aPath, aName, aPageSize, aSegmentIndexPage, aPartitionID)
{

}

PartitionRaw::~PartitionRaw(){}


int PartitionRaw::create(const uint aSizeInPages)
{
	return format();
}

int PartitionRaw::remove()
{
	//write zeros?
	return 0;
}
