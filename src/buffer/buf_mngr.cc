#include "buf_mngr.hh"

void BufferManager::FreeFrames::init(const size_t aNoFreeFrames)
{
    _freeFrameList = new size_t[aNoFreeFrames];
    _noFreeFrames = aNoFreeFrames;
    for(size_t i = 0; i < _noFreeFrames; ++i)
    {
        _freeFrameList[i] = i;
    }
}

void BufferManager::FreeBCBs::init(const size_t aNoFreeBCBs)
{
    _noFreeBCBs = aNoFreeBCBs;
    _BCBs.resize(_noFreeBCBs, nullptr);
    for(size_t i = 0; i < _noFreeBCBs; ++i)
    {
        _BCBs[i] = new BCB;
    }
    BCB* newBCB = _BCBs[0];
    _freeBCBList = newBCB;
    //create the initial list of free BCBs
    for(BCB* ptr : _BCBs)
    {
        newBCB->_nextInChain = ptr;
        newBCB = newBCB->_nextInChain;
    }
}

void BufferManager::FreeBCBs::resetBCB(BCB* aBCB) noexcept
{
    TRACE("FU");
    aBCB->lock();
    freeBCB(aBCB);
    aBCB->unlock();
}
void  BufferManager::FreeBCBs::freeBCB(BCB* aBCB) noexcept
{ 
    getFreeBCBListMtx().lock();
    aBCB->setNextInChain(getFreeBCBList()); 
    setFreeBCBList(aBCB);
    incrNoFreeBCBs();
    getFreeBCBListMtx().unlock();
 }

BufferManager::BufferManager() :
	_noFrames(0),
	_frameSize(0),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrames(),
    _freeBCBs(),
    _cb(nullptr)
{
    TRACE("'BufferManager' constructed");
}

BufferManager::~BufferManager()
{
    delete _bufferHash;
    delete[] _bufferpool;
    TRACE("'BufferManager' destructed");
}

