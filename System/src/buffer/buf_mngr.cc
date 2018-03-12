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

bool BufferManager::fix(const pid aPageID, const LOCK_MODE aMode, BACB* aBufferAccCbPointer)
{
    size_t lHashIndex;
    BCB* lNextBufCb;
    int lFileHandle;
    int lBlockNo;
    //retry
    lHashIndex = _bufferHash->hash(aPageID);
    //sem_get() lock shared

    lNextBufCb = _bufferHash->getBucketCb(lHashIndex);
    while(lNextBufCb != nullptr)
    {
        if(lNextBufCb->getPID() == aPageID)
        {
            if(/*semaphore stuff*/0)
            {
                if(lNextBufCb->getFrameIndex() == 0) //? -1 ?
                {
                    //sem stuff, goto retry..
                }
            }
            else
            {
                //sem stuff
            }
            //goto p_found
        }
        else
        {
            lNextBufCb = lNextBufCb->getNextInChain();
        }
    }
    lNextBufCb = locatePage(aPageID, lHashIndex);
    //p_found
    if(aMode != kFREE)
    {
        if(aMode == kSHARED)
        {
            //sem stuff
        }
        else
        {
            //sem stuff
            lNextBufCb->setFixCount(1);
        }
    }
    if(aMode == kFREE || aMode == kSHARED)
    {
        lNextBufCb->incrFixCount();
    }
    aBufferAccCbPointer = getBufAccCb();
    aBufferAccCbPointer->setPID(lNextBufCb->getPID());
    aBufferAccCbPointer->setPagePtr(_bufferpool[lNextBufCb->getFrameIndex()]);
        
        


}
