#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aNoPages, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) :
	PartitionBase(aPath, aName, aNoPages, aPageSize, aSegmentIndexPage, aPartitionID)
{

}

PartitionRaw::~PartitionRaw(){}

int PartitionRaw::open()
{
	return -1;
}

int PartitionRaw::close()
{
	return -1;
}

int PartitionRaw::createPartition()
{
	return -1;
}

int PartitionRaw::removePartition()
{
	return -1;
}

int PartitionRaw::allocPage()
{
	return -1;
}

int PartitionRaw::freePage(const uint aPageIndex)
{
	return -1;
}

int PartitionRaw::readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	return -1;
}

int PartitionRaw::writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	return -1;
}
