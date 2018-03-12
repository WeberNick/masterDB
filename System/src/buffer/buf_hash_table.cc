#include "buf_hash_table.hh"

BufferHashTable::BufferHashTable(const size_t aHashTableSize) :
	_size(aHashTableSize),
	_hashTable(nullptr),
	_freeCbList(nullptr),
	_freeCbMutex(),
	_noFreeCbs(_size)
{
	_hashTable = new HashBucket[_size];
    BCB* newCb = new BCB;
    _freeCbList = newCb;
    size_t i = 0;
    while(i < _noFreeCbs)
    {
        if(i < (_size - 1))
        {
            newCb->_nextInChain = new BCB;
            newCb = newCb->_nextInChain;
        }
        ++i;
    }
}

BufferHashTable::~BufferHashTable()
{
	BCB* lBufferCbPointer = nullptr;
	for(size_t i = 0; i < _size; ++i)
	{
		lBufferCbPointer = _hashTable[i]._firstBufCb;
		while(_hashTable[i]._firstBufCb)
		{
		    lBufferCbPointer = _hashTable[i]._firstBufCb;
			_hashTable[i]._firstBufCb = _hashTable[i]._firstBufCb->_nextInChain;
			delete lBufferCbPointer;
		}
	}
	delete[] _hashTable;
}


size_t BufferHashTable::hash(const pid aPageID)
{
    std::hash<uint> lHash;
    return (lHash(aPageID._fileID) + lHash(aPageID._pageID)) % _size;
}


