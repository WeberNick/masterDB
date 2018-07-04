#include "buf_mngr.hh"


BufferManager::FreeFrames::FreeFrames() : 
    _freeFrameList(), 
    _freeFrameListMtx(), 
    _noFreeFrames(0)
{}

void BufferManager::FreeFrames::init(const size_t aNoFreeFrames) noexcept
{
    if(!_freeFrameList.get())
    {
        _freeFrameList = std::make_unique<size_t[]>(aNoFreeFrames); 
        _noFreeFrames = aNoFreeFrames;
        for(size_t i = 0; i < _noFreeFrames; ++i)
        {
            _freeFrameList[i] = i;
        }
        TRACE("The free frames list is initialized. All frames are free.");
    }
}
size_t BufferManager::FreeFrames::pop(){
    getFreeFrameListMtx().lock(); //protect free frames array
    size_t lFrameNo = INVALID;
    if(getNoFreeFrames() > 0) //are there any free frames?
    {
        lFrameNo = getFreeFrameList()[getNoFreeFrames() - 1]; //get free frame number
        decrNoFreeFrames(); //decrease free frame counter
    } //no free frame in array
    getFreeFrameListMtx().unlock();
    return lFrameNo;
}
void BufferManager::FreeFrames::push(size_t aFrameNo){
    getFreeFrameListMtx().lock(); //protect free frames array
    getFreeFrameList()[getNoFreeFrames()] = aFrameNo;
    incrNoFreeFrames();
    getFreeFrameListMtx().unlock();
}


BufferManager::FreeBCBs::FreeBCBs() : 
    _BCBs(), 
    _freeBCBList(nullptr), 
    _freeBCBListMtx(), 
    _noFreeBCBs(0)
{}

void BufferManager::FreeBCBs::init(const size_t aNoFreeBCBs) noexcept
{
    if(!_freeBCBList)
    {
        _noFreeBCBs = aNoFreeBCBs;
        _BCBs.resize(_noFreeBCBs);
        for(size_t i = 0; i < _noFreeBCBs; ++i)
        {
            _BCBs[i] = std::make_unique<BCB>();
        }
        BCB* newBCB = _BCBs[0].get();
        _freeBCBList = newBCB;
        //create the initial list of free BCBs
        for(const auto& u_ptr : _BCBs)
        {
            newBCB->setNextInChain(u_ptr.get());
            newBCB = newBCB->getNextInChain();
        }
        TRACE("The free BCB list is initialized.");
    }
}

void BufferManager::FreeBCBs::resetBCB(BCB* aBCB) noexcept
{
    TRACE("FU");
    aBCB->lock();
    insertToFreeBCBs(aBCB);
    aBCB->unlock();
}

BufferManager::BufferManager() :
	_noFrames(0),
	_frameSize(0),
	_bufferHash(),
	_bufferpool(),
	_freeFrames(),
    _freeBCBs(),
    _cb(nullptr)
{
    TRACE("'BufferManager' constructed");
}

BufferManager::~BufferManager()
{
    TRACE("'BufferManager' destructed");
}

void BufferManager::init(const CB& aControlBlock) noexcept
{
    if(!_cb)
    {
        _noFrames = aControlBlock.frames();
        _frameSize = aControlBlock.pageSize();
        _bufferHash = std::make_unique<BufferHashTable>(_noFrames);
        _bufferpool = std::make_unique<byte[]>(_noFrames * _frameSize);
        _freeFrames.init(_noFrames);
        _freeBCBs.init(_noFrames * 1.2);
        _cb = &aControlBlock;
        TRACE("'BufferManager' initialized");
    }
}

