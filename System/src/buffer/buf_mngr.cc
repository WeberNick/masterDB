#include "buf_mngr.hh"

BufferManager::BufferManager(const size_t aNoFrames, const control_block_t& aControlBlock) :
	_noFrames(aNoFrames),
	_frameSize(aControlBlock.pageSize()),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrames(_noFrames),
    _freeBCBs(_noFrames),
    _controlBlock(aControlBlock)
{
	_bufferHash = new BufferHashTable(_noFrames);
	_bufferpool = new byte*[_noFrames];
	for(size_t i; i < _noFrames; ++i)
	{
		_bufferpool[i] = new byte[_frameSize];
	}
}

BufferManager::~BufferManager()
{
    delete _bufferHash;
    for(size_t i = 0; i < _noFrames; ++i)
    {
        delete[] _bufferpool[i];
    }
    delete[] _bufferpool;
}

//the following is not tested at all, expect major bugs
bool BufferManager::fix(const pid aPageID, const LOCK_MODE aMode, byte*& aPagePointer)
{
    aPagePointer = nullptr;
    //retry: ??
    size_t lHashIndex = _bufferHash->hash(aPageID); //determine hash of requested page
    _bufferHash->getBucketMtx(lHashIndex).lock_shared(); //protect by exclusive semaphor (?)
    BCB* lNextBCB = _bufferHash->getBucketBCB(lHashIndex); //initialize search in hash chain
    while(lNextBCB != nullptr) //as long as there are allocated CBs
    {
        if(lNextBCB->getPID() == aPageID) //is there a CB for the requested page
        {
            if(lNextBCB->getMtx().try_lock_shared()) //is x-mutex on page?
            {
                if(lNextBCB->getFrameIndex() == SIZE_T_MAX) //page is being brought in (SIZE_T_MAX defined in types.hh)
                {
                    _bufferHash->getBucketMtx(lHashIndex).unlock_shared(); //release
                    //goto retry??
                }
            }
            else
            {
                lNextBCB->getMtx().unlock_shared(); //correct????
            }
            //goto p_found ??
        }
        else
        {
            lNextBCB = lNextBCB->getNextInChain(); //follow hash chain
        }
    }
    /* page not in bufferpool. before it can be read, a free frame in the 
     * bufferpool must be found or a page must be replaced */
    lNextBCB = locatePage(aPageID, lHashIndex); //get page in bufferpool
    /* Now the requested page is in the bufferpool and the CB for it pointed to by lNextBCB */
    //p_found: ??
    if(aMode != kFREE) //caller wants a lock to be set
    {
        if(aMode == kSHARED) //caller wants a shared lock
        {
            lNextBCB->getMtx().lock_shared(); //shared lock
        }
        else
        {
            lNextBCB->getMtx().lock(); //exclusive lock
            lNextBCB->setFixCount(1); //othere is one fix on that page
            //LSN stuff
        }
    } //lock handling complete
    if(aMode == kFREE || aMode == kSHARED) //no exclusive lock requested
    {
        lNextBCB->incrFixCount(); //increase fix count on page
    }
    //pointer to buffer frame it set 
    aPagePointer = _bufferpool[lNextBCB->getFrameIndex()];
    return true; //method always succeeds, todo error checking
}


BCB* BufferManager::locatePage(const pid aPageID, const size_t aHashIndex)
{
    //BCB = Buffer Control Block
    FreeBCBs* lFBCBs = &getFreeBCBs();
    BCB* lNextBCB;
    lFBCBs->getFreeBCBListMtx().lock(); //lock mutex of free BCB list
    lNextBCB = lFBCBs->getFreeBCBList(); //get free BCB
    lFBCBs->setFreeBCBList(lNextBCB->getNextInChain()); //set free BCB list to next free BCB
    lFBCBs->decrNoFreeBCBs(); //decrement number of free BCBs
    lFBCBs->getFreeBCBListMtx().unlock(); //unlock mutex of free BCB list
    //file handle ?
    //block no = page no?
    lNextBCB->getMtx().lock(); //lock mutex of free BCB
    lNextBCB->setFrameIndex(SIZE_T_MAX); //SIZE_T_MAX defined in types.hh
    /* init BCB */
    lNextBCB->setPID(aPageID); //store requested page
    //lNextBCB->/*filehandle*/
    lNextBCB->setModified(false); //page is not modified
    lNextBCB->setFixCount(0); //page was just read, fix will be applied later
    //LSN stuff
    /* now the control block is linked to the hash table chain */
    size_t lHashIndex = _bufferHash->hash(aPageID); //compute hash for this page
    _bufferHash->getBucketMtx(lHashIndex).lock(); //lock hash bucket

    if(_bufferHash->getBucketBCB(lHashIndex) == nullptr) //if hash chain is empty
    {
        _bufferHash->setBucketBCB(lHashIndex, lNextBCB); //put pointer to BCB in chain
        lNextBCB->setNextInChain(nullptr); //
    }
    else //there is at least one entry in hash chain
    {
        lNextBCB->setNextInChain(_bufferHash->getBucketBCB(lHashIndex)); //make new BCB first in chain
        _bufferHash->setBucketBCB(lHashIndex, lNextBCB); //
    }
    _bufferHash->getBucketMtx(lHashIndex).unlock(); //unlock hash bucket
    size_t lFrameNo = getFrame(); //get a free frame
    PartitionBase* lPart = PartitionManager::getInstance().getPartition(aPageID.fileID()); //get partition which contains the requested page
    byte* lFramePtr = _bufferpool[lFrameNo]; //get pointer to the free frame in the bufferpool
    lPart->open(); //open partition
    lPart->readPage(lFramePtr, aPageID.pageNo(), getFrameSize());//read page from partition into free frame
    lPart->close(); //close partition
    lNextBCB->setFrameIndex(lFrameNo); //if prev. calls were successful, page is in this frame
    _bufferHash->getBucketMtx(lHashIndex).lock_shared(); //shared lock on hash bucket
    lNextBCB->getMtx().unlock(); //unlock mutex of free BCB
    return lNextBCB;
    /* note that the mutex on the hash bucket is kept; will be returned by fix */
}

