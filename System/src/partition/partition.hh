/**
 *  @file    partition.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   Todos: Update LSN etc. if a block is allocated (think about communication with fsip_interpreter)
 *  @section TBD
 */
#ifndef PARTITION_HH
#define PARTITION_HH

#include "infra/types.hh"
#include "interpreter/fsip_interpreter.hh"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmath>
#include <iostream>

class Partition
{
	public:
		Partition(const char* aPath, const uint64_t aPartitionSize = 2048000000, const uint aBlockSize = 4096, const uint aGrowthIndicator = 1000000, const mode_t aAccessRights = 0644);
		Partition(const Partition& aPartition) = delete;
		Partition& operator=(const Partition& aPartition) = delete;
		~Partition();

	public:
		const int createPartition();
		const int removePartition();
		const int allocBlock();
		const int freeBlock(const uint aBlockNo);
		const int readBlock(byte* aBuffer, const uint aBlockNo, const uint aBufferSize = 4096);
		const int writeBlock(const byte* aBuffer, const uint aBlockNo, const uint aBufferSize = 4096);

	public:
		inline const uint getPartitionID(){ return _partitionID; }

	private:
		const uint totalBlocks();
		const int init();

	private:
		/* A path to a partition (i.e., a file) */
		const char* _partitionPath;
		/* The current size of the partition in bytes */
		uint64_t _partitionSize;
		/* The block size in bytes, used by the partition */
		uint _blockSize;
		/* An indicator how the partition will grow (indicator * block size) */			
		uint _growthIndicator;
		/* Access rights to the partition */	
		mode_t _accessRights;
		/* Helper flagh if partition is already created */
		bool _isCreated;
		/* Helper flagh if partition is already removed */
		bool _isRemoved;
		/* A counter for the next partition id */
		static uint s_IDCounter;
		/* An ID representing this partition */
		uint _partitionID;
		/* A counter for the next LSN */
		static long s_LSNCounter;
};


#endif