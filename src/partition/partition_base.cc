#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string& aPath, const std::string& aName, const uint8_t aPartitionID, const CB& aControlBlock) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_pageSize(aControlBlock.pageSize()),
	_sizeInPages(0),
	_partitionID(aPartitionID),
	_openCount(0),
	_fileDescriptor(-1),
	_cb(aControlBlock)
{
    InterpreterFSIP::init(aControlBlock);
}

void PartitionBase::open()
{
	if(_openCount == 0)
	{
		_fileDescriptor = ::open(_partitionPath.c_str(), O_RDWR); //call open in global namespace
		if(_fileDescriptor == -1)
		{
            const std::string lErrMsg = std::string("An error occured while opening the file: '") + std::string(std::strerror(errno));
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
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
            TRACE(lErrMsg);
            throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
		}
		--_openCount;
		_fileDescriptor = -1;
	}
	else if(_openCount > 1)
	{
		--_openCount;
	}
}

uint32_t PartitionBase::allocPage()
{

	byte* lPagePointer = new byte[_pageSize];
	InterpreterFSIP fsip;
	fsip.attach(lPagePointer);
	uint lIndexOfFSIP = 0;
	uint32_t lAllocatedPageIndex;
	do
	{
		readPage(lPagePointer, lIndexOfFSIP, _pageSize);	//Read FSIP into buffer
        try
        {
		    lAllocatedPageIndex = fsip.getNewPage(lPagePointer, LSN, _partitionID);	//Request free block from FSIP
        }
        catch(const FSIPException& ex)
        {
			lIndexOfFSIP += (1 + getMaxPagesPerFSIP()); //Prepare next offset to FSIP
		    if(lIndexOfFSIP >= _sizeInPages) //Next offset is bigger than the partition
            {
                const std::string lErrMsg("The partition is full. Can not allocate any new pages.");
                TRACE(lErrMsg);
				close();
                throw PartitionFullException(FLF, lPagePointer, lIndexOfFSIP); 
            }
            continue;
        }
		writePage(lPagePointer, lIndexOfFSIP, _pageSize);
        break;
        //continue will jump here
	}
	while(true); //if a free page is found, break will be executed. If not, an exception is thrown
	delete[] lPagePointer;
	TRACE(std::string("Page ")+std::to_string(lAllocatedPageIndex)+std::string(" of Partition ")+std::to_string(getID())+std::string(" allocated."));
	return lAllocatedPageIndex;	//return offset to free block
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
}

void PartitionBase::freePage(const uint32_t aPageIndex)
{
	byte* lPagePointer = new byte[_pageSize];
	uint32_t fsipIndex = (aPageIndex / (getMaxPagesPerFSIP()+1))*getMaxPagesPerFSIP();
	TRACE(std::to_string(fsipIndex));
	readPage(lPagePointer, fsipIndex , _pageSize);//fsip auf der aPageIndex verwaltet wird
	InterpreterFSIP fsip;
	fsip.attach(lPagePointer);
	TRACE(std::to_string(aPageIndex));
	//TRACE(std::to_string((aPageIndex % (getMaxPagesPerFSIP() +1)) -1));
	fsip.freePage(aPageIndex);
	fsip.detach();
	delete[] lPagePointer;
}

void PartitionBase::readPage(byte* aBuffer, const uint32_t aPageIndex, const uint aBufferSize)
{
	if(pread(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
        const std::string lErrMsg = std::string("An error occured while reading the file: '") + std::string(std::strerror(errno));
        TRACE(lErrMsg);
        throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
	}
}

void PartitionBase::writePage(const byte* aBuffer, const uint32_t aPageIndex, const uint aBufferSize)
{
	if(pwrite(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1 && _cb.trace())
	{
        const std::string lErrMsg = std::string("An error occured while writing the file: '") + std::string(std::strerror(errno));
        TRACE(lErrMsg);
        throw FileException(FLF, _partitionPath.c_str(), lErrMsg);
	}
}

void PartitionBase::format()
{
	byte* lPagePointer = new byte[_pageSize];
	const uint lPagesPerFSIP = getMaxPagesPerFSIP();
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


uint PartitionBase::getMaxPagesPerFSIP() noexcept
{
	InterpreterFSIP fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}