size_t BufferManager::getFrame()
{
    FreeFrames* lFF = &getFreeFrames();
    size_t lFrameNo;
    lFF->getFreeFrameListMtx().lock(); //protect free frames array
    if(lFF->getNoFreeFrames() > 0) //are there any free frames?
    {
        lFrameNo = lFF->getFreeFrameList()[lFF->getNoFreeFrames() - 1]; //get free frame number
        lFF->decrNoFreeFrames(); //decrease free frame counter
        lFF->getFreeFrameListMtx().unlock(); //release mutex on free frame array
        return lFrameNo; //return free frame index
    } //no free frame in array
    lFF->getFreeFrameListMtx().unlock(); //release mutex on free frame array

    //pick random frame to evict
    std::random_device lSeeder; //the random device that will seed the generator
    std::mt19937 lRNG(lSeeder()); //then make a mersenne twister engine
    const size_t lMin = 0; //random numbers between 0...
    const size_t lMax = _bufferHash->getTableSize() - 1; //...and the size of the hash table
    std::uniform_int_distribution<size_t> lDistr(lMin, lMax); //the distribution
    size_t lRandomIndex;
    BCB* lHashChainEntry;
    /* what follows is a random replacement strategy for frames in the bufferpool. This, however is not
     * tested at all. Need to think about this with Jonas! (todo: testing)*/
    while(true)
    {
        lRandomIndex = lDistr(lRNG); //generate random index
        lHashChainEntry = _bufferHash->getBucketBCB(lRandomIndex);
        /* if chosen hash bucket is not empty, try to get exclusive lock on hash bucket */
        if(lHashChainEntry != nullptr && _bufferHash->getBucketMtx(lRandomIndex).try_lock())
        {//is this enough to work exclusively on hash chain?? maybe need to lock the BCB in chain??
            do
            {
                /* if BCB/frame is not fixed, try to get exclusive lock */
                if(lHashChainEntry->getFixCount() == 0 && lHashChainEntry->getMtx().try_lock())
                {//victim for replacement found
                    const size_t lVictimIndex = lHashChainEntry->getFrameIndex(); //frame index of victim
                    if(lHashChainEntry->getModified()) //is the page in the frame modified?
                    {//write back to disk
                        pid lPageID = lHashChainEntry->getPID(); //page id of victim frame
                        byte* lFramePtr = _bufferpool[lVictimIndex]; //pointer to the victim frame
                        PartitionBase* lPart = PartitionManager::getInstance().getPartition(lPageID.fileID()); //get partition which contains the page to replace
                        lPart->open(); //open partition
                        lPart->writePage(lFramePtr, lPageID.pageNo(), getFrameSize()); //write page back to disk
                        lPart->close(); //close partition
                    }//else page is not modified and can be replaced
                    _lHashChainEntry->getMtx().unlock(); //unlock chain entry and continue with next
                    _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release lock on hash bucket
                    return lVictimIndex; //return frame index
                }//frame is fixed or hash chain entry is locked, go to next entry
                lHashChainEntry = lHashChainEntry->getNextInChain(); //follow chain, get next entry
            }while(lHashChainEntry != nullptr);
            //no victim found, next in chain is nullptr, continue
            _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release bucket mutex
        }//hash bucket is empty or locked, continue
    }
    return SIZE_T_MAX; //in theory, this can not be returned. always check for max value return
}



