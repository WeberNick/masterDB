/**
 *  @file    file_partition.hh
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
		explicit FilePartition(const char* aPath, const uint64_t aPartitionSize, const uint aPageSize, const uint aGrowthIndicator, const uint aPartitionID);
		FilePartition(const FilePartition& aPartition) = delete;
		FilePartition& operator=(const FilePartition& aPartition) = delete;
		~FilePartition();

	public:
		/**
		 *	@brief	opens the file in a given mode
		 *	@param	aMode: a string representing the mode in which to open the file (r/w/rw)
		 *	@return	an int representing a file descriptor, -1 on failure
		 */
		const int openPartition(const std::string aMode);

		/**
		 *	@brief	closes the open file
		 *	@param	aFileDescriptor: a file to close
		 *	@return	0 if successful, -1 on failure
		 */
		const int closePartition(const int aFileDescriptor);

		/**
		 *	@brief	creates the partition on disk
		 *	@param	aAccessRights: access rights to the partition
		 *	@return	0 if successful, -1 on failure
		 */
		const int createPartition(mode_t aAccessRights);

		/**
		 *	@brief	removes the partition on disk
		 *	@return	0 if successful, -1 on failure
		 */
		const int removePartition();

		/**
		 *	@brief	allocates a free page
		 *	@return	an index to the allocated page, -1 on failure
		 */
		const int allocPage();

		/**
		 *	@brief	removes a page
		 *	@param	aPageNo: an index indicating which page to remove
		 *	@return	0 if successful, -1 on failure
		 */
		const int freePage(const uint aPageNo);

		/**
		 *	@brief	reads a page
		 *	@param	aFileDescriptor: aFileDescriptor: a file to read
		 *	@param	aBuffer: where to read into
		 *	@param	aPageNo: an index indicating which page to read
		 *	@param	aBufferSize: size of the page
		 *	@return	0 if successful, -1 on failure
		 */
		const int readPage(const int aFileDescriptor, byte* aBuffer, const uint aPageNo, const uint aBufferSize);

		/**
		 *	@brief	writes a page
		 *	@param	aFileDescriptor: aFileDescriptor: a file to write
		 *	@param	aBuffer: where to write from
		 *	@param	aPageNo: an index indicating which page to write
		 *	@param	aBufferSize: size of the page
		 *	@return	0 if successful, -1 on failure
		 */
		const int writePage(const int aFileDescriptor, const byte* aBuffer, const uint aPageNo, const uint aBufferSize);

	public:
		inline const uint8_t getPartitionID(){ return _partitionID; }
		inline const size_t getPageSize(){ return _pageSize; }

	private:
		const uint totalPages();
		const int init();
		const int reserve(uint aIndex);

	private:
		/* A path to a partition (i.e., a file) */
		const char* _partitionPath;
		/* The current size of the partition in bytes */
		uint64_t _partitionSize;
		/* The block size in bytes, used by the partition */
		size_t _pageSize;
		/* An indicator how the partition will grow (indicator * block size) */			
		uint _growthIndicator;
		/* An ID representing this partition */
		uint8_t _partitionID;
		/*   */
		uint _segmentIndexPage;
		/* Helper flag if partition is already created */
		bool _isCreated;
		/* Helper flag if partition is open */
		bool _isOpen;
};

#endif