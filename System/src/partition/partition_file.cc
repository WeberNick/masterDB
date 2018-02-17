#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPageSize, const uint aPartitionID, const uint aGrowthIndicator) :
	PartitionBase(aPath, aName, aPageSize, aPartitionID),
	_growthIndicator(aGrowthIndicator)
{

}

PartitionFile::~PartitionFile()
{}

int PartitionFile::create(const uint aSizeInPages)
{
  //todo check if file exists etc...
	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(aSizeInPages);
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mA partition with " << (_pageSize * _sizeInPages) << " Bytes (" << _sizeInPages << " pages) was successfully created!\033[0m" << std::endl;
	if(format() != 0 )
	{
		std::cerr << "The partition could not be initialized and will be removed!" << std::endl;
		remove();
		return -1;
	}
	return 0;
}

int PartitionFile::remove()
{
  //do checks..
	std::string lCommand = "rm " + _partitionPath;
	std::cout << "\033[1;30mThe following command will be executed:\033[0m '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	std::cout << "\033[1;30mPartitionFile was successfully removed.\033[0m" << std::endl;
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

