#include "partition_file.hh"

PartitionFile::PartitionFile(const char* aPath, const uint aNoPages, const uint aPageSize, const uint aPartitionID, const uint aGrowthIndicator) :
	PartitionBase(aPath, aNoPages, aPageSize, aPartitionID),
	_growthIndicator(aGrowthIndicator),
	_fileDescriptor(-1)
{}

PartitionFile::~PartitionFile(){}

int PartitionFile::openPartition()
{
	if(_isOpen || !_isCreated) return -1;
	_fileDescriptor = open(_partitionPath, O_RDWR);
	if(_fileDescriptor == -1)
	{
		std::cerr << "Error opening the file: " << std::strerror(errno) << std::endl;
		return -1;
	}
	_isOpen = true;
	return 0;
}

int PartitionFile::closePartition()
{
	if(!_isOpen || !_isCreated) return -1;
	if(close(_fileDescriptor) != 0)
	{
		std::cerr << "Error closing the file: " << std::strerror(errno) << std::endl;
		return -1;
	}
	_fileDescriptor = -1;
	_isOpen = false;
	return 0;
}

int PartitionFile::createPartition()
{
	if(_isCreated) return -1;
	std::string lCommand = "dd if=/dev/zero of=" + std::string(_partitionPath) + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(_sizeInPages);
	std::cout << "\n" << std::endl;
	std::cout << "The following command will be executed: '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "A partition with " << (_pageSize * _sizeInPages) << " Bytes (" << _sizeInPages << " pages) was successfully created!" << std::endl;
	std::cout << "\n" << std::endl;
	_isCreated = true;
	if(openPartition() == -1) 
	{
				return -1;
	}
	if(init() != 0 )
	{
		// std::cerr << "The partition could not be initialized and will be removed!" << std::endl;
		// removePartition();
		std::cout << "init-1" << std::endl;
		return -1;
	}
	if(closePartition() == -1) return -1;
	return 0;
}

int PartitionFile::removePartition()
{
	if(!_isCreated) return -1;
	std::string lCommand = "rm " + std::string(_partitionPath);
	std::cout << "\n" << std::endl;
	std::cout << "The following command will be executed: '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "PartitionFile was successfully removed!" << std::endl;
	std::cout << "\n" << std::endl;
	_isCreated = false;
	return 0;
}

int PartitionFile::allocPage()
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
		lIndexOfFSIP += (1 + fsip.noManagedPages());							//Prepare next offset to FSIP
		if(lIndexOfFSIP >= _sizeInPages) return -1;						//Next offset is bigger than the partition
	}
	while(lAllocatedPageIndex == -1);	//if 'lAllocatedPageIndex != -1' a free block was found
	delete[] lPagePointer;
	return lAllocatedPageIndex;	//return offset to free block
}

int PartitionFile::freePage(const uint aPageIndex)
{
	byte* lPagePointer = new byte[_pageSize];
	if(readPage(lPagePointer, aPageIndex, _pageSize) == -1) return -1;
	FSIPInterpreter fsip;
	fsip.attach(lPagePointer);
	fsip.freePage(aPageIndex);
	fsip.detach();
	delete[] lPagePointer;
	return 0;
}

int PartitionFile::readPage(byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pread(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
		std::cerr << "Error reading the file: " << std::strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int PartitionFile::writePage(const byte* aBuffer, const uint aPageIndex, const uint aBufferSize)
{
	if(pwrite(_fileDescriptor, aBuffer, aBufferSize, (aPageIndex * _pageSize)) == -1)
	{
		std::cerr << "Error writing the file: " << std::strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int PartitionFile::init()
{
	byte* lPagePointer = new byte[_pageSize];
	uint lPagesPerFSIP = c_PagesPerFSIP();
	uint lCurrentPageNo = 0;
	FSIPInterpreter fsip;
	uint remainingPages = _sizeInPages;
	while(remainingPages > 1)
	{
		--remainingPages;
		fsip.initNewFSIP(lPagePointer, LSN, lCurrentPageNo, _partitionID, ((remainingPages > lPagesPerFSIP) ? lPagesPerFSIP : remainingPages));
		if(writePage(lPagePointer, lCurrentPageNo, _pageSize) == -1) {
			return -1;
		}
		lCurrentPageNo += (lPagesPerFSIP + 1);
		remainingPages -= (remainingPages > lPagesPerFSIP) ? lPagesPerFSIP : remainingPages;
	}
	delete[] lPagePointer;
	if(fsip.reservePage(_segmentIndexPage) == -1) return -1;
	return 0;
}



