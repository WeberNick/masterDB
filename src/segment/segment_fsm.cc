#include "segment_fsm.hh"

SegmentFSM::SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentBase(aSegID, aPartition, aControlBlock),
    _fsmPages()
{
    _partition.open();
    // PagesToManage * 2 because one byte manages two pages (4 bits for one page).
    uint32_t lNoPagesToManage = (_partition.getPageSize() - sizeof(fsm_header_t)) * 8 / 4;
    
    // no need to alloc index page as this is done by constructor of SegmentBase
    // alloc FSM
    uint lFSMIndex = _partition.allocPage();
    _fsmPages.push_back(lFSMIndex);
    _partition.close();

    // init FSM using the buffer
    PID lPID = {_partition.getID(), lFSMIndex};
    BCB* lBCB = _bufMan.emptyfix(lPID);
    byte *lPagePointer = _bufMan.getFramePtr(lBCB);
    InterpreterFSM fsmp;
    fsmp.initNewFSM(lPagePointer, LSN, lFSMIndex, _partition.getID(), lNoPagesToManage);
    lBCB->setModified(true);
    _bufMan.unfix(lBCB);
    fsmp.detach();
    InterpreterFSM::setPageSize(_cb.pageSize());
    TRACE("'SegmentFSM' constructed ()");
}

SegmentFSM::SegmentFSM(PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentBase(aPartition, aControlBlock), 
    _fsmPages()
{}

void SegmentFSM::erase()
{
    _partition.open();
    PID lPID;
    lPID._fileID=_partition.getID();
    // remove all fsms
    for (auto iter : _fsmPages){
        lPID._pageNo=iter;
        _bufMan.resetBCB(lPID);
        _partition.freePage(iter);
    }
    _partition.close();
    // remove all index and data pages
    SegmentBase::erase();
}

// returns flag if page empty or not.
PID SegmentFSM::getFreePage(const uint aNoOfBytes, bool& emptyfix, uint aSizeOfOverhead) {
    TRACE("Request for a page with " + std::to_string(aNoOfBytes) + " Bytes free");
    const uint lPageSizeInBytes = getPageSize() - aSizeOfOverhead;
    /* Check if page with enough space is available using FF algorithm. */
    if(aNoOfBytes > lPageSizeInBytes){
        std::string errMsg = "requested more space than the size of a page.";
        TRACE(errMsg);
        throw NSMException(FLF,errMsg);
    }
    byte *lPagePointer = nullptr;
    InterpreterFSM fsmp;
    emptyfix = false;
    PID lPID;
    lPID._fileID = _partition.getID();
    BCB* lBcb;
    // for every FSM page
    for (size_t i = 0; i < _fsmPages.size(); ++i) {
        // load it
        uint32_t lFSMPage = _fsmPages[i];
        lPID._pageNo = lFSMPage;
        lBcb = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE); 
        lPagePointer = _bufMan.getFramePtr(lBcb);
        fsmp.attach(lPagePointer);
        // page status is global, but can only be calculated by an attached Interpreter.
        PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);
        
        if (lPageStatus == PageStatus::kNoType){
            // something went completely wrong, this exception should have been caught before.
            // but stays here for safety.
            std::string errMsg = "asked the interpreter for more space than possible.";
            TRACE(errMsg);
            _bufMan.unfix(lBcb);
            throw NSMException(FLF,errMsg);
        }

        // get a free page
        uint32_t lIndex = fsmp.getFreePage(lPageStatus);

        if (lIndex != INVALID_32) {
            if (lIndex + i * fsmp.getMaxPagesPerFSM() >= _pages.size()) {
                // If this is the case: alloc new page, add to _pages and return that index. (occurs if page does not exist yet)

                _partition.open();
                uint lPageIndex = _partition.allocPage();
                lPID._pageNo = lPageIndex;
                _pages.push_back(std::pair<PID, BCB*>(lPID, nullptr));
                // if there is more space needed to store page indeces at shutdown, reserve another IndexPage
                if(_pages.size() >= _indexPages.size() * (getPageSize()  - sizeof(segment_fsm_header_t)))
                { 
                    uint lIndexPage = _partition.allocPage();
                    _indexPages.push_back(lIndexPage);
                }

                _partition.close();
                emptyfix=true;
                lBcb->setModified(true);
                _bufMan.unfix(lBcb);
                TRACE("successfully found a free page. on existing FSM but created new one");
                return lPID; 
            } else {
                // there is a page with sufficient free space already allocated
                lPID._pageNo = _pages.at(i * fsmp.getMaxPagesPerFSM() + lIndex).first._pageNo;
                lBcb->setModified(true);
                _bufMan.unfix(lBcb);
                TRACE("successfully found a free page. existing page");
                return lPID;
            }
        } // if lIndex == INVALID
        fsmp.detach();
        if(i != _fsmPages.size() -1)
            _bufMan.unfix(lBcb); 
    }
    // No FSM page found that returns a free page, create a new one.
    // alloc next FSM page
    _partition.open();
    uint32_t lFSMIndex = _partition.allocPage();
    _fsmPages.push_back(lFSMIndex);

    /* Insert NextFSM to header of current FSM, which is still loaded and locked. */
    (*((fsm_header_t*) (lPagePointer + getPageSize() - sizeof(fsm_header_t) )))._nextFSM = lFSMIndex;
    lBcb->setModified(true);
    _bufMan.unfix(lBcb);
    // create next FSM page
    lPID._pageNo = lFSMIndex;
    lBcb = _bufMan.emptyfix(lPID);
    lPagePointer = _bufMan.getFramePtr(lBcb);
    fsmp.attach(lPagePointer);
    fsmp.initNewFSM(lPagePointer, 0, lFSMIndex, _partition.getID(), fsmp.getMaxPagesPerFSM());
    // find next free page
    //PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);
    //uint32_t lFreePageIndex = fsmp.getFreePage(lPageStatus);

    lBcb->setModified(true);
    _bufMan.unfix(lBcb);
    emptyfix=true;

    // alloc next page which obviously does not exist by now.
    uint lPageIndex = _partition.allocPage();
    lPID._pageNo = lPageIndex;
    _pages.push_back(std::pair<PID, BCB*>(lPID, nullptr));

    // check if index Page overflows.
    if(_pages.size() >= _indexPages.size() * (getPageSize()  - sizeof(segment_fsm_header_t))) // if there is more space needed to store stuff in the end, reserve another IndexPage
    { 
        uint lIndexPage = _partition.allocPage();
        _indexPages.push_back(lIndexPage);
    }
    _partition.close();
    
    fsmp.detach();
    TRACE("Successfully found free page on segment. created new fsm and new page.");
    return lPID;
}