//the following is not tested at all, expect major bugs
BCB* BufferManager::fix(const PID& aPageID, LOCK_MODE aMode)
{
    TRACE("Trying to fix page : " + aPageID.to_string());
    bool lPageNotFound = true;
    const size_t lHashIndex = _bufferHash->hash(aPageID); //determine hash of requested page
    _bufferHash->getBucketMtx(lHashIndex).lock_shared(); //lock shared 
    BCB* lNextBCB = _bufferHash->getBucketBCB(lHashIndex); //initialize search in hash chain

    while(lNextBCB != nullptr && lPageNotFound) //as long as there are allocated CBs
    {
        TRACE("One step in the chain (DELETE TRACE AFTER DEBUGGING)");
        if(lNextBCB->getPID() == aPageID) //is there a CB for the requested page
        {
            TRACE("## fix: Page found in buffer pool.");
            //page found
            lPageNotFound = false;
            TRACE("  (DELETE TRACE AFTER DEBUGGING)");
            while(lNextBCB->getFrameIndex() == INVALID) //page is being brought in
            {
                //do nothing
                TRACE("  (DELETE TRACE AFTER DEBUGGING)");
            }
            
            _bufferHash->getBucketMtx(lHashIndex).unlock_shared(); //release
        }
        else
        {
            TRACE("  (DELETE TRACE AFTER DEBUGGING)");
            lNextBCB = lNextBCB->getNextInChain(); //follow hash chain
        }
    }

    if(lPageNotFound)
    {
        TRACE("## fix: Page not found in buffer pool. Trying to get a free frame for the page...");
        TRACE( "Page "+std::to_string(aPageID.pageNo())+" not found");
        //bucket has to be unlocked for further code.
        _bufferHash->getBucketMtx(lHashIndex).unlock_shared();
        /* page not in bufferpool. before it can be read, a free frame in the 
        * bufferpool must be found or a page must be replaced */
        lNextBCB = locatePage(aPageID); //get page in bufferpool
        readPageIn(lNextBCB,aPageID);
        TRACE("## fix: Page is in the buffer pool now");
    }
    TRACE("## fix: Setting the lock mode (" + lockModeToString(aMode) + ") for the BCB...");
    lNextBCB->lock(aMode);
    /* Now the requested page is in the bufferpool and the CB for it is pointed to by lNextBCB. It has the requested lock type applied */
    TRACE("Page : " + aPageID.to_string() + " is fixed");
    return lNextBCB;
}

BCB* BufferManager::emptyfix(const PID& aPageID) //assumed to always request in X lock mode
{
    TRACE("Emptyfix on page : " + aPageID.to_string());
    BCB* lNextBCB = nullptr;
    lNextBCB = locatePage(aPageID);
    lNextBCB->lock();
    lNextBCB->setModified(true);
    initNewPage(lNextBCB, aPageID, 1);
    TRACE("Page : " + aPageID.to_string() + " is fixed");
    return lNextBCB;
}

//BCB has to be locked beforehand
void BufferManager::unfix(BCB*& aBufferControlBlockPtr)
{
    TRACE("Unfix BCB with PID : " + aBufferControlBlockPtr->getPID().to_string());
    aBufferControlBlockPtr->unlock();
    TRACE("BCB with PID : " + aBufferControlBlockPtr->getPID().to_string() + " is unfixed");
    aBufferControlBlockPtr = nullptr;
}

//BCB has to be locked beforehand
void BufferManager::flush(BCB*& aBufferControlBlockPtr)
{
    TRACE("Flush the BCB with PID : " + aBufferControlBlockPtr->getPID().to_string());
    if(aBufferControlBlockPtr->getModified())
    {
        const PID lPageID = aBufferControlBlockPtr->getPID(); //page id of frame
        byte* lFramePtr = getFramePtr(aBufferControlBlockPtr); //pointer to the frame
        PartitionBase* lPart = PartitionManager::getInstance().getPartition(lPageID.fileID()); //get partition which contains the page to write
        lPart->open(); //open partition
        lPart->writePage(lFramePtr, lPageID.pageNo(), getFrameSize()); //write page back to disk
        lPart->close(); //close partition
    }
    TRACE("BCB with PID : " + aBufferControlBlockPtr->getPID().to_string() + " was flushed");
}

void BufferManager::flushAll()
{
    TRACE("Flush of the complete buffer starts...");
    std::vector<BCB*> lBCBs = _bufferHash->getAllValidBCBs();
    TRACE("number of Pages to flush: "+std::to_string(lBCBs.size()));
    for (uint i = 0; i < lBCBs.size(); ++i){
        TRACE("iteration: "+std::to_string(i));
        BCB* lBCB = lBCBs.at(i);
        TRACE("Partition: "+std::to_string(lBCB->getPID().fileID())+" Page: "+std::to_string(lBCB->getPID().pageNo()));
        flush(lBCB);
    }
    TRACE("Finished flushing the complete buffer");
}

