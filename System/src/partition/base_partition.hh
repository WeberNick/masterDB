/**
 *  @file 	base_partition.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A class implementing the base partition class
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#ifndef FILE_PARTITION_HH
#define FILE_PARTITION_HH

#include "infra/types.hh"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmath>
#include <cstring>
#include <iostream>

const uint64_t LSN = 0;

class BasePartition
{
	public:
		BasePartition();
		BasePartition(const BasePartition& aPartition) = delete;
		BasePartition& operator=(const BasePartition& aPartition) = delete;
		~BasePartition();

	public:
		const int openPartition(const std::string aMode);
		const int closePartition();
		const int createPartition();
		const int removePartition();
		const int allocPage();
		const int freePage();
		const int readPage();
		const int writePage();

	public:
		inline const uint getPartitionID(){ return _partitionID; }

	private:
		const uint totalPages();
		const int init();
		const int reserve(uint aIndex);

	private:
		/* The current size of the partition in bytes */
		uint64_t _partitionSize;
		/* The block size in bytes, used by the partition */
		uint _pageSize;
		/* An indicator how the partition will grow (indicator * block size) */			
		uint _growthIndicator;
		/* An ID representing this partition */
		uint _partitionID;
		/*   */
		uint _segmentIndexPage;
		/* Helper flag if partition is already created */
		bool _isCreated;
		/* Helper flag if partition is open */
		bool _isOpen;
};


#endif