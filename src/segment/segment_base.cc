#include "segment_base.hh"

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock) : 
	_segID(aSegID),
	_indexPages(),
    _pages(),
    _partition(aPartition),
    _bufMan(BufferManager::getInstance()),
    _cb(aControlBlock)
{
	_partition.open();
    int lSegmentIndex = _partition.allocPage();
	_indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
	_partition.close();
    TRACE("SegmentBase successfully created.") ;

  //no need to init pages, will be done in store.
}

SegmentBase::SegmentBase(PartitionBase& aPartition, const CB& aControlBlock) : 
	_segID(0),
	_indexPages(),
    _pages(),
    _partition(aPartition),
    _bufMan(BufferManager::getInstance()),
    _cb(aControlBlock)
{}

SegmentBase::~SegmentBase()
{}

byte* SegmentBase::getPage(const uint aPageNo, LOCK_MODE aMode)
{
    if(aMode == kNOLOCK)
        return getPageF(aPageNo);
    else if(aMode == kSHARED)
        return getPageS(aPageNo);
    else if(aMode == kEXCLUSIVE)
        return getPageX(aPageNo);
    else 
        return nullptr;
}

void SegmentBase::writePage(const uint aPageNo)
{
    TRACE("Trying to write Page");
    BCB*& lBCB = _pages.at(aPageNo).second; //may throw if aPageNo not in map
    if(lBCB != nullptr)
    {
            TRACE("Trying to write Page");

        lBCB->setModified(true);
        _bufMan.flush(lBCB); 
            TRACE("Trying to write Page");
   
    }
}

void SegmentBase::releasePage(const uint aPageNo, const bool aModified)
{
    BCB*& lBCB = _pages.at(aPageNo).second; //may throw if aPageNo not in map
    if(lBCB != nullptr)
    {
        switch(lBCB->getLockMode())
        {
            case kNOLOCK:
                break;
            case kSHARED:
                lBCB->getMtx().unlock_shared();
                break;
            case kEXCLUSIVE:
                lBCB->setModified(aModified);
                lBCB->getMtx().unlock();
                break;
            default:
                const std::string lErrMsg("Lock type not supported");
                TRACE(lErrMsg);
                throw SwitchException(FLF, lErrMsg);
                break;    
        }
        _bufMan.unfix(lBCB);
    }
    lBCB = nullptr;
}

byte* SegmentBase::getPageF(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
        lBCB = _bufMan.fix(lPID, kNOLOCK);
    }
    else if(lBCB->getLockMode() < kNOLOCK) //should never occur
    {
        std::cerr << "BCB has lock type 'kNoType', this should not occur!" << std::endl;
        return nullptr;
    }
    return _bufMan.getFramePtr(lBCB);
}

byte* SegmentBase::getPageS(const uint aPageNo)
{
    TRACE("Get Page Shared");
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
            TRACE("Fix it");

        lBCB = _bufMan.fix(lPID, kSHARED);
    }
    else
    {
        if(!(kNOLOCK < lBCB->getLockMode())) //check if we have at least a shared lock on BCB
            lBCB->upgradeLock(kSHARED);
    }
    return _bufMan.getFramePtr(lBCB);
}

byte* SegmentBase::getPageX(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
        lBCB = _bufMan.fix(lPID, kEXCLUSIVE);
    }
    else
    {
        if(lBCB->getLockMode() != kEXCLUSIVE) //check if we have at least a shared lock on BCB
            lBCB->upgradeLock(kEXCLUSIVE);
    }
    return _bufMan.getFramePtr(lBCB);
}

void SegmentBase::printPageToFile(uint aPageNo, bool afromDisk ) {
    //get Segment by Name
    //get physical pageNo and check if in buffer before

    byte* lPP2;
    if(!afromDisk){
        lPP2 = getPage(aPageNo, kSHARED);
    }
    else{
        lPP2 = new byte[4096];
        _partition.open();
        _partition.readPage(lPP2,_pages[aPageNo].first._pageNo,4096);
        _partition.close();
    }
    TRACE("Got the Page!");
    std::ofstream myfile;
    std::string filename = "partiton"+std::to_string(_partition.getID())+"Segment"+std::to_string(_segID)+"PageLogical"+ std::to_string(aPageNo) + ".txt";
    myfile.open(_cb.tracePath() + filename);
    uint32_t *lPP = (uint32_t *)lPP2;
    for (uint a = 0; a < 4096 / 4; ++a) {
        myfile << std::hex << *(lPP + a) << std::endl;
    }
    myfile.close();
    if(afromDisk){
        delete lPP2;
    }
    else{
        releasePage(aPageNo);
    }
}