/**
 *  @file 	partition_raw.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every partition
 *  @bugs	Currently no bugs known
 *  @todos	Implement all functionalities
 *  @section TBD
 */
#ifndef PARTITION_RAW_HH
#define PARTITION_RAW_HH

#include "partition_base.hh"

class PartitionRaw : public PartitionBase
{
	private:
		friend class PartitionManager;
		explicit PartitionRaw(const std::string aPath, const std::string aName, const uint aNoPages, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID);
		PartitionRaw(const PartitionRaw& aPartition) = delete;
		PartitionRaw& operator=(const PartitionRaw& aPartition) = delete;
		~PartitionRaw();

	public:
		int open();
		int close();
		int createPartition();
		int removePartition();
		int allocPage();
		int freePage(const uint aPageIndex);
		int readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize);
		int writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize);
};


#endif