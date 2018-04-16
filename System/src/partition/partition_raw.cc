#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPartitionID, const control_block_t& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock)
{
	init();
}

PartitionRaw::~PartitionRaw(){}


void PartitionRaw::create()
{
	if(exists())
	{
		if(isRawDevice())
		{
			format();
		}
	}
}

void PartitionRaw::remove()
{
	//write zeros?
}
