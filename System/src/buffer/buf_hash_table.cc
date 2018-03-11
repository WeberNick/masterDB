#include "buf_hash_table.hh"

BufferHashTable::BufferHashTable(const std::size_t aHashTableSize) :
	_size(aHashTableSize),
	_hashTable(nullptr),
	_freeCbList(nullptr),
	_freeCbSem(),
	_noFreeCbs()
{
	_hashTable = new bucket_t[_size];
    for(std::size_t i = 0; i < _size; ++i)
    {
        _hashTable[i]._firstBufCb = nullptr;
        //initSemaphoreX(_hashTable[i]._bucketSem);
    }
    BCB* newCb = new BCB;
    _freeCbList = newCb;
    uint buffersize = 0; //PLACEHOLDER
    uint delta = 0; //PLACEHOLDER
    _noFreeCbs = buffersize + delta;
    std::size_t i = 0;
    while(i < _noFreeCbs)
    {
        newCb->_pageID = {0,0};
        newCb->_frameIndex = -1; //uint?
        //initSemaphore(newCb->_pageSem);
        newCb->_prevInLRU = nullptr;
        newCb->_nextInLRU = nullptr; 
        if(i == (buffersize - 1))
        {
            newCb->_nextInChain = nullptr;
        }
        else
        {
            newCb->_nextInChain = new BCB;
            newCb = newCb->_nextInChain;
        }
        ++i;
    }
    //initSemaphore(_freeCbSem);
    //
}

BufferHashTable::~BufferHashTable()
{
	BCB* lBufferCbPointer = nullptr;
	for(std::size_t i = 0; i < _size; ++i)
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