PID SegmentFSM::getNewPage()
{
    bool emptyfix = true;
    // doing this without marking the whole page as full had way to many border cases.
    TRACE("Successfully got new page on segment.");
    return getFreePage(getPageSize() - sizeof(basic_header_t), emptyfix);
}
   /*     // reserve new page
   // had way to many border cases to deal with.
    if (_partition.open() == -1) { return -1; }
    uint lPage = _partition.allocPage();
    if (lPage == -1) { return -1; }
    // check for exceptions: if segmentIndexPage oder FSM is full, page has a problem, but this will be dealt with later
    _pages.push_back(lPage);
    // set new fsm entry to full, but should be done otherwise
    uint lFSM = _fsmPages[_fsmPages.size()-2];
    InterpreterFSM fsmp;
    byte* lPP = new byte[_partition.getPageSize()];
    _partition.readPage(lPP,lFSM,_partition.getPageSize());
    fsmp.attach(lPP);
    uint lPos = fsmp.getHeaderPtr()->_noPages;
    if (lPos+1 > ((_partition.getPageSize()-size_of(fsm_header_t))/2) ){
        // add new fsm
    }
    else{
        fsmp.changePageStatus()
    }
    // return page
    */

void SegmentFSM::loadSegment(const uint32_t aPageIndex) {
    TRACE("Trying to load a Segment from Page "+std::to_string(aPageIndex)+ " on partition "+std::to_string(_partition.getID())+" located at "+_partition.getPath());
    // partition and bufferManager have to be set
    size_t lPageSize = getPageSize();
    byte *lPageBuffer;
    uint32_t lnxIndex = aPageIndex;
    segment_fsm_header_t lHeader; // header of fsm index page
    fsm_header_t lHeader2;        // header of the fsm pages
    uint32_t l1FSM;               // index of first fsm. stored at every index page.
    PID lPID;
    BCB* lBCB;
    // load index Pages
    while (lnxIndex != 0) {// next Index is 0 at last index page only.
        lPID =  {_partition.getID(),lnxIndex};
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kSHARED);
        lPageBuffer = _bufMan.getFramePtr(lBCB);
        lHeader = *((segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t)));
        _indexPages.push_back(lnxIndex);
        l1FSM = lHeader._firstFSM;
        _segID = lHeader._segID;
        for (uint i = 0; i < lHeader._currSize; ++i)
        {
            PID lTmpPID = {_partition.getID(), *(((uint32_t *)lPageBuffer) + i)};  
            _pages.push_back(page_t(lTmpPID, nullptr));
        }
        lnxIndex = lHeader._nextIndexPage;
        _bufMan.unfix(lBCB);
    }
    _fsmPages.push_back(l1FSM);
    while (_fsmPages.at(_fsmPages.size() -1) != 0)
    {
        lPID._pageNo = _fsmPages.at(_fsmPages.size() -1);
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kSHARED);
        lPageBuffer = _bufMan.getFramePtr(lBCB);
        lHeader2 = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
        _fsmPages.push_back(lHeader2._nextFSM);
        _bufMan.unfix(lBCB);
    }
    // last one will push a 0 onto _fsmPages. It is deleted again.
    if(_fsmPages.at(_fsmPages.size()-1)==0){
        _fsmPages.erase(_fsmPages.end()-1);
    }
    
    TRACE("Successfully loaded segment.");
}

void SegmentFSM::storeSegment()
{
    size_t lPageSize = getPageSize();
    byte *lPageBuffer;
    uint i = 0; // counter for data pages
    uint j = 0; // counter for index Pages
    uint k;     // counter on individual page
    uint managedPages = _pages.size();
    uint maxPerPage = (lPageSize - sizeof(segment_fsm_header_t)) / sizeof(uint32_t);
    segment_fsm_header_t lHeader;
    basic_header_t lBH;
    PID lPID;
    BCB* lBCB;
    // create last invalid index page:
    _indexPages.push_back(0);

    // for all index pages
    while (j < _indexPages.size() - 1) {
        lPID={_partition.getID(),_indexPages.at(j)};
        lBCB = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE); 
        lPageBuffer = _bufMan.getFramePtr(lBCB);
        k = 0;
        while ((i < managedPages) & (k < maxPerPage))
        {
            *(((uint32_t *)lPageBuffer) + k) = _pages.at(i).first.pageNo();
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
        _bufMan.unfix(lBCB);
        ++j;
    }
    TRACE("Successfully stored segment.");
}
