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
    BCB*& lBCB = _pages.at(aPageNo).second; //may throw if aPageNo not in map
    if(lBCB != nullptr)
    {
        lBCB->setModified(true);
        _bufMan.flush(lBCB);    
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
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
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
