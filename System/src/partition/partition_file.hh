/**
 *  @file    partition_file.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   Todos:  Update LSN?
 *  @section TBD
 */
#ifndef PARTITION_FILE_HH
#define PARTITION_FILE_HH

#include "partition_base.hh"

#include <string>
#include <cmath>
#include <cstdlib>

class PartitionFile : public PartitionBase  {
  private:
    friend class DatabaseInstanceManager;
    friend class PartitionManager;
    explicit PartitionFile(const std::string aPath, const std::string aName, const uint aGrowthIndicator, const uint aPartitionID, const control_block_t& aControlBlock);
    PartitionFile(const PartitionFile&) = delete;
    PartitionFile &operator=(const PartitionFile&) = delete;
    ~PartitionFile();

  public:
    void create();
    void remove();
    void extend(const uint aNoPages);

    void printPage(uint aPageIndex);

  public:
    inline uint getGrowthIndicator() { return _growthIndicator; }

  private:
    /* An indicator how the partition will grow (indicator * block size) */
    uint _growthIndicator;
};

#endif
