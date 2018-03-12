#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPartitionID, const uint aGrowthIndicator, const control_block_t& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
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
        if(_controlBlock.trace()) printErr("Partition already exists and cannot be created");
		return -1;
	}
	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(aSizeInPages);
	system(lCommand.c_str());
	if(assignSize(_sizeInPages) == -1 && _controlBlock.trace())
	{
        printErr("Partition size could not be assigned!");
	}
	if(format() != 0 )
	{
        if(_controlBlock.trace()) printErr("Partition could not be initialized and will be removed");
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
			system(lCommand.c_str());
			return 0;
		}
		else
		{
            if(_controlBlock.trace()) printErr("No file partition at path " + _partitionPath);
			return -1;
		}
	}
	else
	{
        if(_controlBlock.trace()) printErr("No file exists at " + _partitionPath);
		return -1;
	}
}

int PartitionFile::extend(const uint aNoPages)
{
  

  return -1;
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

