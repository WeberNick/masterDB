#include "buf_hash_table.hh"

BufferHashTable::BufferHashTable(const size_t aHashTableSize) :
	_size(aHashTableSize),
	_hashTable(nullptr)
{ 
    _hashTable = new HashBucket[_size]; 
    TRACE("'BufferHashTable' constructed");
}

BufferHashTable::~BufferHashTable()
{
	//BCB* lBCB = nullptr;
	//for(size_t i = 0; i < _size; ++i)
	//{
		//lBCB = _hashTable[i].getBCB();
		//while(_hashTable[i].getBCB())
		//{
			//lBCB = _hashTable[i].getBCB();
			//_hashTable[i].setBCB(lBCB->_nextInChain);
			//delete lBCB;
		//}
	//}
	delete[] _hashTable;
    TRACE("'BufferHashTable' destructed");
}


size_t BufferHashTable::hash(const PID& aPageID) noexcept
{
    //std::hash<uint> lHash;
    //return (lHash(aPageID.fileID()) + lHash(aPageID.pageNo())) % _size;
    return (aPageID.fileID() + aPageID.pageNo()) % _size;
}

sMtx&    BufferHashTable::getBucketMtx(const size_t aHash) noexcept
{ 
	TRACE("got Bucket Mutex of "+std::to_string(aHash));
	return _hashTable[aHash].getMtx(); 
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
			lBCB = lBCB->_nextInChain;
		}
	}
	return rtn;
}


