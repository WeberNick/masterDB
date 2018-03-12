#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string aPath, const std::string aName, const uint aPartitionID, const control_block_t& aControlBlock) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_pageSize(aControlBlock.pageSize()),
	_sizeInPages(0),
	_partitionID(aPartitionID),
	_openCount(0),
	_fileDescriptor(-1),
	_controlBlock(aControlBlock)
{
}

PartitionBase::~PartitionBase(){}


int PartitionBase::format()
{
	byte* lPagePointer = new byte[_pageSize];
	uint lPagesPerFSIP = getMaxPagesPerFSIP();
	uint lCurrentPageNo = 0;
	InterpreterFSIP fsip;
	uint remainingPages = _sizeInPages;
	uint lNumberOfPagesToManage;
	if(open() == -1){ return -1; }
	while(remainingPages > 1)
	{
		--remainingPages;
		lNumberOfPagesToManage = ((remainingPages > lPagesPerFSIP) ? lPagesPerFSIP : remainingPages);
		fsip.initNewFSIP(lPagePointer, LSN, lCurrentPageNo, _partitionID, lNumberOfPagesToManage);
		if(writePage(lPagePointer, lCurrentPageNo, _pageSize) == -1){ return -1; }
		lCurrentPageNo += (lPagesPerFSIP + 1);
		remainingPages -= lNumberOfPagesToManage;
	}
	fsip.detach();
	readPage(lPagePointer, LSN, _pageSize);
	fsip.attach(lPagePointer);
	writePage(lPagePointer, LSN, _pageSize);
	if(close() == -1){ return -1; }
	delete[] lPagePointer;
	return 0;
}


int PartitionBase::open()
{
	if(_openCount == 0)
	{
		_fileDescriptor = ::open(_partitionPath.c_str(), O_RDWR); //call open in global namespace
		if(_fileDescriptor == -1)
		{
            if(_controlBlock.trace())
                printErr("Opening the partition failed", errno);
			return -1;
		}
	}
	++_openCount;
	return 0;
}

int PartitionBase::close()
{
	if(_openCount == 1)
	{
		if(::close(_fileDescriptor) == -1) //call close in global namespace
		{
            if(_controlBlock.trace())
                printErr("Closing the partition failed", errno);
			return -1;
		}
		--_openCount;
		_fileDescriptor = -1;
	}
	else if(_openCount > 1)
	{
		--_openCount;
	}
	return 0;
}

int PartitionBase::allocPage()
{
	byte* lPagePointer = new byte[_pageSize];
	InterpreterFSIP fsip;
	fsip.attach(lPagePointer);
	uint lIndexOfFSIP = 0;
	int lAllocatedPageIndex;
	do
	{
		readPage(lPagePointer, lIndexOfFSIP, _pageSize);	//Read FSIP into buffer
		lAllocatedPageIndex = fsip.getNewPage(lPagePointer, LSN, _partitionID);	//Request free block from FSIP
		if(lAllocatedPageIndex == -1)
		{
			lIndexOfFSIP += (1 + getMaxPagesPerFSIP()); //Prepare next offset to FSIP
		} 
		else
		{
			writePage(lPagePointer, lIndexOfFSIP, _pageSize);
		} 
		if(lIndexOfFSIP >= _sizeInPages) return -1;						//Next offset is bigger than the partition
	}
	while(lAllocatedPageIndex == -1);	//if 'lAllocatedPageIndex != -1' a free block was found
	delete[] lPagePointer;
	return lAllocatedPageIndex;	//return offset to free block
}

int PartitionBase::freePage(const uint aPageIndex)
{
	byte* lPagePointer = new byte[_pageSize];
	if(readPage(lPagePointer, aPageIndex, _pageSize) == -1){ return -1; }
	InterpreterFSIP fsip;
	fsip.attach(lPagePointer);
	fsip.freePage(aPageIndex);
	fsip.detach();
	delete[] lPagePointer;
	return 0;
}

int PartitionBase::readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pread(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1 && _controlBlock.trace())
	{
        printErr("Reading the partition failed", errno);
		return -1;
	}
	return 0;
}

int PartitionBase::writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pwrite(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1 && _controlBlock.trace())
	{
        printErr("Write to partition failed", errno);
		return -1;
	}
	return 0;
}

int PartitionBase::assignSize(uint& aSize) //in number of pages 
{
	aSize = 0;

	if(isFile())
	{
		aSize =	fs::file_size(_partitionPath) / _pageSize;
	}
	else if(isRawDevice())
	{
		int lFileDescriptor = ::open(_partitionPath.c_str(), O_RDONLY);
   		if(lFileDescriptor != -1) 
   	 	{
    		uint64_t lSector_count = 0;
    		uint32_t lSector_size = 0;
       		if(ioctl(lFileDescriptor, P_NO_BLOCKS, &lSector_count) != -1 && ioctl(lFileDescriptor, P_BLOCK_SIZE, &lSector_size) != -1)
       	 	{
       		 	uint64_t lDisk_size = lSector_count * lSector_size; //in bytes
        		if(lDisk_size % _pageSize == 0)
        		{
        			aSize = lDisk_size / _pageSize;
        		} 
        		else
    			{
                    if(_controlBlock.trace()) printErr("Partition size modulo page size is not equal to zero");
		       		 return -1;
		   		 }
    	    }
       		else
        	{
                if(_controlBlock.trace()) printErr("ioctl call failed", errno);
        		return -1;
        	}
			::close(lFileDescriptor);
    	}
   		else
   		{
            if(_controlBlock.trace()) printErr("Opening the partition failed", errno);
       		return -1;
    	}
	}
	else 
	{
        if(_controlBlock.trace()) printErr("Partition type not supported");
		return -1;
	}
	return 0;
}

void PartitionBase::init()
{
	if(exists())
	{
		if(assignSize(_sizeInPages) == -1 && _controlBlock.trace())
		{
            printErr("Partition size could not be assigned!");
		}
	}
	else
	{
		std::cout << "Partition does not exist physically yet. You may call the create functionality provided by the partition object." << std::endl;
	}
}

uint PartitionBase::getMaxPagesPerFSIP()
{
	InterpreterFSIP fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}
