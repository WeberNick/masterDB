#include "segment_base.hh"

/**
 * @brief Construct a new SegmentBase::SegmentBase object
 * 
 * @param aSegID 
 * @param aPartition 
 * @param aControlBlock 
 */
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
    TRACE("index page: "+std::to_string(_indexPages.at(0)));
	_partition.close();
    TRACE("'SegmentBase' constructed");
    // no need to init pages, will be done in store.
}

/** TODO
 * @brief Construct a new SegmentBase::SegmentBase object
 * 
 * @param aPartition 
 * @param aControlBlock 
 */
SegmentBase::SegmentBase(PartitionBase& aPartition, const CB& aControlBlock) : 
	_segID(0),
	_indexPages(),
    _pages(),
    _partition(aPartition),
    _bufMan(BufferManager::getInstance()),
    _cb(aControlBlock)
{}

void SegmentBase::erase()
{
    _partition.open();
    TRACE("Remove all data pages");
    for (const auto& iter : _pages)
    {
        const auto& lPID = iter.first;
        _bufMan.resetBCB(lPID);
        _partition.freePage(lPID.pageNo());
    }
    TRACE("Remove all index Pages");
    PID lPID;
    lPID._fileID=_partition.getID();
    for (auto iter : _indexPages)
    {
        lPID._pageNo=iter;
        _bufMan.resetBCB(lPID);
        _partition.freePage(iter);
    }
    _partition.close();
}

byte* SegmentBase::getPage(const uint aPageNo, LOCK_MODE aMode)
{
    if(toType(aMode) == toType(LOCK_MODE::kNOLOCK))
        return getPageF(aPageNo);
    else if(toType(aMode) == toType(LOCK_MODE::kSHARED))
        return getPageS(aPageNo);
    else if(toType(aMode) == toType(LOCK_MODE::kEXCLUSIVE))
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
    BCB*& lBCB = _pages.at(aPageNo).second; // may throw if aPageNo not in map
    if(lBCB != nullptr)
    {
        if(toType(lBCB->getLockMode()) == toType(LOCK_MODE::kEXCLUSIVE)){ lBCB->setModified(aModified); }
        _bufMan.unfix(lBCB);
    }
    lBCB = nullptr;
}

byte* SegmentBase::getPageF(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); // may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) // no valid BCB -> this segment has to request the page again
    {
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kNOLOCK);
        _pages.at(aPageNo).second = lBCB;

    }
    else if(toType(lBCB->getLockMode()) < toType(LOCK_MODE::kNOLOCK)) // should never occur
    {
        std::cerr << "BCB has lock type 'kNoType', this should not occur!" << std::endl;
        return nullptr;
    }
    return _bufMan.getFramePtr(lBCB);
}

byte* SegmentBase::getPageS(const uint aPageNo)
{
    TRACE("Get Page " + std::to_string(_pages.at(aPageNo).first.pageNo()) + " shared ");
    auto& lPair = _pages.at(aPageNo); // may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) // no valid BCB -> this segment has to request the page again
    {
        TRACE("Fix it");
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kSHARED);
        _pages.at(aPageNo).second = lBCB;
    }
    else
    {
        TRACE("still got it, in mode " + lockModeToString(lBCB->getLockMode()));
        if(toType(lBCB->getLockMode()) < toType(LOCK_MODE::kSHARED))
        { // check if we have at least a shared lock on BCB
            lBCB->upgradeLock(LOCK_MODE::kSHARED);
        }
        else if (toType(lBCB->getLockMode()) > toType(LOCK_MODE::kSHARED)){
            TRACE("you locked this page exclusively and are probably not aware of it...");
            // how do we deal with this?
        }
    }
    return _bufMan.getFramePtr(lBCB);
}

byte* SegmentBase::getPageX(const uint aPageNo)
{
    auto& lPair = _pages.at(aPageNo); // may throw if aPageNo not in map
    PID& lPID = lPair.first;
    BCB*& lBCB = lPair.second;
    if(lBCB == nullptr) // no valid BCB -> this segment has to request the page again
    {
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE);
        _pages.at(aPageNo).second = lBCB;
    }
    else
    {
        if(toType(lBCB->getLockMode()) != toType(LOCK_MODE::kEXCLUSIVE)) // check if we have at least a shared lock on BCB
        {   
            lBCB->upgradeLock(LOCK_MODE::kEXCLUSIVE); 
        }
    }
    return _bufMan.getFramePtr(lBCB);
}

void SegmentBase::printPageToFile(uint aPageNo, bool afromDisk ) {
    // get Segment by Name
    // get physical pageNo and check if in buffer before
    byte* lPP2;
    if(!afromDisk)
    {
        lPP2 = getPage(aPageNo, LOCK_MODE::kSHARED);
    }
    else
    {
        lPP2 = new byte[4096];
        _partition.open();
        _partition.readPage(lPP2,_pages[aPageNo].first._pageNo,4096);
        _partition.close();
    }
    TRACE("Got the Page!");
    std::ofstream myfile;
    std::string filename = "partiton" + std::to_string(_partition.getID()) + "Segment" + std::to_string(_segID) + "PageLogical" + std::to_string(aPageNo) + ".txt";
    myfile.open(_cb.tracePath() + filename);
    uint32_t *lPP = (uint32_t *)lPP2;
    for (uint a = 0; a < 4096 / 4; ++a) {
        myfile << std::hex << *(lPP + a) << std::endl;
    }
    myfile.close();
    if(afromDisk)
    {
        delete lPP2;
    }
    else
    {
        releasePage(aPageNo);
    }
}

std::ostream& operator<< (std::ostream& stream, const SegmentBase& aSegment)
{
    stream << aSegment.to_string();
    return stream;
}
