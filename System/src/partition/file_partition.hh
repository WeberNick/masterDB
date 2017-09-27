/**
 *  @file    partition.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   Todos: init(): where to calc. lPagesPerFSIP?, Update LSN?
 *  @section TBD
 */
#ifndef FILE_PARTITION_HH
#define FILE_PARTITION_HH

#include "infra/types.hh"
#include "interpreter/fsip_interpreter.hh"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmath>
#include <cstring>
#include <iostream>

const uint64_t LSN = 0;

class FilePartition
{
	public:
		FilePartition(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator, const uint aPartitionID);
		FilePartition(const FilePartition& aPartition) = delete;
		FilePartition& operator=(const FilePartition& aPartition) = delete;
		~FilePartition();

	public:
		const int openPartition(const std::string aMode);
		const int closePartition(const int aFileDescriptor);
		const int createPartition(mode_t aAccessRights);
		const int removePartition();
		const int allocPage();
		const int freePage(const uint aPageNo);
		const int readPage(const int aFileDescriptor, byte* aBuffer, const uint aPageNo, const uint aBufferSize);
		const int writePage(const int aFileDescriptor, const byte* aBuffer, const uint aPageNo, const uint aBufferSize);

	public:
		inline const uint getPartitionID(){ return _partitionID; }

	private:
		const uint totalPages();
		const int init();

	private:
		/* A path to a partition (i.e., a file) */
		const char* _partitionPath;
		/* The current size of the partition in bytes */
		uint64_t _partitionSize;
		/* The block size in bytes, used by the partition */
		uint _pageSize;
		/* An indicator how the partition will grow (indicator * block size) */			
		uint _growthIndicator;
		/* An ID representing this partition */
		uint _partitionID;
		/* Helper flag if partition is already created */
		bool _isCreated;
		/* Helper flag if partition is open */
		bool _isOpen;
};


#endif