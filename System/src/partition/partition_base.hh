/**
 *  @file 	partition_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every partition
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#ifndef PARTITION_BASE_HH
#define PARTITION_BASE_HH

#include "infra/types.hh"

class PartitionBase
{
	protected:
		friend class PartitionManager;
		explicit PartitionBase(const char* aPath, const uint aNoPages, const uint aPageSize, const uint aPartitionID);
		PartitionBase(const PartitionBase& aPartition) = delete;
		PartitionBase& operator=(const PartitionBase& aPartition) = delete;
		virtual ~PartitionBase() = 0;

	public:
		virtual int openPartition() = 0;
		virtual int closePartition() = 0;
		virtual int allocPage() = 0;
		virtual int freePage(const uint aPageIndex) = 0;
		virtual int readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize) = 0;
		virtual int writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize) = 0;

	public:
		inline const char* 	getPath(){ return _partitionPath; }
		inline uint 		getSizeInPages(){ return _sizeInPages; }
		inline uint 		getPageSize(){ return _pageSize; }
		inline uint8_t 		getID(){ return _partitionID; }
		inline uint 		getSegmentIndexPage(){ return _segmentIndexPage; }

	protected:
		/* A path to a partition (i.e., a file) */
		const char* _partitionPath;
		/* The current size of the partition in number of pages */
		uint _sizeInPages;
		/* The block size in bytes, used by the partition */
		uint _pageSize;
		/* An ID representing this partition */
		uint8_t _partitionID;
		/* The index within the partition where the segment manager is stored */
		uint _segmentIndexPage;
		/* Helper flag if partition is already created */
		bool _isCreated;
		/* Helper flag if partition is open */
		bool _isOpen;
};


#endif