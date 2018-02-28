#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPageSize, const uint aPartitionID) :
	PartitionBase(aPath, aName, aPageSize, aPartitionID)
{
	init();
}

PartitionRaw::~PartitionRaw(){}


int PartitionRaw::create(const uint aSizeInPages)
{
	if(exists())
	{
		if(isRawDevice())
		{
			return format();
		}
	}
	return -1;
}

int PartitionRaw::remove()
{
	//write zeros?
	return -1;
}
