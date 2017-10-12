/**
 *  @file    partition_file.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   Todos: init(): where to calc. lPagesPerFSIP?, Update LSN?
 *  @section TBD
 */
#ifndef PARTITION_FILE_HH
#define PARTITION_FILE_HH

#include "partition_base.hh"
#include "interpreter/fsip_interpreter.hh"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmath>
#include <cstring>
#include <iostream>

const uint64_t LSN = 0;

class PartitionFile : public PartitionBase
{
	private:
		friend class PartitionManager;
		explicit PartitionFile(const char* aPath, const uint aNoPages, const uint aPageSize, const uint aGrowthIndicator, const uint aPartitionID);
		PartitionFile(const PartitionFile& aPartition) = delete;
		PartitionFile& operator=(const PartitionFile& aPartition) = delete;
		~PartitionFile();

	public:
		/**
		 *	@brief	opens the file in read/write mode
		 *	@return	an int representing a file descriptor, -1 on failure
		 */
		int openPartition();

		/**
		 *	@brief	closes the open file
		 *	@return	0 if successful, -1 on failure
		 */
		int closePartition();

		/**
		 *	@brief	creates the partition on disk
		 *	@param	aAccessRights: access rights to the partition
		 *	@return	0 if successful, -1 on failure
		 */
		int createPartition();

		/**
		 *	@brief	removes the partition on disk
		 *	@return	0 if successful, -1 on failure
		 */
		int removePartition();

		/**
		 *	@brief	allocates a free page
		 *	@return	an index to the allocated page, -1 on failure
		 */
		int allocPage();

		/**
		 *	@brief	removes a page
		 *	@param	aPageIndex: an index indicating which page to remove
		 *	@return	0 if successful, -1 on failure
		 */
		int freePage(const uint aPageIndex);

		/**
		 *	@brief	reads a page
		 *	@param	aFileDescriptor: aFileDescriptor: a file to read
		 *	@param	aBuffer: where to read into
		 *	@param	aPageIndex: an index indicating which page to read
		 *	@param	aBufferSize: size of the page
		 *	@return	0 if successful, -1 on failure
		 */
		int readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize);

		/**
		 *	@brief	writes a page
		 *	@param	aFileDescriptor: aFileDescriptor: a file to write
		 *	@param	aBuffer: where to write from
		 *	@param	aPageIndex: an index indicating which page to write
		 *	@param	aBufferSize: size of the page
		 *	@return	0 if successful, -1 on failure
		 */
		int writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize);
		/**
		 *	@brief	prints a page into a file decoded in hex, usefull for debugging. 
		 *	@param	aPageIndex: indicating which page to print
		 */
		void printPage(uint aPageIndex);

	public:
		inline uint getGrowthIndicator(){ return _growthIndicator; }

	private:
		uint getMaxPagesPerFSIP();
		int init();

	private:
		/* An indicator how the partition will grow (indicator * block size) */			
		uint _growthIndicator;
		/* The partitions file descriptor */
		int _fileDescriptor;
};

#endif