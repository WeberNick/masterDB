/**
 *  @file    partition_file.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A class implementing the interface of a partition stored on disk (currently a file)
 *  @bugs    Currently no bugs known
 *  @todos   Todos: init(): where to calc. lPagesPerFSIP?, Update LSN?
 *  @section TBD
 */
#ifndef PARTITION_FILE_HH
#define PARTITION_FILE_HH

#include "partition_base.hh"

#include <cmath>
#include <iostream>

class PartitionFile : public PartitionBase {
  private:
    friend class DatabaseInstanceManager;
    friend class PartitionManager;
    explicit PartitionFile(const std::string aPath, const std::string aName, const uint aPageSize, const uint aGrowthIndicator, const uint aPartitionID);
    PartitionFile(const PartitionFile &aPartition) = delete;
    PartitionFile &operator=(const PartitionFile &aPartition) = delete;
    ~PartitionFile();

  public:
    /**
     *	@brief 	if file does not exist, create it
	   *	@param 	aSizeInPages size with which the partition is created
     *	@return	0 if successful, -1 on failure
     */
        //check whether path exists and references a file
    int create(const uint aSizeInPages);

    /**
     *	@brief	removes the file from disk
     *	@return	0 if successful, -1 on failure
     */
    int remove();

    int extend(const uint aNoPages);

    void printPage(uint aPageIndex);

  public:
    inline uint getGrowthIndicator() { return _growthIndicator; }

  private:
    /* An indicator how the partition will grow (indicator * block size) */
    uint _growthIndicator;
};

#endif
