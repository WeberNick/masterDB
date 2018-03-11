#include "buf_mngr.hh"

BufferHashTable::BufferHashTable(const std::size_t aHashTableSize) :
	_size(aHashTableSize),
	_hashTable(nullptr),
	_freeCbList(nullptr),
	_freeCbSem(),
	_noFreeCbs()
{
	_hashTable = new buffer_bucket_t[_size];
    for(std::size_t i = 0; i < _size; ++i)
    {
        _hashTable[i]._firstBufCb = nullptr;
        //initSemaphoreX(_hashTable[i]._bucketSem);
    }
    buffer_cb_t* newCb = new buffer_cb_t;
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
            newCb->_nextInChain = new buffer_cb_t;
            newCb = newCb->_nextInChain;
        }
        ++i;
    }
    //initSemaphore(_freeCbSem);
    //
}

BufferHashTable::~BufferHashTable()
{
	buffer_cb_t* lBufferCbPointer = nullptr;
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

/* ===================================================================================================== */

BufferManager::BufferManager(const std::size_t aNoFrames, const std::size_t aFrameSize) :
	_noFrames(aNoFrames),
	_frameSize(aFrameSize),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrameIndexes(nullptr),
	_freeFrameSem(NULL), //todo
	_noFreeFrames(_noFrames),
	_pageMRU(nullptr),
	_pageLRU(nullptr),
	_LRUSem(),
	_freeAccCbList(nullptr),
	_freeAccCbSem(),
	_noFreeAccCb(0)
{
	const std::size_t lHashTableSize = 0; 
	_bufferHash = new BufferHashTable(lHashTableSize);
	_bufferpool = new byte*[_noFrames];
	for(std::size_t i; i < _noFrames; ++i)
	{
		_bufferpool[i] = new byte[aFrameSize];
	}
	_freeFrameIndexes = new uint[_noFrames];
	for(std::size_t i; i < _noFrames; ++i)
	{
		_freeFrameIndexes[i] = i;
	}
	//initSemaphore(_freeFrameSem);
	//initSemaphore(_LRUSem);
}

BufferManager::~BufferManager()
{


}


