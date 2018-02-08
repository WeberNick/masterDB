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
		explicit PartitionRaw(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID);
		PartitionRaw(const PartitionRaw& aPartition) = delete;
		PartitionRaw& operator=(const PartitionRaw& aPartition) = delete;
		~PartitionRaw();

	public:
	    /**
	     *	@brief	checks whether raw device exists and assigns size if so
	     *	@return	0 if successful, -1 on failure
	     */
	    int create();

		int format();
		int init();

	    /**
	     *	@brief	does currently nothing
	     *	@return	0 if successful, -1 on failure
	     */
	    int remove();

};


#endif
