#include "segment_base.hh"

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock) : 
	_segID(aSegID),
	_indexPages(),
    _pages(),
    _partition(aPartition),
    _cb(aControlBlock)
{
	if (_partition.open() == -1) { /* error handling */ }
	int lSegmentIndex = _partition.allocPage();
	_indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
	if (_partition.close() == -1) { /* error handling */ }

  //no need to init pages, will be done in store.
}

SegmentBase::SegmentBase(PartitionBase& aPartition, const CB& aControlBlock) : 
	_segID(0),
	_indexPages(),
    _pages(),
    _partition(aPartition),
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
        return getPageX($`aPageNo`);
    else 
        return nullptr;
}

void SegmentBase::writePage(const uint aPageNo)
{
    BCB*& lBCB = _pages.at(aPageNo).second; //may throw if aPageNo not in map
    if(lBCB != nullptr)
    {
        BufferManager::getInstance().flush(lBCB);    
    }
}

void SegmentBase::releasePage(const uint aPageNo)
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
                lBCB->getMtx().unlock();
                break;
            default:
                const std::string lErrMsg("Lock type not supported");
 55             if(_cb.trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
                throw SwitchException(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg);
                break;    
        }
        BufferManager::getInstance().unfix(lBCB);
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
        lBCB = BufferManager::getInstance().fix(lPID, kNOLOCK);
    }
    else if(lBCB->getLockMode() < kNOLOCK) //should never occur
    {
        std::cerr << "BCB has lock type 'kNoType', this should not occur!" << std::endl;
        return nullptr;
    }
    return BufferManager::getInstance().getFramePtr(lBCB);
}

byte* SegmentBase::getPageS(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
        lBCB = BufferManager::getInstance().fix(lPID, kSHARED);
    }
    else
    {
        if(!(kNOLOCK < lBCB->getLockMode())) //check if we have at least a shared lock on BCB
            lBCB->upgradeLock(kSHARED);
    }
    return BufferManager::getInstance().getFramePtr(lBCB);
}

byte* SegmentBase::getPageX(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); //may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) //no valid BCB -> this segment has to request the page again
    {
        lBCB = BufferManager::getInstance().fix(lPID, kEXCLUSIVE);
    }
    else
    {
        if(lBCB->getLockMode() != kEXCLUSIVE) //check if we have at least a shared lock on BCB
            lBCB->upgradeLock(kEXCLUSIVE);
    }
    return BufferManager::getInstance().getFramePtr(lBCB);
}
