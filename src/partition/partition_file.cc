#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string& aPath, const std::string& aName, const uint16_t aGrowthIndicator, const uint8_t aPartitionID, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
	_growthIndicator(aGrowthIndicator)
{
    std::string lMes = std::string("File partition instance created. _gI/agI ")+std::to_string(_growthIndicator)+std::to_string(aGrowthIndicator);
    create();
    TRACE(lMes);
}

PartitionFile::PartitionFile(const Partition_T& aTuple, const CB& aControlBlock):
	PartitionBase(aTuple.path(), aTuple.name(), aTuple.ID(), aControlBlock),
	_growthIndicator(aTuple.growth())
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
        /*
        load last fsip and attach fsip interpreter
        uint remainingPages = Interpreter.grow(_growthIndicator,getMaxPagesPerFSIP() )
        if(remainingPages>0){
            load next position where a fsip should be (which is something like lastFSIP + getMaxPagesPerFSIP +1)
            initNewFSIP with remainingPages
        }
        */
        InterpreterFSIP lFSIP;
        lFSIP.attach(ex.getBufferPtr());
        const size_t lPagesPerFSIP = getMaxPagesPerFSIP();
        const uint lRemainingPages = lFSIP.grow(_growthIndicator, lPagesPerFSIP);
        if(lRemainingPages > 0)
        {
            const uint lNextFSIP = ex.getIndexOfFSIP() + lPagesPerFSIP + 1;
            const uint lNumberOfPagesToManage = ((lRemainingPages > lPagesPerFSIP) ? lPagesPerFSIP : lRemainingPages);
            lFSIP.initNewFSIP(ex.getBufferPtr(), LSN, lNextFSIP, _partitionID, lNumberOfPagesToManage);
		    writePage(ex.getBufferPtr(), lNextFSIP, _pageSize);
        }
        delete[] ex.getBufferPtr();
        lPageIndex = PartitionBase::allocPage();
    }
    return lPageIndex;
}

size_t PartitionFile::partSize() noexcept
{
	if(isFile())
	{
        return fs::file_size(_partitionPath);
	}
    return 0;
}

size_t PartitionFile::partSizeInPages() noexcept
{
    return (partSize() / _pageSize);
}


void PartitionFile::create()
{   
	if(exists())
	{
        TRACE("Partition already exists and cannot be created");
        throw PartitionExistsException(FLF);
    }
    std::string lTraceMsg = std::string("Trying to create file partition at '") + _partitionPath + std::string("'");
    TRACE(lTraceMsg);
    std::ofstream ofs(_partitionPath, std::ofstream::out);
    ofs.close();
    if(exists())
    {
        const size_t lFileSize = _growthIndicator * _pageSize;
        fs::resize_file(_partitionPath, lFileSize); //will throw if fails
        _sizeInPages = partSizeInPages(); 
        lTraceMsg = "File partition (with " + std::to_string(_sizeInPages) + " pages) was successfully created in the file system";
        TRACE(lTraceMsg); 
        format(); //may throw
    }
    else
    {
        lTraceMsg = "Something went wrong while trying to create the file";
        TRACE(lTraceMsg); 
        throw PartitionException(FLF, lTraceMsg);
    }
}

void PartitionFile::remove()
{
    std::string lTraceMsg;
    if(!exists())
    {
        lTraceMsg = std::string("No file exists at '") + _partitionPath + std::string("'");
        TRACE(lTraceMsg);
        return;
    }
    lTraceMsg = std::string("Trying to remove file partition at '") + _partitionPath + std::string("'");
    TRACE(lTraceMsg);
    if(fs::remove(_partitionPath))
    {
        lTraceMsg = "File partition was successfully removed from the file system";
        TRACE(lTraceMsg); 
    }
    else
    {
        lTraceMsg = "Something went wrong while trying to remove the file from the file system";
        TRACE(lTraceMsg); 
        throw PartitionException(FLF, lTraceMsg);
    }
}

void PartitionFile::extend()
{
    const size_t lNewSize = (_sizeInPages + _growthIndicator) * _pageSize;
    fs::resize_file(_partitionPath, lNewSize); //will throw if fails
    _sizeInPages = lNewSize / _pageSize;
    //GROW FSIPS
    
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

