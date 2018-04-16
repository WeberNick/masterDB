#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string aPath, const std::string aName, const uint aPartitionID, const CB& aControlBlock) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_pageSize(aControlBlock.pageSize()),
	_sizeInPages(0),
	_partitionID(aPartitionID),
	_openCount(0),
	_fileDescriptor(-1),
	_cb(aControlBlock)
{
    InterpreterFSIP::setPageSize(aControlBlock.pageSize());
}

PartitionBase::~PartitionBase(){}


void PartitionBase::format()
{
	byte* lPagePointer = new byte[_pageSize];
	uint lPagesPerFSIP = getMaxPagesPerFSIP();
	uint lCurrentPageNo = 0;
	InterpreterFSIP fsip;
	uint remainingPages = _sizeInPages;
	uint lNumberOfPagesToManage;
    open();
	while(remainingPages > 1)
	{
		--remainingPages;
		lNumberOfPagesToManage = ((remainingPages > lPagesPerFSIP) ? lPagesPerFSIP : remainingPages);
		fsip.initNewFSIP(lPagePointer, LSN, lCurrentPageNo, _partitionID, lNumberOfPagesToManage);
		writePage(lPagePointer, lCurrentPageNo, _pageSize);
		lCurrentPageNo += (lPagesPerFSIP + 1);
		remainingPages -= lNumberOfPagesToManage;
	}
	fsip.detach();
	readPage(lPagePointer, LSN, _pageSize);
	fsip.attach(lPagePointer);
	writePage(lPagePointer, LSN, _pageSize);
	close();
	delete[] lPagePointer;
}


void PartitionBase::open()
{
	if(_openCount == 0)
	{
		_fileDescriptor = ::open(_partitionPath.c_str(), O_RDWR); //call open in global namespace
		if(_fileDescriptor == -1)
		{
            const std::string lErrMsg = std::string("An error occured while opening the file: '") + std::string(std::strerror(errno));
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
		}
	}
	++_openCount;
}

void PartitionBase::close()
{
	if(_openCount == 1)
	{
		if(::close(_fileDescriptor) == -1) //call close in global namespace
		{
            const std::string lErrMsg = std::string("An error occured while closing the file: '") + std::string(std::strerror(errno));
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
		}
		--_openCount;
		_fileDescriptor = -1;
	}
	else if(_openCount > 1)
	{
		--_openCount;
	}
}

uint PartitionBase::allocPage()
{
	/*
	byte* lPagePointer;
	BufferManager lBufMan;
	BCB* lBCB;
	pid_t lPID;
	InterpreterFSIP fsip;
	uint lIndexOfFSIP = 0;
	int lAllocatedPageIndex;
	do
	{ //does lock exclusive although not always needed.
		lPID = {getID(),lIndexOfFSIP};
		lBCB = lBufMan.fix(lPID);
		lPagePointer = lBufMan.getFramePrt(lBCB);
	//	readPage(lPagePointer, lIndexOfFSIP, _pageSize);	//Read FSIP into buffer
		lAllocatedPageIndex = fsip.getNewPage(lPagePointer, LSN, _partitionID);	//Request free block from FSIP
		if(lAllocatedPageIndex == -1)
		{
			lIndexOfFSIP += (1 + getMaxPagesPerFSIP()); //Prepare next offset to FSIP
		} 
		else
		{
			lBCB->setModified(true);
			//writePage(lPagePointer, lIndexOfFSIP, _pageSize);
		} 
		lBCB->getMtx().unlock();
		lBufMan.unfix(lBCB);
		if(lIndexOfFSIP >= _sizeInPages) return -1;						//Next offset is bigger than the partition
	}
	while(lAllocatedPageIndex == -1);	//if 'lAllocatedPageIndex != -1' a free block was found

	return lAllocatedPageIndex;	//return offset to free block
*/
    return allocPageForce();
}

uint PartitionBase::allocPageForce()
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
		if(lIndexOfFSIP >= _sizeInPages) //Next offset is bigger than the partition
        {
            const std::string lErrMsg("Error Message"); //change to appropriate msg
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw BaseException(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); //achnge to approp exc
        }
	}
	while(lAllocatedPageIndex == -1);	//if 'lAllocatedPageIndex != -1' a free block was found
	delete[] lPagePointer;
	return lAllocatedPageIndex;	//return offset to free block
}

void PartitionBase::freePage(const uint aPageIndex)
{
	byte* lPagePointer = new byte[_pageSize];
	readPage(lPagePointer, aPageIndex, _pageSize);
	InterpreterFSIP fsip;
	fsip.attach(lPagePointer);
	fsip.freePage(aPageIndex);
	fsip.detach();
	delete[] lPagePointer;
}

void PartitionBase::readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pread(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
        const std::string lErrMsg = std::string("An error occured while reading the file: '") + std::string(std::strerror(errno));
        if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
        throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
	}
}

void PartitionBase::writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pwrite(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1 && _cb.trace())
	{
        const std::string lErrMsg = std::string("An error occured while writing the file: '") + std::string(std::strerror(errno));
        if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
        throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
	}
}

uint PartitionBase::retrieveSizeInPages() //in number of pages 
{
    uint lSize = 0;
	if(isFile())
	{
		lSize = fs::file_size(_partitionPath) / _pageSize;
	}
	else if(isRawDevice())
	{
		int lFileDescriptor = ::open(_partitionPath.c_str(), O_RDONLY);
   		if(lFileDescriptor == -1) 
   	 	{
            const std::string lErrMsg = std::string("An error occured while opening the file: ") + std::string(std::strerror(errno));
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
        }
    	uint64_t lSector_count = 0;
    	uint32_t lSector_size = 0;
       	if(ioctl(lFileDescriptor, P_NO_BLOCKS, &lSector_count) == -1 || ioctl(lFileDescriptor, P_BLOCK_SIZE, &lSector_size) == -1)
       	{
            const std::string lErrMsg = std::string("An error occurred while using ioctl: ") + std::string(std::strerror(errno));
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
    	}
       	uint64_t lDisk_size = lSector_count * lSector_size; //in bytes
        if(lDisk_size % _pageSize != 0)
        {
            const std::string lErrMsg = std::string("Partition size modulo page size is not equal to zero");
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
        } 
        lSize = lDisk_size / _pageSize;
        if(::close(lFileDescriptor) == -1) //call close in global namespace
		{
            const std::string lErrMsg = std::string("An error occured while closing the file: ") + std::string(std::strerror(errno));
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
		}
    }
	else 
	{
        const std::string lErrMsg = std::string("Partition type is not supported");
        if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
        throw FileException(__FILE__, __LINE__, __PRETTY_FUNCTION__, _partitionPath.c_str(), lErrMsg);
	}
    return lSize;
}

void PartitionBase::init()
{
	if(exists())
	{
        try{ _sizeInPages = retrieveSizeInPages(); }
        catch(const FileException& ex){ std::cerr << ex.what() << std::endl; }
	}
	else
	{
		std::cerr << "Partition does not exist physically yet. You may call the create functionality provided by the partition object." << std::endl;
	}
}

uint PartitionBase::getMaxPagesPerFSIP()
{
	InterpreterFSIP fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}
