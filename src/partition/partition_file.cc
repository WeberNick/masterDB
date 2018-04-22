#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aPartitionID, const uint aGrowthIndicator, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
	_growthIndicator(aGrowthIndicator)
{
	 init();
}
PartitionFile::PartitionFile(part_t aTuple, const CB& aControlBlock):
	PartitionBase(aTuple._pPath, aTuple._pName, aTuple._pID, aControlBlock),
	_growthIndicator(aTuple._pGrowth)
{
    _sizeInPages = retrieveSizeInPages();
}

PartitionFile::~PartitionFile()
{}

void PartitionFile::create()
{
	if(exists())
	{
        const std::string lErrMsg("Partition already exists and cannot be created");
        if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
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
            const std::string lErrMsg("No file partition at " + _partitionPath);
            if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
            return;
		}
	}
	else
	{
        const std::string lErrMsg("No file exists at " + _partitionPath);
        if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
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

