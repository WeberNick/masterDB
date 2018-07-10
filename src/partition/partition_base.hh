/**
 *  @file 	partition_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	An abstract class implementing the interface for every partition
 *  @bugs	Currently no bugs known
 *  @todos	-
 *  @section TBD
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/file_util.hh"
#include "../infra/header_structs.hh"
#include "../interpreter/interpreter_fsip.hh"

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
    // unsupported
    #define P_NO_BLOCKS 0 
    #define P_BLOCK_SIZE 0 
    #error SYSTEM IS NOT COMPATIBLE WITH NON-UNIX OPERATING SYSTEMS
#endif

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>

// placeholder
constexpr uint64_t LSN = 0;

class PartitionBase 
{
    protected:
        friend class PartitionManager;
        PartitionBase() = delete;
        PartitionBase(const std::string& aPath, const std::string& aName, const uint8_t aPartitionID, const CB& aControlBlock);
        explicit PartitionBase(const PartitionBase&) = delete;
        explicit PartitionBase(PartitionBase&&) = delete;
        PartitionBase& operator=(const PartitionBase&) = delete;
        PartitionBase& operator=(PartitionBase&&) = delete;

    public:
        virtual ~PartitionBase();

    public:
        /**
         *  @brief  Opens the partition in read/write mode. If the partition is already open, an open counter
         *          will be increased
         *  @throws FileException on failure
         *  @see    infra/exception.hh
         */
        void open();
    
        /**
         *  @brief  Closes the partition. If the open count is greater than 1, it is decreased. Otherwise the
         *          partition will be closed and the file descriptor is set to -1
         *
         *  @throws FileException on failure
         *  @see    infra/exception.hh
         */
        void close();
    
        /**
         *  @brief  Allocates a new page in the partition
         *  @return an index to the allocated page
         *
         *  @throws
         *  @see    interpreter/interpreter_fsip.hh, infra/exception.hh
         *  @note   doesn't use the buffer manager yet but retrieves its fsips directly
         */
        virtual uint32_t allocPage();
    
        /**
         *  @brief  Physically remove a page by setting its bit in the fsip
         *
         *  @param  aPageIndex: an index indicating which page to remove
         *  @see    interpeter/interpreter_fsip.hh
         */
        void freePage(const uint32_t aPageIndex);
    
        /**
         *  @brief  Read a page from the partition into a main memory buffer
         *
         *  @param  aBuffer: the buffer to read the page into
         *  @param  aPageIndex: an index indicating which page to read
         *  @param  aBufferSize: size of the buffer in bytes
         *  @throws FileException on failure
         *  @see    infra/exception.hh
         */
        void readPage(byte* aBuffer, const uint32_t aPageIndex, const uint aBufferSize);
    
        /**
         *  @brief  Write a page from a main memory buffer on the partition
         *
         *  @param  aBuffer: where to write from
         *  @param  aPageIndex: an index indicating which page to write
         *  @param  aBufferSize: size of the buffer in bytes
         *  @throws FileException on Failure
         *  @see    infra/exception.hh
         */
        void writePage(const byte* aBuffer, const uint32_t aPageIndex, const uint aBufferSize);

    public:
        // Getter
        inline const std::string& getPath() const noexcept { return _partitionPath; }
        inline const std::string& getPath() noexcept { return _partitionPath; }
        inline const std::string& getName() const noexcept { return _partitionName; }
        inline const std::string& getName() noexcept { return _partitionName; }
        inline uint getPageSize() const noexcept { return _pageSize; }
        inline uint getPageSize() noexcept { return _pageSize; }
        inline uint getSizeInPages() const noexcept { return _sizeInPages; }
        inline uint getSizeInPages() noexcept { return _sizeInPages; }
        inline uint8_t getID() const noexcept { return _partitionID; }
        inline uint8_t getID() noexcept { return _partitionID; }
  
        inline std::string to_string() const noexcept;
        inline std::string to_string() noexcept { return static_cast<const PartitionBase&>(*this).to_string(); }

    protected:
        /**
         *  @brief  Format the partition by initializing its fsip
         *
         */
        void format();
    
        /**
         *  @brief  Physically create the partition. Needs to be implemented by respective partition type
         *
         *  @see    partition_file.hh, partition_raw.hh
         */
        virtual void create() = 0;
    
        /**
         *  @brief  Physically removes the partition. Needs to be implemented by respective partition type
         *
         *  @see    partition_file.hh, partition_raw.hh
         */
        virtual void remove() = 0;

    protected:
        // Wrapper call. Checks if a file exists at the partition path
        inline bool exists() noexcept { return FileUtil::exists(_partitionPath); }
        // Wrapper call. Checks if the partition path is a regular file
        inline bool isFile() noexcept { return FileUtil::isFile(_partitionPath); }
        // Wrapper call. Checks if the partition path is a raw device
        inline bool isRawDevice() noexcept { return FileUtil::isRawDevice(_partitionPath); }
        virtual size_t partSize() = 0;
        virtual size_t partSizeInPages() = 0;
        uint getMaxPagesPerFSIP() noexcept;

    protected:
        std::string _partitionPath; // A path to a partition (i.e., a file)
        std::string _partitionName; // Name of the partition
        uint _pageSize;             // The page size in bytes, used by the partition
        uint _sizeInPages;          // The current size of the partition in number of pages
        uint8_t _partitionID;       // An ID representing this partition
        uint _openCount;            // Counts the number of open calls
        int _fileDescriptor;        // The partitions file descriptor
        const CB& _cb;              // Control block containing all specified parameters*/
};

std::string PartitionBase::to_string() const noexcept
{
    return std::string("Path : '")
      + getPath() + "', Name: '"
      + getName() + "', ID : "
      + std::to_string(getID());
}

std::ostream& operator<< (std::ostream& stream, const PartitionBase& aPartition);
