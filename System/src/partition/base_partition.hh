/**
 *  @file 	base_partition.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A class implementing the base partition class
 *  @bugs	Currently no bugs known
 *  @todos	Implement the class.. Atm it is just a general overview which functionalities a partition needs to provide
 *  @section TBD
 */
#ifndef BASE_PARTITION_HH
#define BASE_PARTITION_HH

#include "infra/types.hh"

class BasePartition
{
	public:
		explicit BasePartition();
		BasePartition(const BasePartition& aPartition) = delete;
		BasePartition& operator=(const BasePartition& aPartition) = delete;
		virtual ~BasePartition() = 0;

	public:
		virtual const int openPartition(const std::string aMode);
		virtual const int closePartition();
		virtual const int createPartition();
		virtual const int removePartition();
		virtual const int allocPage();
		virtual const int freePage();
		virtual const int readPage();
		virtual const int writePage();

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