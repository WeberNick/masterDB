/**
 *  @file 	partition_raw.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every partition
 *  @bugs	Currently no bugs known
 *  @todos	Implement all functionalities
 *  @section TBD
 */
#pragma once

#include "partition_base.hh"

class PartitionRaw : public PartitionBase
{
	private:
		friend class PartitionManager;
        explicit PartitionRaw() = delete;
		explicit PartitionRaw(const std::string aPath, const std::string aName, const uint aPartitionID, const CB& aControlBlock);
		explicit PartitionRaw(part_t aTuple, const CB& aControlBlock);
		explicit PartitionRaw(const PartitionRaw&) = delete;
        explicit PartitionRaw(PartitionRaw&&) = delete;
		PartitionRaw& operator=(const PartitionRaw&) = delete;
        PartitionRaw& operator=(PartitionRaw&&) = delete;
		~PartitionRaw();

	public:
	    /**
	     *	@brief 	formats the raw partition	
	     */
	    void create();

	    /**
	     *	@brief	does currently nothing
	     */
	    void remove();

};