void BufferManager::init(const CB& aControlBlock)
{
    if(!_cb)
    {
        _noFrames = aControlBlock.frames();
        _frameSize = aControlBlock.pageSize();
        try
        {
            _bufferHash = new BufferHashTable(_noFrames);
            _bufferpool = new byte[_noFrames * _frameSize];
        }
        catch(std::bad_alloc& ba)
        {
            TRACE(std::string("bad_alloc caught: ") + std::string(ba.what()));
            throw;
        }
        _freeFrames.init(_noFrames);
        _freeBCBs.init(_noFrames * 1.2);
        _cb = &aControlBlock;
        BufferControlBlock::setCB(_cb);
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
    const std::vector<BCB*> lBCBs = _bufferHash->getAllValidBCBs();
    for(auto bcb : lBCBs)
    {
        if(bcb->getPID() == aPageID)
        {
            throw ReturnException(FLF);
        }
    }
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
    return _bufferpool + (lFrameIndex * _frameSize);
}

BCB* BufferManager::locatePage(const PID& aPageID) noexcept
{
    TRACE("Try to locate page");
    //BCB = Buffer Control Block
    getFreeBCBs().getFreeBCBListMtx().lock(); //lock free BCB list
    BCB* lFBCB = getFreeBCBs().getFreeBCBList(); //get free BCB

    //should not happen because we have more bcbs than frames.
    //everytime a frame is reused, a BCB is freed and inserted back into the free bcb list
    assert(lFBCB != nullptr);
    getFreeBCBs().setFreeBCBList(lFBCB->getNextInChain()); //set free BCB list to next free BCB
    getFreeBCBs().decrNoFreeBCBs(); //decrement number of free BCBs
    getFreeBCBs().getFreeBCBListMtx().unlock(); //unlock mutex of free BCB list

    /* init BCB */
    lFBCB->getMtx().lock(); //lock mutex of free BCB
    lFBCB->setLockMode(LOCK_MODE::kNoType); //indicate mutex has no lock type yet 
    lFBCB->setFrameIndex(INVALID); //INVALID defined in types.hh
    lFBCB->setPID(aPageID); //store requested page
    lFBCB->setModified(false); //page is not modified
    lFBCB->setFixCount(0); //fix will be applied later
    lFBCB->setNextInChain(nullptr); //BCB has no next BCB yet
    /* now the control block is linked to the hash table chain */
        TRACE("");
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
    lFBCB->getMtx().unlock(); //lock mutex of free BCB
    TRACE("Page located");
    return lFBCB;
}

void BufferManager::readPageIn(BCB* lFBCB, const PID& aPageID){
    TRACE("Read page '" + aPageID.to_string() + "' from disk into the buffer pool");
    TRACE("The BCB is : " + lFBCB->to_string());
    PartitionBase* lPart = PartitionManager::getInstance().getPartition(aPageID.fileID()); //get partition which contains the requested page
    lFBCB->getMtx().lock_shared();
    byte* lFramePtr = getFramePtr(lFBCB); //get pointer to the free frame in the bufferpool
    const size_t lNoTries = 3;
    for(size_t i = 0; i < lNoTries; ++i)
    {
        try
        {
            lPart->open(); //open partition
            break;
        }
        catch(const FileException& fex)
        {
            if(i == (lNoTries - 1))
            {
                TRACE(std::to_string(lNoTries) + std::string(" unsuccessful tries to open partition"));
                throw; //throw catched exception
            }
            TRACE("Could not open partition. Retry in one second");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    TRACE("Reading the page from disk into the buffer pool...");
    lPart->readPage(lFramePtr, aPageID.pageNo(), getFrameSize());//read page from partition into free frame
    lPart->close(); //close partition
    lFBCB->getMtx().unlock_shared();
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
        _bufferHash->setBucketBCB(lHashIndex, lCurBCB->getNextInChain());
        _bufferHash->getBucketMtx(lHashIndex).unlock();
        //put into free BCB List! ####
        return;
    }

    while(lCurBCB->getNextInChain()) //as long as there are allocated CBs
    {
        TRACE("one step in the chain");
        if(lCurBCB->getNextInChain()->getPID() == aPID) //is there a CB for the requested page
        {
            TRACE("page found");
            BCB* tmp = lCurBCB->getNextInChain()->getNextInChain();
            _freeBCBs.resetBCB(lCurBCB->getNextInChain());
            lCurBCB->setNextInChain(tmp);
            _bufferHash->getBucketMtx(lHashIndex).unlock(); //release
            return;
        }
        else
        {
            lCurBCB = lCurBCB->getNextInChain(); //follow hash chain
        }
    }
}


size_t BufferManager::getFrame() noexcept
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
     * tested at all. Nick and Jonas have to think about this*/
    while(true)
    {

        size_t debug = 0;

        lRandomIndex = lDistr(lRNG); //generate random index
        lHashChainEntry = _bufferHash->getBucketBCB(lRandomIndex);
        /* if chosen hash bucket is not empty, try to get exclusive lock on hash bucket */
        TRACE("trying to kick out bcb from bucket "+std::to_string(lRandomIndex));
        if(lHashChainEntry != nullptr && _bufferHash->getBucketMtx(lRandomIndex).try_lock())
        {//is this enough to work exclusively on hash chain?? maybe need to lock the BCB in chain??
            TRACE("there is something in that bucket");
            //if the first one was free
            if(lHashChainEntry->getFixCount() == 0 && lHashChainEntry->getMtx().try_lock())
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
                getFreeBCBs().getFreeBCBListMtx().lock(); //lock free BCB list
                lHashChainEntry->setNextInChain(_freeBCBs.getFreeBCBList());
                _freeBCBs.setFreeBCBList(lHashChainEntry);
                _freeBCBs.incrNoFreeBCBs();
                 getFreeBCBs().getFreeBCBListMtx().unlock(); //lock free BCB list
                lHashChainEntry->getMtx().unlock(); //unlock chain entry and continue with next
                _bufferHash->getBucketMtx(lRandomIndex).unlock();
                return lVictimIndex;
            }
            //if it was not the first one to be cleared out
            while(lHashChainEntry->getNextInChain()) //as long as there are allocated CBs
            {
                ++debug;
                TRACE("one step in the chain");
                BCB* lNext = lHashChainEntry->getNextInChain();
                //if the next one is free
                if(lNext->getFixCount() == 0 && lNext->getMtx().try_lock()) //is there a CB for the requested page
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
                    lNext->getMtx().unlock();//resetBCB will lock itself
                    _freeBCBs.resetBCB(lNext);
                    
                    _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release
                    return lVictimIndex;
                }
                else
                {
                    //continue;
                    lHashChainEntry = lNext; //follow hash chain
                }
                if(debug==20){
                    throw ReturnException(FLF);
                }
            }
           _bufferHash->getBucketMtx(lRandomIndex).unlock(); //release 
        }
    }
    return INVALID; //in theory, this can not be returned. always check for max value return
}

