#include "buf_mngr.hh"

BufferManager::BufferManager(const size_t aNoFrames, const control_block_t& aControlBlock) :
	_noFrames(aNoFrames),
	_frameSize(aControlBlock.pageSize()),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrameIndexes(nullptr),
	_freeFrameMutex(), //todo
	_noFreeFrames(_noFrames),
    _controlBlock(aControlBlock)
{
	_bufferHash = new BufferHashTable(_noFrames);
	_bufferpool = new byte*[_noFrames];
	for(size_t i; i < _noFrames; ++i)
	{
		_bufferpool[i] = new byte[_frameSize];
	}
	_freeFrameIndexes = new uint[_noFrames];
	for(size_t i; i < _noFrames; ++i)
	{
		_freeFrameIndexes[i] = i;
	}
}

BufferManager::~BufferManager()
{
    delete _bufferHash;
    delete[] _freeFrameIndexes;
    for(size_t i = 0; i < _noFrames; ++i)
    {
        delete[] _bufferpool[i];
    }
    delete[] _bufferpool;
}

bool BufferManager::fix(const pid aPageID, const LOCK_MODE aMode, byte*& aPagePointer)
{
    aPagePointer = nullptr;
    //retry: ??
    size_t lHashIndex = _bufferHash->hash(aPageID); //determine hash of requested page
    _bufferHash->getBucketMutex(lHashIndex).lock_shared(); //protect by exclusive semaphor (?)
    BCB* lNextBufCb = _bufferHash->getBucketCb(lHashIndex); //initialize search in hash chain
    while(lNextBufCb != nullptr) //as long as there are allocated CBs
    {
        if(lNextBufCb->getPID() == aPageID) //is there a CB for the requested page
        {
            if(lNextBufCb->getMutex().try_lock_shared()) //is x-mutex on page?
            {
                if(lNextBufCb->getFrameIndex() == -1) //page is being brought in (-1 == MAX_VALUE)
                {
                    _bufferHash->getBucketMutex(lHashIndex).unlock_shared(); //release
                    //goto retry??
                }
            }
            else
            {
                lNextBufCb->getMutex().unlock_shared(); //correct????
            }
            //goto p_found ??
        }
        else
        {
            lNextBufCb = lNextBufCb->getNextInChain(); //follow hash chain
        }
    }
    /* page not in bufferpool. before it can be read, a free frame in the 
     * bufferpool must be found or a page must be replaced */
    lNextBufCb = locatePage(aPageID, lHashIndex); //get page in bufferpool
    /* Now the requested page is in the bufferpool and the CB for it pointed to by lNextBufCb */
    //p_found: ??
    if(aMode != kFREE) //caller wants a lock to be set
    {
        if(aMode == kSHARED) //caller wants a shared lock
        {
            lNextBufCb->getMutex().lock_shared(); //shared lock
        }
        else
        {
            lNextBufCb->getMutex().lock(); //exclusive lock
            lNextBufCb->setFixCount(1); //othere is one fix on that page
            //LSN stuff
        }
    } //lock handling complete
    if(aMode == kFREE || aMode == kSHARED) //no exclusive lock requested
    {
        lNextBufCb->incrFixCount(); //increase fix count on page
    }
    //pointer to buffer frame it set 
    aPagePointer = _bufferpool[lNextBufCb->getFrameIndex()];
    return true; //method always succeeds, todo error checking
}


BCB* BufferManager::locatePage(const pid aPageID, const size_t aHashIndex)
{


}

int BufferManager::getFrame()
{




}



