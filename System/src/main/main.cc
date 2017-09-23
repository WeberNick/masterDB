// #include "partition/partition_manager.hh"
#include "partition/partition.hh"


int main(const int argc, const char* argv[])
{
	Partition test(argv[1]);
	test.createPartition();
	test.removePartition();


	return 0;
}