#include "partition.hh"

uint Partition::s_IDCounter = 0;
long Partition::s_LSNCounter = 0;

Partition::Partition(const char* aPath, const uint64_t aPartitionSize, const uint aBlockSize, const uint aGrowthIndicator, const mode_t aAccessRights) :
	_partitionPath(aPath),
	_partitionSize(aPartitionSize),
	_blockSize(aBlockSize),
	_growthIndicator(aGrowthIndicator),
	_accessRights(aAccessRights),
	_isCreated(false),
	_isRemoved(false),
	_partitionID(s_IDCounter++)
{}

Partition::~Partition()
{
	if(!_isRemoved)
	{
		removePartition();
	}
}

const int Partition::createPartition()
{
	if(_isCreated)
	{
		std::cerr << "The partition is already created!" << std::endl;
		return -1;
	}
	int lFileDescriptor = open(_partitionPath, O_WRONLY | O_CREAT | O_EXCL, _accessRights);
	if(lFileDescriptor == -1)
	{
		std::cerr << "An error occured while creating the file: '" << _partitionPath << "'" << std::endl;
		return -1;
	}
	lseek (lFileDescriptor, _partitionSize - 1, SEEK_SET);
	int zero = 0;
	write (lFileDescriptor, &zero, 1);
	close(lFileDescriptor);
	_isCreated = true;
	std::cout << "A partition with " << _partitionSize << " Bytes was successfully created!" << std::endl;
	if(init() != 0 )
	{
		std::cerr << "The partition could not be initialized and will be removed!" << std::endl;
		removePartition();
		return -1;
	}
	return 0;
}

const int Partition::removePartition()
{
	if(_isRemoved)
	{
		std::cerr << "The partition was already removed!" << std::endl;
		return -1;
	}
	std::cout << "Press any key to remove the partition!" << std::endl;
	system("read");
	std::string command = std::string("rm ") + std::string(_partitionPath);
	std::cout << "Executing '" << command << "'" << std::endl;
	system(command.c_str());
	_isRemoved = true;
	std::cout << "Partition was removed!" << std::endl;
	return 0;
}

const int Partition::allocBlock()
{
	byte* lPagePointer = new byte[_blockSize];
	const uint lNumberOfTotalBlocks = totalBlocks();
	FSIPInterpreter fsip;
	uint lOffsetFSIP = 0;
	int lAllocatedOffset;
	do
	{
		readBlock(lPagePointer, lOffsetFSIP);				//Read FSIP into buffer
		lAllocatedOffset = fsip.getNewBlock(lPagePointer);	//Request free block from FSIP
		lOffsetFSIP += (1 + fsip.noManagedBlocks());		//Prepare next offset to FSIP
		if(lOffsetFSIP >= lNumberOfTotalBlocks)				//Next offset is bigger than the partition
		{
			std::cerr << "No free block available in the partition!" << std::endl;
			return -1;
		}
	}
	while(lAllocatedOffset == -1);	//if 'lAllocatedOffset != -1' a free block was found
	delete[] lPagePointer;
	return lAllocatedOffset;	//return offset to free block
}

const int Partition::freeBlock(const uint aBlockNo)
{
	byte* lPagePointer = new byte[_blockSize];
	if(readBlock(lPagePointer, aBlockNo) == -1)
	{
		std::cerr << "An error occured while trying to free the block: " << aBlockNo << std::endl;
		return -1;
	}
	FSIPInterpreter fsip;
	fsip.attach(lPagePointer);
	fsip.freeBlock(aBlockNo);
	fsip.detach();
	return 0;
}

const int Partition::readBlock(byte* aBuffer, const uint aBlockNo, const uint aBufferSize)
{
	int lFileDescriptor = open(_partitionPath, O_RDONLY);
	if(lFileDescriptor == -1)
	{
		std::cerr << "An error occured while opening the file: '" << _partitionPath << "'" << std::endl;
		return -1;
	}
	pread(lFileDescriptor, aBuffer, aBufferSize, (aBlockNo * _blockSize));
	if(close(lFileDescriptor) == -1)
	{
		std::cerr << "An error occured while closing the file!" << std::endl;
		return -1;
	}
	return 0;
}

const int Partition::writeBlock(const byte* aBuffer, const uint aBlockNo, const uint aBufferSize)
{
	int lFileDescriptor = open(_partitionPath, O_WRONLY);
	if(lFileDescriptor == -1)
	{
		std::cerr << "An error occured while opening the file: '" << _partitionPath << "'" << std::endl;
		return -1;
	}
	pwrite(lFileDescriptor, aBuffer, aBufferSize, (aBlockNo * _blockSize));
	if(close(lFileDescriptor) == -1)
	{
		std::cerr << "An error occured while closing the file!" << std::endl;
		return -1;
	}
	return 0;
}

const uint Partition::totalBlocks()
{
	return std::ceil(_partitionSize / (double) _blockSize);
}

const int Partition::init()
{
	byte* lPagePointer = new byte[_blockSize];	//buffer to write the initial FSIP into
	uint lBlocksPerFSIP = 32576;	//todo: auslagern, berechnung in fsip?
	uint lCurrentBlockNo = 0;
	FSIPInterpreter fsip;
	uint remainingBlocks = totalBlocks();
	while(remainingBlocks > 1)
	{
		--remainingBlocks;
		fsip.initNewFSIP(lPagePointer, s_LSNCounter++, lCurrentBlockNo, _partitionID, remainingBlocks);
		if(writeBlock(lPagePointer, lCurrentBlockNo) == -1)
		{
			return -1;
		}
		lCurrentBlockNo += (lBlocksPerFSIP + 1);
		remainingBlocks -= (remainingBlocks > lBlocksPerFSIP) ? lBlocksPerFSIP : remainingBlocks;
	}
	delete[] lPagePointer;
	return 0;
}