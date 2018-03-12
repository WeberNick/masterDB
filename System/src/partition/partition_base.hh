/**
 *  @file 	partition_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every partition
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#ifndef PARTITION_BASE_HH
#define PARTITION_BASE_HH

#include "infra/types.hh"
#include "infra/error.hh"
#include "infra/header_structs.hh"
#include "interpreter/interpreter_fsip.hh"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __linux__
  #include <linux/fs.h>
  #define P_NO_BLOCKS BLKGETSIZE
  #define P_BLOCK_SIZE BLKSSZGET
#elif __APPLE__
  #include <sys/disk.h>
  #define P_NO_BLOCKS DKIOCGETBLOCKCOUNT 
  #define P_BLOCK_SIZE DKIOCGETBLOCKSIZE 
#else
	//unsupported
  #define P_NO_BLOCKS 0 
  #define P_BLOCK_SIZE 0 
#endif

#include <iostream>
#include <string>
#include <cstring>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

const uint64_t LSN = 0;

class PartitionBase {
  protected:
    friend class PartitionManager;
    explicit PartitionBase(const std::string aPath, const std::string aName, const uint aPartitionID, const control_block_t& aControlBlock);
    PartitionBase(const PartitionBase &aPartition) = delete;
    PartitionBase &operator=(const PartitionBase &aPartition) = delete;
    virtual ~PartitionBase() = 0;

  public:
    int format();
    /**
     *  @brief  opens the file in read/write mode
     *  @return an int representing a file descriptor, -1 on failure
     */
    int open();

    /**
     *  @brief  closes the open file
     *  @return 0 if successful, -1 on failure
     */
    int close();

    /**
     *  @brief  allocates a free page
     *  @return an index to the allocated page, -1 on failure
     */
    int allocPage();

    /**
     *  @brief  removes a page
     *  @param  aPageIndex: an index indicating which page to remove
     *  @return 0 if successful, -1 on failure
     */
    int freePage(const uint aPageIndex);

    /**
     *  @brief  reads a page
     *  @param  aFileDescriptor: aFileDescriptor: a file to read
     *  @param  aBuffer: where to read into
     *  @param  aPageIndex: an index indicating which page to read
     *  @param  aBufferSize: size of the page
     *  @return 0 if successful, -1 on failure
     */
    int readPage(byte *aBuffer, const uint aPageIndex, const uint aBufferSize);

    /**
     *  @brief  writes a page
     *  @param  aFileDescriptor: aFileDescriptor: a file to write
     *  @param  aBuffer: where to write from
     *  @param  aPageIndex: an index indicating which page to write
     *  @param  aBufferSize: size of the page
     *  @return 0 if successful, -1 on failure
     */
    int writePage(const byte *aBuffer, const uint aPageIndex, const uint aBufferSize);


    
	virtual int create(const uint aSizeInPages) = 0;
	virtual int remove() = 0;

  public:
    inline std::string getPath() { return _partitionPath; }
    inline std::string getName() { return _partitionName; }
    inline uint getPageSize() { return _pageSize; }
    inline uint getSizeInPages() { return _sizeInPages; }
    inline uint8_t getID() { return _partitionID; }
    inline uint getOpenCount() { return _openCount; }

  protected:
    inline bool exists(){ return fs::exists(_partitionPath); }
    inline bool isFile(){ return fs::is_regular_file(_partitionPath); }
    inline bool isRawDevice(){ return fs::is_block_file(_partitionPath); }

  protected:
    int assignSize(uint& aSize);
	void init();
    uint getMaxPagesPerFSIP();


  protected:
    /* A path to a partition (i.e., a file) */
    std::string _partitionPath;
    /* Name of the partition */
    std::string _partitionName;
    /* The page size in bytes, used by the partition */
    uint _pageSize;
    /* The current size of the partition in number of pages */
    uint _sizeInPages;
    /* An ID representing this partition */
    uint8_t _partitionID;
    /* Counts the number of open calls */
    uint _openCount;
    /* The partitions file descriptor */
    int _fileDescriptor;

    const control_block_t& _controlBlock;
};

#endif
