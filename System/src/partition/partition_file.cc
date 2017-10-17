#include "partition_file.hh"

PartitionFile::PartitionFile(const char* aPath, const uint aNoPages, const uint aPageSize, const uint aPartitionID, const uint aGrowthIndicator) :
	PartitionBase(aPath, aNoPages, aPageSize, aPartitionID),
	_growthIndicator(aGrowthIndicator),
	_fileDescriptor(-1)
{}

PartitionFile::~PartitionFile(){}

int PartitionFile::open()
{
	if(!(!_isOpen && _isCreated)){ return -1; }
	_fileDescriptor = ::open(_partitionPath, O_RDWR); //call open in global namespace
	if(_fileDescriptor == -1)
	{
		std::cerr << "Error opening the file: " << std::strerror(errno) << std::endl;
		return -1;
	}
	_isOpen = true;
	return 0;
}

int PartitionFile::close()
{
	if(!(_isOpen && _isCreated)){ return -1; }
	if(::close(_fileDescriptor) != 0) //call close in global namespace
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
	if(_isCreated){return -1;}
	std::string lCommand = "dd if=/dev/zero of=" + std::string(_partitionPath) + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(_sizeInPages);
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mA partition with " << (_pageSize * _sizeInPages) << " Bytes (" << _sizeInPages << " pages) was successfully created!\033[0m" << std::endl;
	_isCreated = true;
	if(init() != 0 )
	{
		std::cerr << "The partition could not be initialized and will be removed!" << std::endl;
		removePartition();
		return -1;
	}
	return 0;
}

int PartitionFile::removePartition()
{
	if(!_isCreated){return -1;}
	std::string lCommand = "rm " + std::string(_partitionPath);
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mPartitionFile was successfully removed.\033[0m" << std::endl;
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

int PartitionFile::freePage(const uint aPageIndex)
{
	byte* lPagePointer = new byte[_pageSize];
	if(readPage(lPagePointer, aPageIndex, _pageSize) == -1){return -1;}
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

uint PartitionFile::getMaxPagesPerFSIP()
{
	FSIPInterpreter fsip;
	return (_pageSize - fsip.getHeaderSize()) * 8;
}

int PartitionFile::init()
{
	byte* lPagePointer = new byte[_pageSize];
	uint lPagesPerFSIP = getMaxPagesPerFSIP();
	uint lCurrentPageNo = 0;
	FSIPInterpreter fsip;
	uint remainingPages = _sizeInPages;
	uint lNumberOfPagesToManage;
	if(open() == -1){return -1;}
	while(remainingPages > 1)
	{
		--remainingPages;
		lNumberOfPagesToManage = ((remainingPages > lPagesPerFSIP) ? lPagesPerFSIP : remainingPages);
		fsip.initNewFSIP(lPagePointer, LSN, lCurrentPageNo, _partitionID, lNumberOfPagesToManage);
		if(writePage(lPagePointer, lCurrentPageNo, _pageSize) == -1){return -1;}
		lCurrentPageNo += (lPagesPerFSIP + 1);
		remainingPages -= lNumberOfPagesToManage;
	}
	fsip.detach();
	readPage(lPagePointer, 0, _pageSize);
	fsip.attach(lPagePointer);
	if(fsip.reservePage(_segmentIndexPage) == -1)
	{
		delete[] lPagePointer;
		return -1;
	}
	writePage(lPagePointer, 0, _pageSize);
	if(close() == -1){return -1;}
	delete[] lPagePointer;
	return 0;
}

void PartitionFile::printPage(uint aPageIndex)
{
  	open();
	 byte* lPagePointer = new byte[_pageSize];
	 readPage(lPagePointer, aPageIndex, _pageSize);	
	 std::ofstream myfile;
	 std::string filename = "page" + std::to_string(aPageIndex) + ".txt";
	 myfile.open (filename);
	 uint32_t* lPP2 = (uint32_t*) lPagePointer;
	 for(uint a = 0; a < _pageSize/4 ; ++a)
	 {
	     myfile <<  std::hex << std::setw(8) << std::setfill('0')<< *(lPP2+a) << std::endl;
	 }
	 std::cout << "pagePrinted" <<std::endl;
	 myfile.close();
	 delete[] lPagePointer;
	 close();
}

