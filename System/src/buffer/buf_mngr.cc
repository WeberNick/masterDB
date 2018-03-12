#include "buf_mngr.hh"

BufferManager::BufferManager(const size_t aNoFrames, const size_t aFrameSize) :
	_noFrames(aNoFrames),
	_frameSize(aFrameSize),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrameIndexes(nullptr),
	_freeFrameSem(0,10), //todo
	_noFreeFrames(_noFrames),
	_pageMRU(nullptr),
	_pageLRU(nullptr),
	_LRUSem(0,10),//todo
	_freeAccCbList(nullptr),
	_freeAccCbSem(0,10),//todo
	_noFreeAccCb(0)
{
	const size_t lHashTableSize = 0; 
	_bufferHash = new BufferHashTable(lHashTableSize);
	_bufferpool = new byte*[_noFrames];
	for(size_t i; i < _noFrames; ++i)
	{
		_bufferpool[i] = new byte[aFrameSize];
	}
	_freeFrameIndexes = new uint[_noFrames];
	for(size_t i; i < _noFrames; ++i)
	{
		_freeFrameIndexes[i] = i;
	}
	//initSemaphore(_freeFrameSem);
	//initSemaphore(_LRUSem);
}

BufferManager::~BufferManager()
{


}


