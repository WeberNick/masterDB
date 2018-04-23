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
		explicit PartitionRaw(const part_t& aTuple, const CB& aControlBlock);
		explicit PartitionRaw(const PartitionRaw&) = delete;
        explicit PartitionRaw(PartitionRaw&&) = delete;
		PartitionRaw& operator=(const PartitionRaw&) = delete;
        PartitionRaw& operator=(PartitionRaw&&) = delete;

    public:
		~PartitionRaw() = default;

    public:
        /**
         *  @brief  Wrapper for call to allocPage in PartitonBase (this handles specific behaviour)
         *  @return an index to the allocated page
         *  @see    partition_base.hh
         */
        virtual uint32_t allocPage();
        
        /**
        * @brief Retrieves the size of the raw partition
        */
        virtual size_t partSize();
        virtual size_t partSizeInPages();

	private:
	    /**
	     *	@brief 	formats the raw partition	
	     */
	    void create();

	    /**
	     *	@brief	runs format on partition, setting all pages as free
	     */
	    void remove();

};
