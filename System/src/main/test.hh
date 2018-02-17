#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "partition/partition_manager.hh"
#include "segment/segment.hh"
#include "segment/segment_manager.hh"

void test(const std::string aPath, const uint aPageSize, const uint aGrwothIndicator) {
    std::cout << "\n" << std::endl;

    PartitionFile *lPartFile = PartitionManager::getInstance().createPartitionFileInstance(aPath, "DefaultName", aPageSize, aGrwothIndicator);

    if(lPartFile->create(1000) == -1){
        std::cout << "open" << std::endl;
    }
    
	std::cout << "Size in Pages: " << lPartFile->getSizeInPages() << std::endl;
    PartitionManager::getInstance().getPartition(lPartFile->getID())->open();

    uint lNoFreePage = PartitionManager::getInstance().getPartition(lPartFile->getID())->allocPage();
    std::cout << "get free Page results in Page " <<	lNoFreePage << std::endl;
}
