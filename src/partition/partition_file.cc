#include "partition_file.hh"

PartitionFile::PartitionFile(const std::string& aPath, const std::string& aName, const uint16_t aGrowthIndicator, const uint8_t aPartitionID, const CB& aControlBlock) :
	PartitionBase(aPath, aName, aPartitionID, aControlBlock),
	_growthIndicator(aGrowthIndicator)
{
    create();
    TRACE("'PartitionFile' object constructed (For a new partition)");
}

PartitionFile::~PartitionFile()
{
    TRACE("'PartitionFile' object destructed");
}

PartitionFile::PartitionFile(const Partition_T& aTuple, const CB& aControlBlock):
	PartitionBase(aTuple.path(), aTuple.name(), aTuple.ID(), aControlBlock),
	_growthIndicator(aTuple.growth())
{
    if(exists()) _sizeInPages = partSizeInPages();
    else _sizeInPages = 0;
    assert(_growthIndicator >= 8);
    TRACE("'PartitionFile' object constructed (For a existing partition)");
}

uint32_t PartitionFile::allocPage()
{
    uint32_t lPageIndex = 0;
    try
    {
        lPageIndex = PartitionBase::allocPage();
       // printPage(0);
    }
    catch(const PartitionFullException& ex)
    {
        //Before the exception is thrown, the partition (file) will be clossed by the alloc call in partition base...
        //Open file again for recovering from the exception
        open();
        //extend
        TRACE("Extending the file partition. Grow by " + std::to_string(_growthIndicator) + " pages (currently " + std::to_string(_sizeInPages) + " pages)");
        const size_t lNewSize = (_sizeInPages + _growthIndicator) * _pageSize;
        FileUtil::resize(_partitionPath, lNewSize);
        _sizeInPages = lNewSize / _pageSize;
        TRACE("Extending the file partition was successful. New size is " + std::to_string(_sizeInPages) + " pages");
        //extend finished
        //grow fsip
        InterpreterFSIP lFSIP;
        lFSIP.attach(ex.getBufferPtr());
        const size_t lPagesPerFSIP = getMaxPagesPerFSIP();
        const uint lRemainingPages = lFSIP.grow(_growthIndicator, lPagesPerFSIP);
        writePage(ex.getBufferPtr(),ex.getIndexOfFSIP(),_pageSize);
        TRACE("remaining pages after growing: " + std::to_string(lRemainingPages));
        if(lRemainingPages > 0)
        {
            const uint lNextFSIP = ex.getIndexOfFSIP() + lPagesPerFSIP + 1;
            const uint lNumberOfPagesToManage = ((lRemainingPages > lPagesPerFSIP) ? lPagesPerFSIP : lRemainingPages);
            lFSIP.initNewFSIP(ex.getBufferPtr(), LSN, lNextFSIP, _partitionID, lNumberOfPagesToManage);
		    writePage(ex.getBufferPtr(), lNextFSIP, _pageSize);
        }
        delete[] ex.getBufferPtr();
        TRACE("FSIP's were successfully updated with the new partition size");
        lPageIndex = PartitionBase::allocPage();
    }
    return lPageIndex;
}

size_t PartitionFile::partSize() noexcept
{
    return FileUtil::isFile(_partitionPath) ? FileUtil::fileSize(_partitionPath) : 0;
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
    TRACE("Creating a file at '" + _partitionPath + "'");
    FileUtil::create(_partitionPath);
    if(exists())
    {
        TRACE("File created at '" + _partitionPath + "'");
        const size_t lFileSize = _growthIndicator * _pageSize;
        FileUtil::resize(_partitionPath, lFileSize);
        _sizeInPages = partSizeInPages(); 
        TRACE("File partition (with " + std::to_string(_sizeInPages) + " pages) was successfully created in the file system"); 
        format(); //may throw
    }
    else
    {
        const std::string lMsg = "Something went wrong while trying to create the file";
        TRACE(lMsg); 
        throw PartitionException(FLF, lMsg);
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
    if(FileUtil::remove(_partitionPath))
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

void PartitionFile::printPage(uint aPageIndex)
{
    TRACE("pagePrinted");
    open();
    byte *lPagePointer = new byte[_pageSize];
    readPage(lPagePointer, aPageIndex, _pageSize);
    std::ofstream myfile;
    std::string filename = "page_alloc" + std::to_string(aPageIndex) + ".txt";
    myfile.open(filename);
    uint32_t *lPP2 = (uint32_t *)lPagePointer;
    for (uint a = 0; a < _pageSize / 4; ++a) {
        myfile << std::hex << std::setw(8) << std::setfill('0') << *(lPP2 + a) << std::endl;
    }
    myfile.close();
    delete[] lPagePointer;
    close();
}

std::ostream& operator<< (std::ostream& stream, const PartitionFile& aPartition)
{
    stream << aPartition.to_string();
    return stream;
}
