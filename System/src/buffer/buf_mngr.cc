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
	_bufferpool = new byte[_noFrames * _frameSize];
}

BufferManager::~BufferManager()
{
    delete _bufferHash;
    delete[] _bufferpool;
}

//the following is not tested at all, expect major bugs
BCB* BufferManager::fix(const pid aPageID)
{
    bool lPageNotFound = true;
    const size_t lHashIndex = _bufferHash->hash(aPageID); //determine hash of requested page
    _bufferHash->getBucketMtx(lHashIndex).lock_shared(); //lock exclusively 
    BCB* lNextBCB = _bufferHash->getBucketBCB(lHashIndex); //initialize search in hash chain
    while(lNextBCB != nullptr && lPageNotFound) //as long as there are allocated CBs
    {
        if(lNextBCB->getPID() == aPageID) //is there a CB for the requested page
        {
            //page found
            lNextBCB->getMtx().lock();
            lPageNotFound = false;

            while(lNextBCB->getFrameIndex() == SIZE_T_MAX) //page is being brought in
            {
                //do nothing
            }
            
            _bufferHash->getBucketMtx(lHashIndex).unlock_shared(); //release
        }
        else
        {
            lNextBCB = lNextBCB->getNextInChain(); //follow hash chain
        }
    }

    if(lPageNotFound)
    {
        /* page not in bufferpool. before it can be read, a free frame in the 
        * bufferpool must be found or a page must be replaced */
        lNextBCB = locatePage(aPageID, lHashIndex); //get page in bufferpool
    }
    /* Now the requested page is in the bufferpool and the CB for it is pointed to by lNextBCB */
    lNextBCB->incrFixCount(); //increase fix count on page
    return lNextBCB; //method always succeeds, todo error checking, return nullptr on error
}

BCB* BufferManager::emptyfix(const pid aPageID)
{
    BCB* lNextBCB = nullptr;
    //todo..
    return lNextBCB;
}

//BCB has to be locked beforehand
void BufferManager::unfix(BCB*& aBufferControlBlockPtr)
{
    aBufferControlBlockPtr->decrFixCount();
    aBufferControlBlockPtr = nullptr;
}

//BCB has to be locked beforehand
void BufferManager::flush(BCB*& aBufferControlBlockPtr)
{
    if(aBufferControlBlockPtr->getModified())
    {
        const pid lPageID = aBufferControlBlockPtr->getPID(); //page id of frame
        byte* lFramePtr = getFramePtr(aBufferControlBlockPtr); //pointer to the frame
        PartitionBase* lPart = PartitionManager::getInstance().getPartition(lPageID.fileID()); //get partition which contains the page to write
        lPart->open(); //open partition
        lPart->writePage(lFramePtr, lPageID.pageNo(), getFrameSize()); //write page back to disk
        lPart->close(); //close partition
    }
}

void BufferManager::flushAll()
{

}


BCB* BufferManager::locatePage(const pid aPageID, const size_t aHashIndex)
{
    //BCB = Buffer Control Block
    getFreeBCBs().getFreeBCBListMtx().lock(); //lock free BCB list
    BCB* lFBCB = getFreeBCBs().getFreeBCBList(); //get free BCB
    getFreeBCBs().setFreeBCBList(lFBCB->getNextInChain()); //set free BCB list to next free BCB
    getFreeBCBs().decrNoFreeBCBs(); //decrement number of free BCBs
    getFreeBCBs().getFreeBCBListMtx().unlock(); //unlock mutex of free BCB list
    /* init BCB */
    lFBCB->getMtx().lock(); //lock mutex of free BCB
    lFBCB->setFrameIndex(SIZE_T_MAX); //SIZE_T_MAX defined in types.hh
    lFBCB->setPID(aPageID); //store requested page
    lFBCB->setModified(false); //page is not modified
    lFBCB->setFixCount(0); //fix will be applied later
    /* now the control block is linked to the hash table chain */
    const size_t lHashIndex = _bufferHash->hash(aPageID); //compute hash for this page
    _bufferHash->getBucketMtx(lHashIndex).lock(); //lock hash bucket
    if(_bufferHash->getBucketBCB(lHashIndex) == nullptr) //if hash chain is empty
    {
        _bufferHash->setBucketBCB(lHashIndex, lFBCB); //put pointer to BCB in chain
        lFBCB->setNextInChain(nullptr);
    }
    else //there is at least one entry in hash chain
    {
        lFBCB->setNextInChain(_bufferHash->getBucketBCB(lHashIndex)); //make new BCB first in chain
        _bufferHash->setBucketBCB(lHashIndex, lFBCB); //
    }
    _bufferHash->getBucketMtx(lHashIndex).unlock(); //unlock hash bucket
    const size_t lFrameNo = getFrame(); //get a free frame
    lFBCB->setFrameIndex(lFrameNo);     
    PartitionBase* lPart = PartitionManager::getInstance().getPartition(aPageID.fileID()); //get partition which contains the requested page
    byte* lFramePtr = getFramePtr(lFBCB); //get pointer to the free frame in the bufferpool
    lPart->open(); //open partition
    lPart->readPage(lFramePtr, aPageID.pageNo(), getFrameSize());//read page from partition into free frame
    lPart->close(); //close partition
    //if prev. calls were successful, page is in this frame
    return lFBCB;
    /* note that the mutex on the free BCB is kept; */
}

size_t BufferManager::getFrame()
{
    size_t lFrameNo;
    getFreeFrames().getFreeFrameListMtx().lock(); //protect free frames array
    const size_t lNoFreeFrames = getFreeFrames().getNoFreeFrames();
    if(lNoFreeFrames > 0) //are there any free frames?
    {
        lFrameNo = getFreeFrames().getFreeFrameList()[lNoFreeFrames - 1]; //get free frame number
        getFreeFrames().decrNoFreeFrames(); //decrease free frame counter
        getFreeFrames().getFreeFrameListMtx().unlock(); //release mutex on free frame array
        return lFrameNo; //return free frame index
    } //no free frame in array
    getFreeFrames().getFreeFrameListMtx().unlock(); //release mutex on free frame array
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
                    //is the page in the frame modified? if so, flush it and check if successfull (== 0)
                    if(lHashChainEntry->getModified())
                    {//write back to disk
                        flush(lHashChainEntry);
                    }
                    //else page is not modified and can be replaced
                    lHashChainEntry->getMtx().unlock(); //unlock chain entry and continue with next
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



