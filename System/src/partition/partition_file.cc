#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID, const uint aGrowthIndicator) :
	PartitionBase(aPath, aName, aPageSize, aSegmentIndexPage, aPartitionID),
	_growthIndicator(aGrowthIndicator)
{

}

PartitionFile::~PartitionFile()
{}

int PartitionFile::create()
{
	if(_isCreated){ return -1; }

	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(_sizeInPages);
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mA partition with " << (_pageSize * _sizeInPages) << " Bytes (" << _sizeInPages << " pages) was successfully created!\033[0m" << std::endl;
	_isCreated = true;
	if(init() != 0 )
	{
		std::cerr << "The partition could not be initialized and will be removed!" << std::endl;
		remove();
		return -1;
	}
	return 0;
}

int PartitionFile::format()
{
	return 0;
}

int PartitionFile::remove()
{
	if(!_isCreated){ return -1; }
	std::string lCommand = "rm " + _partitionPath;
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mPartitionFile was successfully removed.\033[0m" << std::endl;
	_isCreated = false;
	return 0;
}

int PartitionFile::init()
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

void PartitionFile::printPage(uint aPageIndex)
{
    open();
    byte *lPagePointer = new byte[_pageSize];
    readPage(lPagePointer, aPageIndex, _pageSize);
    std::ofstream myfile;
    std::string filename = "page" + std::to_string(aPageIndex) + ".txt";
    myfile.open(filename);
    uint32_t *lPP2 = (uint32_t *)lPagePointer;
    for (uint a = 0; a < _pageSize / 4; ++a) {
        myfile << std::hex << std::setw(8) << std::setfill('0') << *(lPP2 + a) << std::endl;
    }
    std::cout << "pagePrinted" << std::endl;
    myfile.close();
    delete[] lPagePointer;
    close();
}

// std::filesystem::resize_file(p, 1024*1024*1024); // resize to 1 G


