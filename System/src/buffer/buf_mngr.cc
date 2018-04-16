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
    for(size_t i = 1; i < _noFreeBCBs; ++i)
    {
        newBCB->_nextInChain = _BCBs[i];
        newBCB = newBCB->_nextInChain;
    }
}

BufferManager::BufferManager() :
	_noFrames(0),
	_frameSize(0),
	_bufferHash(nullptr),
	_bufferpool(nullptr),
	_freeFrames(),
    _freeBCBs(),
    _cb(nullptr),
    _init(false)
{}

BufferManager::~BufferManager()
{
    delete _bufferHash;
    delete[] _bufferpool;
}

void BufferManager::init(const CB& aCB)
{
    if(!_init)
    {
        _noFrames = aCB.frames();
        _frameSize = aCB.pageSize();
        try
        {
            _bufferHash = new BufferHashTable(_noFrames);
            _bufferpool = new byte[_noFrames * _frameSize];
        }
        catch(std::bad_alloc& ba)
        {
            if(aCB.trace())
            {
                const std::string lErrMsg = std::string("bad_alloc caught: ") + std::string(ba.what()); 
                Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg);
            }
            throw ReturnException(__FILE__, __LINE__, __PRETTY_FUNCTION__);
        }
        _freeFrames.init(_noFrames);
        _freeBCBs.init(_noFrames);
        _cb = &aCB;
        BufferControlBlock::setCB(_cb);
        _init = true;
    }
}

//the following is not tested at all, expect major bugs
BCB* BufferManager::fix(const PID aPageID, LOCK_MODE aMode)
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
        readPageIn(lNextBCB,aPageID);
    }

    if(aMode == kNOLOCK)
    {
        lNextBCB->setLockMode(aMode);
        lNextBCB->incrFixCount();
    }
    else //some lock requested
    {
        if(aMode == kSHARED) // shared lock requested
        {
            lNextBCB->getMtx().lock_shared();
            lNextBCB->setLockMode(aMode);  
            lNextBCB->incrFixCount();
        }
        else if(aMode == kEXCLUSIVE) //exclusive lock requested
        {
            lNextBCB->getMtx().lock();
            lNextBCB->setLockMode(aMode);
            lNextBCB->setFixCount(1);
            lNextBCB->setModified(true);
        }
        else std::cerr << "Lock type not supported." << std::endl;
    }

    /* Now the requested page is in the bufferpool and the CB for it is pointed to by lNextBCB. It has the requested lock type applied */
    return lNextBCB;
}

BCB* BufferManager::emptyfix(const PID aPageID) //assumed to always request in X lock mode
{
    BCB* lNextBCB = nullptr;
    const size_t lHashIndex = _bufferHash->hash(aPageID); //determine hash of requested page
    lNextBCB = locatePage(aPageID,lHashIndex);
    lNextBCB->getMtx().lock();
    lNextBCB->setLockMode(kEXCLUSIVE);
    lNextBCB->setFixCount(1);
    lNextBCB->setModified(true);
    initNewPage(lNextBCB,aPageID,1);
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
        const PID lPageID = aBufferControlBlockPtr->getPID(); //page id of frame
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

byte* BufferManager::getFramePtr(BCB* aBCB)
{
    const size_t lFrameIndex = aBCB->getFrameIndex();   
    if(lFrameIndex >= _noFrames)
    {
        const std::string lErrMsg = std::string("Invalid BCB provided: frame index is not in buffer pool"); 
        if(_cb->trace())
        {
            Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg);
        }
        throw InvalidArgumentException(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg);
        return nullptr;
    }
    return _bufferpool + (lFrameIndex * _frameSize);
}

BCB* BufferManager::locatePage(const PID aPageID, const size_t aHashIndex)
{
    //BCB = Buffer Control Block
    getFreeBCBs().getFreeBCBListMtx().lock(); //lock free BCB list
    BCB* lFBCB = getFreeBCBs().getFreeBCBList(); //get free BCB
    getFreeBCBs().setFreeBCBList(lFBCB->getNextInChain()); //set free BCB list to next free BCB
    getFreeBCBs().decrNoFreeBCBs(); //decrement number of free BCBs
    getFreeBCBs().getFreeBCBListMtx().unlock(); //unlock mutex of free BCB list
    /* init BCB */
    lFBCB->getMtx().lock(); //lock mutex of free BCB
    lFBCB->setLockMode(kNoType); //indicate mutex has no lock type yet 
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
    lFBCB->getMtx().unlock(); //lock mutex of free BCB
    return lFBCB;
}

void BufferManager::readPageIn(BCB* lFBCB, PID aPageID){
    PartitionBase* lPart = PartitionManager::getInstance().getPartition(aPageID.fileID()); //get partition which contains the requested page
    lFBCB->getMtx().lock_shared();
    try
    {
        byte* lFramePtr = getFramePtr(lFBCB); //get pointer to the free frame in the bufferpool
        lPart->open(); //open partition
        lPart->readPage(lFramePtr, aPageID.pageNo(), getFrameSize());//read page from partition into free frame
        lPart->close(); //close partition
    }
    catch(const InvalidArgumentException& ex) //BCB had an invalid frame index
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(const FileException& ex) //something wnt wrong opening/closing/reading the partition
    {
        std::cerr << ex.what() << std::endl;
    }
    lFBCB->getMtx().unlock_shared();
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

void BufferManager::initNewPage(BCB* aFBCB,PID aPageID, uint64_t aLSN){
    byte* lFramePtr = getFramePtr(aFBCB);
    //LSN,PageIndex,PartitionID,Version, unused,unused
    basic_header_t lBH = {aLSN, aPageID.pageNo(), aPageID.fileID(), 1, 0, 0};
    lFramePtr += getFrameSize() - sizeof(basic_header_t);
    *((basic_header_t*) lFramePtr) = lBH;
}

  