byte* BufferManager::getFramePtr(BCB* aBCB)
{
    const size_t lFrameIndex = aBCB->getFrameIndex();   
    assert(lFrameIndex < _noFrames); // otherwise BCB is somewhere else corrupted
    return _bufferpool.get() + (lFrameIndex * _frameSize);
}

BCB* BufferManager::locatePage(const PID& aPageID) noexcept
{
    TRACE("Locating page '" + aPageID.to_string() + "'...");
    //BCB = Buffer Control Block
    BCB* lFBCB = getFreeBCBs().popFromList(); //get free BCB

    /* init BCB */
    lFBCB->lock(); //lock mutex of free BCB
   // lFBCB->setLockMode(LOCK_MODE::kNoType); //indicate mutex has no lock type yet 
    lFBCB->setFrameIndex(INVALID); //INVALID defined in types.hh
    lFBCB->setPID(aPageID); //store requested page
    lFBCB->setModified(false); //page is not modified
    //lFBCB->setFixCount(0); //fix will be applied later
    lFBCB->setNextInChain(nullptr); //BCB has no next BCB yet
    /* now the control block is linked to the hash table chain */
    const size_t lHashIndex = _bufferHash->hash(aPageID); //compute hash for this page
    _bufferHash->getBucketMtx(lHashIndex).lock(); //lock hash bucket
    if(_bufferHash->getBucketBCB(lHashIndex) == nullptr) //if hash chain is empty
    {
        _bufferHash->setBucketBCB(lHashIndex, lFBCB); //put pointer to BCB in chain
    }
    else //there is at least one entry in hash chain
    {
        lFBCB->setNextInChain(_bufferHash->getBucketBCB(lHashIndex)); //make new BCB first in chain
        _bufferHash->setBucketBCB(lHashIndex, lFBCB); //
    }
    _bufferHash->getBucketMtx(lHashIndex).unlock(); //unlock hash bucket
    const size_t lFrameNo = getFrame(); //get a free frame
    lFBCB->setFrameIndex(lFrameNo);     
    lFBCB->unlock(); //lock mutex of free BCB
    TRACE("Page located");
    return lFBCB;
}

void BufferManager::readPageIn(BCB* lFBCB, const PID& aPageID){
    TRACE("Read page '" + aPageID.to_string() + "' from disk into the buffer pool");
    TRACE("The BCB is : " + lFBCB->to_string());
    PartitionBase* lPart = PartitionManager::getInstance().getPartition(aPageID.fileID()); //get partition which contains the requested page
    lFBCB->lock_shared();//why shared?
    byte* lFramePtr = getFramePtr(lFBCB); //get pointer to the free frame in the bufferpool
    lPart->open(); //open partition
    TRACE("Reading the page from disk into the buffer pool...");
    lPart->readPage(lFramePtr, aPageID.pageNo(), getFrameSize());//read page from partition into free frame
    lPart->close(); //close partition
    lFBCB->unlock();
    TRACE("Read in finished. Page is now in the buffer pool");
}



