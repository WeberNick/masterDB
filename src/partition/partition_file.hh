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


class PartitionFile : public PartitionBase  {
  private:
    friend class PartitionManager;
    explicit PartitionFile(const std::string aPath, const std::string aName, const uint aGrowthIndicator, const uint aPartitionID, const CB& aControlBlock);
    PartitionFile(const part_t& aTuple, const CB& aControlBlock);
    PartitionFile(const PartitionFile&) = delete;
    PartitionFile &operator=(const PartitionFile&) = delete;
  public:
    ~PartitionFile();

  private:
    void create();
    void extend(const uint aNoPages);
    void remove();
    void printPage(uint aPageIndex);

  private:
    /* An indicator how the partition will grow (indicator * block size) */
    uint _growthIndicator;
};

