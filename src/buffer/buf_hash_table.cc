#include "buf_hash_table.hh"

/** TODO
 * @brief Construct a new BufferHashTable::BufferHashTable object
 * 
 * @param aHashTableSize 
 */
BufferHashTable::BufferHashTable(const size_t aHashTableSize) :
	_size(aHashTableSize),
	_hashTable(std::make_unique<HashBucket[]>(_size))
{ 
    TRACE("'BufferHashTable' constructed");
}

BufferHashTable::~BufferHashTable()
{
    TRACE("'BufferHashTable' destructed");
}

size_t BufferHashTable::hash(const PID& aPageID) noexcept
{
    // std::hash<uint> lHash;
    // return (lHash(aPageID.fileID()) + lHash(aPageID.pageNo())) % _size;
    return (aPageID.fileID() + aPageID.pageNo()) % _size;
}

std::vector<BCB*> BufferHashTable::getAllValidBCBs(){
	std::vector<BCB*> rtn;
	BCB* lBCB = nullptr;
	for(size_t i = 0; i < _size; ++i)
	{
		lBCB = _hashTable[i].getBCB();
		while(lBCB)
		{
			rtn.push_back(lBCB);
			lBCB = lBCB->getNextInChain();
		}
	}
	return rtn;
}


