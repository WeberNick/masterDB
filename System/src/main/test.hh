#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "partition/partition_manager.hh"
#include "segment/segment.hh"
#include "segment/segment_manager.hh"

void test(const std::string aPath, const uint aPageSize, const uint aSegmentIndexPage, const uint aGrwothIndicator) {
    std::cout << "\n" << std::endl;

    PartitionFile *lPartFile = PartitionManager::getInstance().createPartitionFileInstance(aPath, "DefaultName", aPageSize, aSegmentIndexPage, aGrwothIndicator);
	std::cout << "Size in Pages: " << lPartFile->getSizeInPages() << std::endl;

}
