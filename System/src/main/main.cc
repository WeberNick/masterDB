#include "infra/types.hh"
#include "partition/partition_manager.hh"
#include "partition/partition_file.hh"

#include <iostream>

int main(const int argc, const char* argv[])
{
	PartitionManager lPartMngr;
	PartitionFile* lPartFile = lPartMngr.createPartitionFileInstance(argv[1], c_PartitionSizeInPages(), c_PageSize(), c_GrowthIndicator());

	std::cout << "The created partition instance has address: " << (void*)lPartFile << std::endl;
	std::cout << "This should have the same address: " << (void*)lPartMngr.getPartition(lPartFile->getID()) << std::endl;


	std::cout << ">>>>>Create the partition on disk..." << std::endl;
	if(lPartFile->createPartition() == -1)
	{
		std::cout << "Something went wrong creating the partition..." << std::endl;	//will be printed as long reservePage in fsip is not implemented)
	}
	if(lPartMngr.getPartition(lPartFile->getID())->openPartition() == -1)
	{
		std::cout << "Something went wrong opening the partition..." << std::endl;
	}
	std::cout << ">>>>>Partition with path " << lPartMngr.getPartition(lPartFile->getID())->getPath() << " is open!" << std::endl;
	

	//Add further tests...







	if(lPartMngr.getPartition(lPartFile->getID())->closePartition() == -1)
	{
		std::cout << "Something went wrong closing the partition..." << std::endl;
	}
	std::cout << ">>>>>Partition is closed!" << std::endl;
	std::cout << ">>>>>Remove the partition from disk..." << std::endl;
	if(lPartFile->removePartition() == -1)
	{
		std::cout << "Something went wrong removing the partition..." << std::endl;
	}

	return 0;
}