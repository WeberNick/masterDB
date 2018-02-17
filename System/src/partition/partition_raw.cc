#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPageSize, const uint aPartitionID) :
	PartitionBase(aPath, aName, aPageSize, aPartitionID)
{

}

PartitionRaw::~PartitionRaw(){}


int PartitionRaw::create(const uint aSizeInPages)
{
        //check whether raw partition exists and if call format()
	return format();
}

int PartitionRaw::remove()
{
	//write zeros?
	return 0;
}
