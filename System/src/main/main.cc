#include "config.hh"
#include "partition/partition_manager.hh"
#include "partition/file_partition.hh"

int main(const int argc, const char* argv[])
{
	const uint pID = 0;
	FilePartition test(argv[1], C_PARTITION_SIZE, C_PAGE_SIZE, C_GROWTH_INDICATOR, pID);
	test.createPartition(C_ACCESS_RIGHTS);
	// int offset = test.allocPage();	//can only be tested if the FSIP interpreter works.
	// test.freePage(offset); //can only be tested if the FSIP interpreter works.
	test.removePartition();

	return 0;
}