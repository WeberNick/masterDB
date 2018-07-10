/**
 *  @file 	partition_raw.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	Implementation of a raw device partition
 *  @bugs	Currently no bugs known
 *  @todos	Implement all functionalities
 *  @section TBD
 */

#pragma once

#include "partition_base.hh"
#include "../infra/partition_t.hh"

class PartitionRaw : public PartitionBase
{
	private:
		friend class PartitionManager;
        PartitionRaw() = delete;
		PartitionRaw(const std::string& aPath, const std::string& aName, const uint8_t aPartitionID, const CB& aControlBlock);
		PartitionRaw(const Partition_T& aTuple, const CB& aControlBlock);
		explicit PartitionRaw(const PartitionRaw&) = delete;
        explicit PartitionRaw(PartitionRaw&&) = delete;
		PartitionRaw& operator=(const PartitionRaw&) = delete;
        PartitionRaw& operator=(PartitionRaw&&) = delete;

    public:
		~PartitionRaw();

    public:
        /**
         *  @brief  Wrapper for call to allocPage in PartitonBase (this handles specific behaviour)
         *  @return an index to the allocated page
         *  @see    partition_base.hh
         */
        uint32_t allocPage() override;
        
        /**
        * @brief Retrieves the size of the raw partition
        */
        size_t partSize() override;
        size_t partSizeInPages() override;

	private:
	    /**
	     *	@brief 	formats the raw partition	
	     */
	    void create() override;

	    /**
	     *	@brief	runs format on partition, setting all pages as free
	     */
	    void remove() override;
};
