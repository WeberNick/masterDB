#include "infra/types.hh"
#include "partition/partition_manager.hh"
#include "partition/partition_file.hh"
#include "infra/bit_intrinsics.hh"


#include <iostream>


// pfad zur Patition beim Ausfuehren mit angeben!
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
	/*std::cout<<"highest bit set in all 1 "<<idx_highest_bit_set<uint64_t>(~0)<<std::endl;
	std::cout<<"highest bit set in 1"<<idx_highest_bit_set<uint64_t>(1)<<std::endl;
	std::cout<<"lowest bit set in all 1 "<<idx_lowest_bit_set<uint64_t>(~0)<<std::endl;
*/
	uint nxFreePage = lPartMngr.getPartition(lPartFile->getID())->allocPage();
	std::cout << "get free Page results in Page " <<	nxFreePage << std::endl;
	lPartMngr.getPartition(lPartFile->getID())->freePage(2);

	








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