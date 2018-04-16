#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPartitionID, const uint aGrowthIndicator, const control_block_t& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
	_growthIndicator(aGrowthIndicator)
{
	 init();
}

PartitionFile::~PartitionFile()
{}

void PartitionFile::create()
{
	if(exists())
	{
        std::cerr << "Partition already exists and cannot be created" << std::endl;
        return;
    }

	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(_growthIndicator);
	system(lCommand.c_str());
    _sizeInPages = retrieveSizeInPages(); //may throw
    format(); //may throw
}

void PartitionFile::remove()
{
	if(exists())
	{
		if(isFile())
		{
			std::string lCommand = "rm " + _partitionPath;
			system(lCommand.c_str());
		}
		else
		{
            std::cerr << "No file partition at " + _partPath << std::endl;
            return;
		}
	}
	else
	{
        std::cerr << "No file exists at " + _partPath << std::endl;
        return;
	}
}

void PartitionFile::extend(const uint aNoPages)
{
  //todo 
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
    myfile.close();
    delete[] lPagePointer;
    close();
}

