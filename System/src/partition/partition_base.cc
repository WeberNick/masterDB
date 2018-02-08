#include "partition_base.hh"

PartitionBase::PartitionBase(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) : 
	_partitionPath(aPath),
	_partitionName(aName),
	_sizeInPages(0),
	_pageSize(aPageSize),
	_partitionID(aPartitionID),
	_segmentIndexPage(aSegmentIndexPage),
	_isCreated(false),
	_openCount(0),
	_fileDescriptor(-1)
{


}

PartitionBase::~PartitionBase(){}


int PartitionBase::open()
{
	if(!_isCreated){ return -1; }
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
	if(!_isCreated){ return -1; }
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

uint PartitionBase::getMaxPagesPerFSIP()
{
	FSIPInterpreter fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}