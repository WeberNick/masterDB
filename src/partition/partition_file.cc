#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string aPath, const std::string aName, const uint aGrowthIndicator, const uint aPartitionID, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
	_growthIndicator(aGrowthIndicator)
{
    std::string lMes = std::string("File partition instance created. _gI/agI ")+std::to_string(_growthIndicator)+std::to_string(aGrowthIndicator);

    create();
    
    TRACE(lMes);

}
PartitionFile::PartitionFile(const part_t& aTuple, const CB& aControlBlock):
	PartitionBase(aTuple._pPath, aTuple._pName, aTuple._pID, aControlBlock),
	_growthIndicator(aTuple._pGrowth)
{
    if(exists()) _sizeInPages = partSizeInPages();
    else _sizeInPages = 0;
}

uint32_t PartitionFile::allocPage()
{
    uint32_t lPageIndex = 0;
    try
    {
        lPageIndex = PartitionBase::allocPage();
    }
    catch(const PartitionFullException& ex)
    {
        try
        {
            extend();
            //extend was successfull
        }
        catch(const fs::filesystem_error& fse)
        {
            const std::string lTraceMsg = std::string("An Error occured while trying to extend the file size: ") + std::string(fse.what());
            TRACE(lTraceMsg);
            throw ex;
        }
            //need to reformat the new pages
        lPageIndex = PartitionBase::allocPage();
    }
    return lPageIndex;
}

size_t PartitionFile::partSize()
{
	if(isFile())
	{
        return fs::file_size(_partitionPath);
	}
    return 0;
}

size_t PartitionFile::partSizeInPages()
{
    return (partSize() / _pageSize);
}


void PartitionFile::create()
{
	if(exists())
	{
        const std::string lTraceMsg("Partition already exists and cannot be created");
        TRACE(lTraceMsg);
        return;
    }

	std::string lCommand = "dd if=/dev/zero of=" + _partitionPath + " bs=" + std::to_string(_pageSize) + " count=" + std::to_string(_growthIndicator);
    if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, std::string("Execute: ") + lCommand); }
	system(lCommand.c_str());
    _sizeInPages = partSizeInPages(); 
    const std::string lTraceMsg("File partition was created");
    TRACE(lTraceMsg); 
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
            const std::string lTraceMsg("No file partition at " + _partitionPath);
            TRACE(lTraceMsg);
            return;
		}
	}
	else
	{
        const std::string lTraceMsg("No file exists at " + _partitionPath);
        TRACE(lTraceMsg);
        return;
	}
}

void PartitionFile::extend()
{
    const size_t lNewSize = (_sizeInPages + _growthIndicator) * _pageSize;
    fs::resize_file(_partitionPath, lNewSize); //will throw if fails
    _sizeInPages = lNewSize / _pageSize;
    //FSIP.grow fehlt noch. Idee: gehe auf letzte FSIP, schau wie viele unvalid bits dort sind. Setze anzahl als valid, wenn nötig, initialisiere neue FSIPs.
    TRACE("Size of file partition was extended");
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

