#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPartitionID, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock)
{
	init();
}

PartitionRaw::PartitionRaw(const part_t& aTuple, const CB& aControlBlock):
	PartitionBase(aTuple._pPath, aTuple._pName, aTuple._pID, aControlBlock)
{
	_sizeInPages = retrieveSizeInPages(); 
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
