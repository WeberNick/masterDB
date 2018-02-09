#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_pageSize(aPageSize),
	_sizeInPages(size()),
	_partitionID(aPartitionID),
	_segmentIndexPage(aSegmentIndexPage),
	_openCount(0),
	_fileDescriptor(-1)
{
}

PartitionBase::~PartitionBase(){}


int PartitionBase::format()
{
	byte* lPagePointer = new byte[_pageSize];
	uint lPagesPerFSIP = getMaxPagesPerFSIP();
	uint lCurrentPageNo = 0;
	FSIPInterpreter fsip;
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
	if(fsip.reservePage(_segmentIndexPage) == -1)
	{
		delete[] lPagePointer;
		return -1;
	}
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
			std::cerr << "Error opening the partition: " << std::strerror(errno) << std::endl;
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
		if(::close(_fileDescriptor) != 0) //call close in global namespace
		{
			std::cerr << "Error closing the partition: " << std::strerror(errno) << std::endl;
			return -1;
		}
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
	FSIPInterpreter fsip;
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
	FSIPInterpreter fsip;
	fsip.attach(lPagePointer);
	fsip.freePage(aPageIndex);
	fsip.detach();
	delete[] lPagePointer;
	return 0;
}

int PartitionBase::readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pread(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
		std::cerr << "Error reading the partition: " << std::strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int PartitionBase::writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pwrite(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
		std::cerr << "Error writing the partition: " << std::strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

uint PartitionBase::size() //in number of pages 
{
	struct stat lFileStats;
	if(stat(_partitionPath.c_str(), &lFileStats) == -1){ return 0; }
	return lFileStats.st_size / _pageSize;
}

uint PartitionBase::getMaxPagesPerFSIP()
{
	FSIPInterpreter fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}
