#include "segment_fsm.hh"

SegmentFSM::SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentBase(aSegID, aPartition, aControlBlock),
    _fsmPages()
{
    _partition.open();
    /* PagesToManage * 2 because one byte manages two pages (4 bits for one page). */
    int lNoPagesToManage = (_partition.getPageSize() - sizeof(fsm_header_t)) * 8 / 4;
    int lSegmentIndex = _partition.allocPage();
    _indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
    //no need to init indexPage as pageHeader is written on store
    int lFSMIndex = _partition.allocPage();
    _fsmPages.push_back((lFSMIndex > 0) ? (uint32_t)lFSMIndex : 0);
    _partition.close();

    //initFSM
    PID lPID={_partition.getID(),lFSMIndex};
    BCB* lBCB = BufferManager::getInstance().emptyfix(lPID);
    byte *lPagePointer = BufferManager::getInstance().getFramePtr(lBCB);
    InterpreterFSM fsmp;
    fsmp.initNewFSM(lPagePointer, LSN, lFSMIndex, _partition.getID(), lNoPagesToManage);
    lBCB->setModified(true);
    lBCB->getMtx().unlock();
    fsmp.detach();
    InterpreterFSM::setPageSize(_cb.pageSize());
}

SegmentFSM::SegmentFSM(PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentBase(aPartition, aControlBlock), 
    _fsmPages()
{}

SegmentFSM::~SegmentFSM() {}


//returns flag if page empty or not. Partitionsobjekt evtl ersezten durch reine nummer, so selten, wie man sie jetzt noch braucht.
PID SegmentFSM::getFreePage(uint aNoOfBytes, bool& emptyfix) {
	//=====Nick: I changed the return of a physical page number into a return of a logical index====
    uint lPageSizeInBytes = getPageSize() - sizeof(fsm_header_t);
    /* PagesToManage * 2 because one byte manages two pages (4 bits for one page). */
    int lNoPagesToManage = (getPageSize() - sizeof(fsm_header_t)) * 8 / 4;

    /* Check if page with enough space is available using FF algorithm. */
    byte *lPagePointer;
    InterpreterFSM fsmp;
    emptyfix=false;
    PID lPID;
    BCB* lBcb;
    
    for (uint i = 0; i < _fsmPages.size(); ++i) {
        int lFSMPage = _fsmPages[i];
        lPID = {_partition.getID(),lFSMPage};
        lBcb = BufferManager::getInstance().fix(lPID, kNOLOCK); //CORRECT LOCK MODE?
        lPagePointer = BufferManager::getInstance().getFramePtr(lBcb);
        fsmp.attach(lPagePointer);
        PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);

        int lIndex = fsmp.getFreePage(lPageStatus);
        if (lIndex != -1) {
            fsmp.detach();
            /* If this is the case: alloc new page, add to _pages and return that index. (occurs if page does not exist yet, otherwise the page already exists) */
            if (lIndex > _pages.size()) {
                int lPageIndex = _partition.allocPage();
                //_pages.push_back(lPageIndex);
                //fix this for new data structure
                emptyfix=true;
                lBcb->setModified(true);
                lBcb->getMtx().unlock();
                BufferManager::getInstance().unfix(lBcb);
                //return _pages.size() - 1;
                return PID{0,0}; //fix this
            } else {
                lBcb->setModified(true);
                lBcb->getMtx().unlock();
                BufferManager::getInstance().unfix(lBcb);
                //return (i * lNoPagesToManage) + lIndex;
                return PID{0,0}; //fix this
            }
        }
        //if lIndex == -1
    }
    /* No FSM page found that returns a free page, create a new one. */
    int lFSMIndex = _partition.allocPage();
    if (lFSMIndex == -1) { return /*-1*/ PID{0,0}; } //fix this. -1 for invalid value? throw error
    _fsmPages.push_back((uint32_t)lFSMIndex);

    /* Insert NextFSM to header of current FSM, which is still loaded and locked. */
   // PID lPID = {_partition.getID(),_fsmPages[_fsmPages.size() - 2]};
    //BCB* lBcb = _bufMngr.fix(lPID);
    //byte* lPP = BufferManager::getInstance().getFramePtr(lBcb);
    (*((fsm_header_t*) (lPagePointer + getPageSize() - sizeof(fsm_header_t) )))._nextFSM = lFSMIndex;
    lBcb->setModified(true);
    lBcb->getMtx().unlock();
    BufferManager::getInstance().unfix(lBcb);
    fsmp.detach();
    
    lPID._pageNo=lFSMIndex;
    lBcb = BufferManager::getInstance().emptyfix(lPID);
    lPagePointer = BufferManager::getInstance().getFramePtr(lBcb);
    fsmp.attach(lPagePointer);

    PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);
    int lFreePageIndex = fsmp.getFreePage(lPageStatus);

    fsmp.detach();
    lBcb->setModified(true);
    lBcb->getMtx().unlock();
    BufferManager::getInstance().unfix(lBcb);
    emptyfix=true;