// size_t BufferManager::getFrame() noexcept
// {
//     size_t lFrameNo;
//     getFreeFrames().getFreeFrameListMtx().lock(); //protect free frames array
//     const size_t lNoFreeFrames = getFreeFrames().getNoFreeFrames();
//     if(lNoFreeFrames > 0) //are there any free frames?
//     {
//         lFrameNo = getFreeFrames().getFreeFrameList()[lNoFreeFrames - 1]; //get free frame number
//         getFreeFrames().decrNoFreeFrames(); //decrease free frame counter
//         getFreeFrames().getFreeFrameListMtx().unlock(); //release mutex on free frame array
//         return lFrameNo; //return free frame index
//     } //no free frame in array
//     getFreeFrames().getFreeFrameListMtx().unlock(); //release mutex on free frame array
//     //pick random frame to evict
//     std::random_device lSeeder; //the random device that will seed the generator
//     std::mt19937 lRNG(lSeeder()); //then make a mersenne twister engine
//     const size_t lMin = 0; //random numbers between 0...
//     const size_t lMax = _bufferHash->getTableSize() - 1; //...and the size of the hash table
//     std::uniform_int_distribution<size_t> lDistr(lMin, lMax); //the distribution
//     size_t lRandomIndex;
//     BCB* lHashChainEntry;
//     /* what follows is a random replacement strategy for frames in the bufferpool. This, however is not
//      * tested at all. Need to think about this with Jonas! (todo: testing)*/
//     while(true)
//     {
//         lRandomIndex = lDistr(lRNG); //generate random index
//         lHashChainEntry = _bufferHash->getBucketBCB(lRandomIndex);
//         /* if chosen hash bucket is not empty, try to get exclusive lock on hash bucket */
//         TRACE("trying to kick out bcb from bucket "+std::to_string(lRandomIndex));
//         if(lHashChainEntry != nullptr && _bufferHash->getBucketMtx(lRandomIndex).try_lock())
//         {//is this enough to work exclusively on hash chain?? maybe need to lock the BCB in chain??
//         TRACE("there is something in that bucket");
//         uint counter = 0;
//             do
//             {
//                 ++counter;
//                 /* if BCB/frame is not fixed, try to get exclusive lock */
//                 TRACE("trying to find a victim");
//                 TRACE("candidate: "+std::to_string(counter)+": "+lHashChainEntry->to_string());
//                 if(lHashChainEntry->getFixCount() == 0 && lHashChainEntry->getMtx().try_lock())
//                 {//victim for replacement found
//                     TRACE("found a victim");
//                     const size_t lVictimIndex = lHashChainEntry->getFrameIndex(); //frame index of victim
//                     //is the page in the frame modified? if so, flush it and check if successfull (== 0)
//                     if(lHashChainEntry->getModified())
//                     {//write back to disk
//                         flush(lHashChainEntry);
//                     }
//                     lHashChainEntry->setNextInChain(_freeBCBs.getFreeBCBList());
//                     _freeBCBs.setFreeBCBList(lHashChainEntry);
//                     //else page is not modified and can be replaced
//                     lHashChainEntry->getMtx().unlock(); //unlock chain entry and continue with next
//                     _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release lock on hash bucket
//                     return lVictimIndex; //return frame index
//                 }//frame is fixed or hash chain entry is locked, go to next entry
//                 lHashChainEntry = lHashChainEntry->getNextInChain(); //follow chain, get next entry
//                 if(counter==20){
//                     throw ReturnException(FLF);
//                 }
//             }while(lHashChainEntry != nullptr);
//             //no victim found, next in chain is nullptr, continue
//             _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release bucket mutex
//         }//hash bucket is empty or locked, continue
//     }
//     return INVALID; //in theory, this can not be returned. always check for max value return
// }

void BufferManager::initNewPage(BCB* aFBCB, const PID& aPageID, uint64_t aLSN){
    byte* lFramePtr = getFramePtr(aFBCB);
    //LSN,PageIndex,PartitionID,Version, unused,unused
    basic_header_t lBH = {aLSN, aPageID.pageNo(), aPageID.fileID(), 1, 0, 0};
    lFramePtr += getFrameSize() - sizeof(basic_header_t);
    *((basic_header_t*) lFramePtr) = lBH;
}

void BufferManager::resetBCB(const PID& aPID) noexcept
{
    const size_t lHashIndex = _bufferHash->hash(aPID); //determine hash of requested page
    TRACE("Partition: "+std::to_string(aPID.fileID())+" Page: "+std::to_string(aPID.pageNo()));

    _bufferHash->getBucketMtx(lHashIndex).lock(); //lock exclusively 
    BCB* lCurBCB = _bufferHash->getBucketBCB(lHashIndex); //initialize search in hash chain

    if(!lCurBCB)
    {
        TRACE("Hash Bucket empty");
        _bufferHash->getBucketMtx(lHashIndex).unlock();
        return;
    }
    else if(lCurBCB->getPID() == aPID)
    {
        TRACE("was first in bucket");
        _bufferHash->setBucketBCB(lHashIndex, lCurBCB->getNextInChain()); //delete from bucket
        getFreeFrames().push(lCurBCB->getFrameIndex()); //free frame
        _freeBCBs.resetBCB(lCurBCB);    //free BCB
        _bufferHash->getBucketMtx(lHashIndex).unlock();
        return;
    }

    while(lCurBCB->getNextInChain()) //as long as there are allocated CBs
    {
        TRACE("one step in the chain");
        if(lCurBCB->getNextInChain()->getPID() == aPID) //is there a CB for the requested page
        {
            TRACE("page found");
            BCB* tmp = lCurBCB->getNextInChain()->getNextInChain();
            getFreeFrames().push(lCurBCB->getNextInChain()->getFrameIndex()); //free frame
            _freeBCBs.resetBCB(lCurBCB->getNextInChain()); //free BCB
            lCurBCB->setNextInChain(tmp);   //link exclude from chain
            _bufferHash->getBucketMtx(lHashIndex).unlock(); //release
            return;
        }
        else
        {
            lCurBCB = lCurBCB->getNextInChain(); //follow hash chain
        }
    }
    _bufferHash->getBucketMtx(lHashIndex).unlock();
    TRACE("PID was not in Buffer");
}


