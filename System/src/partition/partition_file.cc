#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPageSize, const uint aPartitionID, const uint aGrowthIndicator) :
	PartitionBase(aPath, aName, aPageSize, aPartitionID),
	_growthIndicator(aGrowthIndicator)
{
	 init();
}

PartitionFile::~PartitionFile()
{}

int PartitionFile::create(const uint aSizeInPages)
{
	if(exists())
	{
		std::cerr << "## CREATE PARTITION: The partition already exists!" << std::endl;
		return -1;
	}
	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(aSizeInPages);
	std::cout << "## CREATE PARTITION: The following command will be executed: '" << lCommand << "'" << std::endl;
	system(lCommand.c_str());
	if(assignSize(_sizeInPages) == -1)
	{
		std::cerr << "## CREATE PARTITION: # ERROR: Partition size could not be assigned!" << std::endl;
	}
	if(format() != 0 )
	{
		std::cerr << "## CREATE PARTITION: The partition could not be initialized and will be removed!" << std::endl;
		remove();
		return -1;
	}
	return 0;
}

int PartitionFile::remove()
{
	if(exists())
	{
		if(isFile())
		{
			std::string lCommand = "rm " + _partitionPath;
			std::cout << "## REMOVE PARTITION: The following command will be executed: '" << lCommand << "'" << std::endl;
			system(lCommand.c_str());
			std::cout << "## REMOVE PARTITION: PartitionFile was successfully removed." << std::endl;
			return 0;
		}
		else
		{
			std::cerr << "## REMOVE PARTITION: # ERROR: The file at " << _partitionPath << " is no file partition." << std::endl;
			return -1;
		}
	}
	else
	{
		std::cerr << "## REMOVE PARTITION: # ERROR: No file exists at " << _partitionPath << std::endl;
		return -1;
	}
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

