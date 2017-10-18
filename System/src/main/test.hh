#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "partition/partition_manager.hh"
#include "segment/segment.hh"
#include "segment/segment_manager.hh"


void test(const std::string aPath, const uint aPartitionSize, const uint aPageSize, const uint aSegmentIndexPage, const uint aGrwothIndicator)
{
	std::cout << "\n" << std::endl;

    PartitionFile *lPartFile = PartitionManager::getInstance().createPartitionFileInstance(aPath, "DefaultName", aPartitionSize, aPageSize, aSegmentIndexPage, aGrwothIndicator);

    /***************************************************************************************************
	** Test creation of partition on disk **************************************************************
	***************************************************************************************************/

    std::cout << "> Creating partition on disk..." << std::endl;
    if (lPartFile->createPartition() != 0) // This could be commented out
        std::cout << "\033[0;31m> Something went wrong while creating the partition...\033[0m" << std::endl;

    /***************************************************************************************************
	** Test deletion of partition on disk **************************************************************
	***************************************************************************************************/
	 std::cout << "> Remove the partition from disk...\n" << std::endl;
    if (lPartFile->removePartition() != 0) // This could be commented out
        std::cout << "\033[0;31m> Something went wrong removing the partition...\033[0m\n" << std::endl;
}