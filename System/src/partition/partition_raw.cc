#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPartitionID, const control_block_t& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock)
{
	init();
}

PartitionRaw::PartitionRaw(part_t aTuple, control_block_t& aControlBlock):
	PartitionBase(aTuple._pPath, aTuple._pName, aTuple._pID, aControlBlock)
{
	assignSize(_sizeInPages);
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