//    return ((_fsmPages.size() - 1) * lNoPagesToManage) + lFreePageIndex;
    return PID{0,0}; //fix this
}

PID SegmentFSM::getNewPage() {
    bool a = true;
    bool& emptyfix = a;
    //return getFreePage(getPageSize() - sizeof(segment_fsm_header_t) ,emptyfix);
    return PID{0,0}; //INVALID: CHANGE CORRECTLY
   /*     //reserve new page
    if (_partition.open() == -1) { return -1; }
    uint lPage = _partition.allocPage();
    if (lPage == -1) { return -1; }
    //check for exceptions: if segmentIndexPage oder FSM is full, page has a problem, but this will be dealt with later
    _pages.push_back(lPage);
    //set new fsm entry to full, but should be done otherwise
    uint lFSM = _fsmPages[_fsmPages.size()-2];
    InterpreterFSM fsmp;
    byte* lPP = new byte[_partition.getPageSize()];
    _partition.readPage(lPP,lFSM,_partition.getPageSize());
    fsmp.attach(lPP);
    uint lPos = fsmp.getHeaderPtr()->_noPages;
    if (lPos+1 > ((_partition.getPageSize()-size_of(fsm_header_t))/2) ){
        //add new fsm
    }
    else{
        fsmp.changePageStatus()
    }
    //return page
    */
}

int SegmentFSM::loadSegment(const uint32_t aPageIndex) {
    // partition and bufferManager have to be set
    size_t lPageSize = getPageSize();
    byte *lPageBuffer;
    uint32_t lnxIndex = aPageIndex;
    segment_fsm_header_t lHeader;
    fsm_header_t lHeader2;
    uint32_t l1FSM;
    PID lPID;
    BCB* lBCB;

    while (lnxIndex != 0) {
        lPID =  {_partition.getID(),lnxIndex};
        lBCB = BufferManager::getInstance().fix(lPID, kNOLOCK); //IS THIS CORRECT LOCK MODE??
        lBCB->getMtx().unlock();
        lBCB->getMtx().lock_shared();
        lPageBuffer = BufferManager::getInstance().getFramePtr(lBCB);
        lHeader = *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t));
        _indexPages.push_back(lnxIndex);
        l1FSM = lHeader._firstFSM;
        _segID = lHeader._segID;
        for (uint i = 0; i < lHeader._currSize; ++i) {
            //_pages.push_back(*(((uint32_t *)lPageBuffer) + i));
            //fix this for new data structure
        }
        lnxIndex = lHeader._nextIndexPage;
        lBCB->getMtx().unlock_shared();
        BufferManager::getInstance().unfix(lBCB);
    }
    _fsmPages.push_back(l1FSM);
    while (_fsmPages.at(_fsmPages.size() -1) != 0) {
        lPID._pageNo = _fsmPages.at(_fsmPages.size() -1);
        lBCB = BufferManager::getInstance().fix(lPID, kNOLOCK); //IS THIS CORRECT LOCK MODE?
        lBCB->getMtx().unlock();
        lBCB->getMtx().lock_shared();
        lPageBuffer = BufferManager::getInstance().getFramePtr(lBCB);
        lHeader2 = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
        _fsmPages.push_back(lHeader2._nextFSM);
        lBCB->getMtx().unlock_shared();
        BufferManager::getInstance().unfix(lBCB);
    }

    return 0;
}

int SegmentFSM::storeSegment() {
    size_t lPageSize = getPageSize();
    byte *lPageBuffer;
    uint i = 0;
    uint j = 0;
    uint k;
    uint managedPages = _pages.size();
    uint maxPerPage = lPageSize - sizeof(segment_fsm_header_t);
    segment_fsm_header_t lHeader;
    basic_header_t lBH;
    PID lPID;
    BCB* lBCB;
    // create last invalid index page:
    _indexPages.push_back(0);

    // for all index pages
    while (j < _indexPages.size() - 1) {
        lPID={_partition.getID(),_indexPages.at(j)};
        lBCB = BufferManager::getInstance().fix(lPID, kNOLOCK); //CORRECT LOCK MODE??
        lPageBuffer = BufferManager::getInstance().getFramePtr(lBCB);
        k = 0;
        while ((i < managedPages) & (k < maxPerPage)) {
            //*(((uint32_t *)lPageBuffer) + k) = _pages.at(i);
            //fix for new data structure
            ++i;
            ++k;
        }
        // Create header
        // Basic header: LSN, PageIndex, PartitionId, Version, Unused
        lBH = {0, _indexPages.at(j), _partition.getID(), 1, 0, 0};
        // lCurrSize,  lFirstFSM; lNextIndexPage; lSegID; lVersion = 1; lUnused = 0;
        lHeader = {k, _fsmPages.at(0), _indexPages.at(j + 1), _segID, 1, 0, lBH};
        *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t)) = lHeader;
        lBCB->setModified(true);
        lBCB->getMtx().unlock();
        BufferManager::getInstance().unfix(lBCB);
        ++j;
    }
    return 0;
}
