#include "buf_mngr.hh"

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


