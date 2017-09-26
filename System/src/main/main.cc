//#include "partition/partition_manager.hh"
#include "partition/partition.hh"

#include <cmath>
#include <iostream>


int main(const int argc, const char* argv[])
{
	Partition test(argv[1]);
	test.createPartition();
	// int offset = test.allocBlock();	//can only be tested if the FSIP interpreter works.
	// test.freeBlock(offset); //can only be tested if the FSIP interpreter works.
	test.removePartition();

	return 0;
}