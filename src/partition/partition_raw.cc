#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string& aPath, const std::string& aName, const uint8_t aPartitionID, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock)
{
    create();
    TRACE("'PartitionRaw' constructed");
}

PartitionRaw::PartitionRaw(const Partition_T& aTuple, const CB& aControlBlock):
	PartitionBase(aTuple.path(), aTuple.name(), aTuple.ID(), aControlBlock)
{
    if(exists()) _sizeInPages = partSizeInPages();
    else _sizeInPages = 0;
}

PartitionRaw::~PartitionRaw()
{
    TRACE("'PartitionRaw' object destructed");
}

uint32_t PartitionRaw::allocPage()
{
    #pragma message ("TODO: If the alloc page fails because of a full partition, an exception will be thrown. As this is a raw device, it is not possible to grow the partition. Therefore, this needs to be catched from some higher level (CLI? Segments?) and the allocation (prob. a result of an tuple insertion) must fail gracefully.")
    return PartitionBase::allocPage();
}

size_t PartitionRaw::partSize()
{
    #pragma message ("TODO: Test if functionality actually works for raw devices")
	if(isRawDevice())
	{
		int lFileDescriptor = ::open(_partitionPath.c_str(), O_RDONLY);
   		if(lFileDescriptor == -1) 
   	 	{
            const std::string lErrMsg = std::string("An error occured while opening the file: ") + std::string(std::strerror(errno));
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
        }
    	uint64_t lSector_count = 0;
    	uint32_t lSector_size = 0;
       	if(ioctl(lFileDescriptor, P_NO_BLOCKS, &lSector_count) == -1 || ioctl(lFileDescriptor, P_BLOCK_SIZE, &lSector_size) == -1)
       	{
            const std::string lErrMsg = std::string("An error occurred while using ioctl: ") + std::string(std::strerror(errno));
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
    	}
       	uint64_t lDisk_size = lSector_count * lSector_size; // in bytes
        if(lDisk_size % _pageSize != 0)
        {
            const std::string lErrMsg = std::string("Partition size modulo page size is not equal to zero");
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
        } 
        if(::close(lFileDescriptor) == -1) // call close in global namespace
		{
            const std::string lErrMsg = std::string("An error occured while closing the file: ") + std::string(std::strerror(errno));
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
		}
       return (lDisk_size / _pageSize);
    }
    return 0;
}

size_t PartitionRaw::partSizeInPages()
{
    return (partSize() / _pageSize);
}

void PartitionRaw::create()
{
    #pragma message ("TODO: Test if functionality actually works for raw devices")
	if(exists())
	{
		if(isRawDevice())		
        {
            _sizeInPages = partSizeInPages();
            TRACE("Raw partition was created");
			format();
		}
	}
}

void PartitionRaw::remove()
{
    format();
}
