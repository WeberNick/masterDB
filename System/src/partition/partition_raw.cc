#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPartitionID, const control_block_t& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock)
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
