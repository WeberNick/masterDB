#include "infra/types.hh"
#include "partition/partition_manager.hh"
#include "partition/partition_file.hh"

int main(const int argc, const char* argv[])
{
	const uint pID = 0;
	PartitionFile test(argv[1], c_PartitionSizeInPages(), c_PageSize(), pID, c_GrowthIndicator());
	test.createPartition();
	// int offset = test.allocPage();	//can only be tested if the FSIP interpreter works.
	// test.freePage(offset); //can only be tested if the FSIP interpreter works.
	test.removePartition();

	return 0;
}