size_t BufferManager::getFrame() noexcept
{
    
    //look pop() implemented, look at old getFrame() to see what was before
    size_t lFrameNo = getFreeFrames().pop();
    if(lFrameNo != INVALID){
        return lFrameNo;
    }
    //if INVALID, there is no free frame in the list, try to get rid of one.

    //pick random frame to evict
    std::random_device lSeeder; //the random device that will seed the generator
    std::mt19937 lRNG(lSeeder()); //then make a mersenne twister engine
    const size_t lMin = 0; //random numbers between 0...
    const size_t lMax = _bufferHash->getTableSize() - 1; //...and the size of the hash table
    std::uniform_int_distribution<size_t> lDistr(lMin, lMax); //the distribution
    size_t lRandomIndex;
    BCB* lHashChainEntry;
    /* what follows is a random replacement strategy for frames in the bufferpool. This, however is not
     * tested at all. Nick and Jonas have to think about this*/
    while(true)
    {
        lRandomIndex = lDistr(lRNG); //generate random index
        lHashChainEntry = _bufferHash->getBucketBCB(lRandomIndex);
        /* if chosen hash bucket is not empty, try to get exclusive lock on hash bucket */
        TRACE("trying to kick out bcb from bucket "+std::to_string(lRandomIndex));
        if(lHashChainEntry != nullptr && _bufferHash->getBucketMtx(lRandomIndex).try_lock())
        {//is this enough to work exclusively on hash chain?? maybe need to lock the BCB in chain??
            TRACE("there is something in that bucket");
            //if the first one was free
            if(lHashChainEntry->getFixCount() == 0 && lHashChainEntry->try_lock())
            {
                TRACE("was first in bucket");
                const size_t lVictimIndex = lHashChainEntry->getFrameIndex(); //frame index of victim
                //is the page in the frame modified? if so, flush it and check if successfull (== 0)
                if(lHashChainEntry->getModified())
                {//write back to disk
                    flush(lHashChainEntry);
                }
                //kick out from bucket
                _bufferHash->setBucketBCB(lRandomIndex, lHashChainEntry->getNextInChain());
                //insert into free BCB list
                getFreeBCBs().insertToFreeBCBs(lHashChainEntry);
                lHashChainEntry->unlock(); //unlock chain entry and continue with next
                _bufferHash->getBucketMtx(lRandomIndex).unlock();
                return lVictimIndex;
            }
            //if it was not the first one to be cleared out
            while(lHashChainEntry->getNextInChain()) //as long as there are allocated CBs
            {
                TRACE("one step in the chain");
                BCB* lNext = lHashChainEntry->getNextInChain();
                //if the next one is free
                if(lNext->getFixCount() == 0 && lNext->try_lock()) //is there a CB for the requested page
                {
                    TRACE("found a victim");
                    BCB* tmp = lNext->getNextInChain();
                    const size_t lVictimIndex = lNext->getFrameIndex();
                    //if modified
                    if(lNext->getModified())
                    {//write back to disk
                        flush(lNext);
                    }
                    //exclude BCB from chain (linked list)
                    lHashChainEntry->setNextInChain(tmp);                 
                    //insert BCB into free BCB list
                    _freeBCBs.insertToFreeBCBs(lNext);
                    lNext->unlock();//resetBCB will lock itself

                    _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release
                    return lVictimIndex;
                }
                else
                {
                    //continue;
                    lHashChainEntry = lNext; //follow hash chain
                }
            }
           _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release 
        }
    }
    return INVALID; //in theory, this can not be returned. always check for max value return
}

