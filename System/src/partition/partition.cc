#include "partition.hh"

uint Partition::s_IDCounter = 0;

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
	init();
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

	return 0;
}

const int Partition::freeBlock()
{

	return 0;
}

const int Partition::readBlock()
{

	return 0;
}

const int Partition::writeBlock()
{

	return 0;
}

void Partition::init()
{
	
}

