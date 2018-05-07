/**
 *  @file    partition_file.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   -Update LSN?
 *           -implement extend of file
 *  @section TBD
 */
#pragma once

#include "partition_base.hh"

#include <string>
#include <cmath>
#include <cstdlib>


class PartitionFile : public PartitionBase
{
    private:
        friend class PartitionManager;
        explicit PartitionFile(const std::string aPath, const std::string aName, const uint aPartitionID, const uint aGrowthIndicator,const CB& aControlBlock);
        PartitionFile(const part_mem_t& aTuple, const CB& aControlBlock);
        PartitionFile(const PartitionFile&) = delete;
        PartitionFile &operator=(const PartitionFile&) = delete;
    public:
        ~PartitionFile() = default;

    public:
        /**
         *  @brief  Wrapper for call to allocPage in PartitonBase (this handles specific behaviour)
         *  @return an index to the allocated page
         *  @see    partition_base.hh
         */
        virtual uint32_t allocPage();
        
        /**
        * @brief Retrieves the size of the file
        */
        virtual size_t partSize();
        virtual size_t partSizeInPages();
        inline uint getGrowthIndicator(){ return _growthIndicator; }


    private:
        void create();
        void extend();
        void remove();
        void printPage(uint aPageIndex);

    private:
        /* An indicator how the partition will grow (indicator * block size) */
        uint _growthIndicator;
};

