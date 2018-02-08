#include "partition_raw.hh"

PartitionRaw::PartitionRaw(const std::string aPath, const std::string aName, const uint aPageSize, const uint aSegmentIndexPage, const uint aPartitionID) :
	PartitionBase(aPath, aName, aPageSize, aSegmentIndexPage, aPartitionID)
{

}

PartitionRaw::~PartitionRaw(){}


int PartitionRaw::create()
{
	if(_isCreated){ return -1; }

	if(std::filesystem::exists(_partitionPath) && std::filesystem::is_block_file(_partitionPath))
	{
		_sizeInPages = std::filesystem::file_size(_partitionPath) / _pageSize;
		_isCreated = true;
	}
	return 0;
}

int PartitionRaw::remove()
{
	return 0;
}

int PartitionRaw::init()
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
