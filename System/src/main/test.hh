#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "partition/partition_manager.hh"
#include "segment/segment.hh"
#include "segment/segment_manager.hh"

void test(const std::string aPath, const uint aPageSize, const uint aSegmentIndexPage, const uint aGrwothIndicator) {
    std::cout << "\n" << std::endl;

    PartitionFile *lPartFile = PartitionManager::getInstance().createPartitionFileInstance(aPath, "DefaultName", aPageSize, aSegmentIndexPage, aGrwothIndicator);
	std::cout << "Size in Pages: " << lPartFile->getSizeInPages() << std::endl;

    if(lPartFile->create(1) == -1){
        std::cout << "open" << std::endl;
    }
    /*if(PartitionManager::getInstance().getPartition(lPartFile->getID())->open() == -1){
        std::cout << "Something went wrong opening the partition..." << std::endl;
    }*/
    /*std::cout << ">>>>>Partition with path " << PartitionManager::getInstance().getPartition(lPartFile->getID())->getPath() << " is open!" << std::endl;
   */ 
    if(PartitionManager::getInstance().getPartition(lPartFile->getID())->open()==-1){
            std::cout << ">>>>>Partition with path " << PartitionManager::getInstance().getPartition(lPartFile->getID())->getPath() << " is not open!" << std::endl;

    }
    uint lNoFreePage = PartitionManager::getInstance().getPartition(lPartFile->getID())->allocPage();
    std::cout << "get free Page results in Page " <<	lNoFreePage << std::endl;
}
