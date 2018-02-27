#include "segment_fsm.hh"

SegmentFSM::SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition) :
    SegmentBase(aSegID, aPartition),
    _fsmPages()
{
    _partition.open();
    /* PagesToManage * 2 because one byte manages two pages (4 bits for one page). */
    int lNoPagesToManage = (_partition.getPageSize() - sizeof(fsm_header_t)) * 8 / 4;
    int lSegmentIndex = _partition.allocPage();
    _indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
    int lFSMIndex = _partition.allocPage();
    _fsmPages.push_back((lFSMIndex > 0) ? (uint32_t)lFSMIndex : 0);

    byte *lPagePointer = new byte[_partition.getPageSize()];
    InterpreterFSM fsmp;
    fsmp.initNewFSM(lPagePointer, LSN, lFSMIndex, _partition.getID(), lNoPagesToManage);
    _partition.writePage(lPagePointer, lFSMIndex, _partition.getPageSize());
    fsmp.detach();
    delete[] lPagePointer;
    _partition.close();
}

SegmentFSM::SegmentFSM(PartitionBase &aPartition) :
    SegmentBase(aPartition), 
    _fsmPages()
{}

SegmentFSM::~SegmentFSM() {}

int SegmentFSM::getFreePage(uint aNoOfBytes) {
	//=====Nick: I changed the return of a physical page number into a return of a logical index====
    uint lPageSizeInBytes = getPageSize() - sizeof(fsm_header_t);
    /* PagesToManage * 2 because one byte manages two pages (4 bits for one page). */
    int lNoPagesToManage = (getPageSize() - sizeof(fsm_header_t)) * 8 / 4;

    /* Check if page with enough space is available using FF algorithm. */
    byte *lPagePointer = new byte[getPageSize()];
    InterpreterFSM fsmp;
    fsmp.detach();
    fsmp.attach(lPagePointer);

    for (uint i = 0; i < _fsmPages.size(); ++i) {
        int lFSMPage = _fsmPages[i];

		if(readPage(lPagePointer, lFSMPage) == -1){ return -1; }
        PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);

        int lIndex = fsmp.getFreePage(lPageStatus);
        if (lIndex != -1) {
            fsmp.detach();
            delete[] lPagePointer;
            /* If this is the case: alloc new page, add to _pages and return that index. (occurs if page does not exist yet, otherwise the page already exists) */
            if (lIndex > _pages.size()) {
                if(open() == -1) { return -1; }
                int lPageIndex = _partition.allocPage();
                if (lPageIndex == -1) { return -1; }
                _pages.push_back(lPageIndex);
                if (close() == -1) { return -1; }
                return _pages.size() - 1;
            } else {
                return (i * lNoPagesToManage) + lIndex;
            }
        }
    }
    /* No FSM page found that returns a free page, create a new one. */
    if (open() == -1) { return -1; }
    int lFSMIndex = _partition.allocPage();
    if (lFSMIndex == -1) { return -1; }
    _fsmPages.push_back((uint32_t)lFSMIndex);

    /* Insert NextFSM to header of current FSM. */
    byte* lPP = new byte[getPageSize()];
    if(readPage(lPP, _fsmPages[_fsmPages.size() - 2]) == -1){ return -1; }
    (*((fsm_header_t*) (lPP + getPageSize() - sizeof(fsm_header_t) )))._nextFSM = lFSMIndex;
    if(writePage(lPP, _fsmPages[_fsmPages.size() - 2]) == -1){ return -1; }
    delete[] lPP;

	if(writePage(lPagePointer, lFSMIndex) == -1){ return -1; } 
    fsmp.detach();
    if(readPage(lPagePointer, lFSMIndex) == -1){ return -1; }
    fsmp.attach(lPagePointer);

    PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);
    int lFreePageIndex = fsmp.getFreePage(lPageStatus);

    fsmp.detach();
    delete[] lPagePointer;
    if(close() == -1) { return -1; }
    return ((_fsmPages.size() - 1) * lNoPagesToManage) + lFreePageIndex;
}

int SegmentFSM::getNewPage() {
    return getFreePage(getPageSize() - sizeof(segment_fsm_header_t) );
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
    // partition has to be set and opened
    size_t lPageSize = getPageSize();
    byte *lPageBuffer = new byte[lPageSize];
    uint32_t lnxIndex = aPageIndex;
    segment_fsm_header_t lHeader;
    fsm_header_t lHeader2;
    uint32_t l1FSM;

    while (lnxIndex != 0) {
        if(readPage(lPageBuffer, lnxIndex) == -1){ return -1; }
        lHeader = *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t));
        _indexPages.push_back(lnxIndex);
        l1FSM = lHeader._firstFSM;
        _segID = lHeader._segID;
        for (uint i = 0; i < lHeader._currSize; ++i) {
            _pages.push_back(*(((uint32_t *)lPageBuffer) + i));
        }
        lnxIndex = lHeader._nextIndexPage;
    }
    _fsmPages.push_back(l1FSM);
    while (_fsmPages.at(_fsmPages.size() -1) != 0) {
        if(readPage(lPageBuffer, _fsmPages.at(_fsmPages.size() -1)) == -1){ return -1; }
        lHeader2 = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
        _fsmPages.push_back(lHeader2._nextFSM);
    }

    delete lPageBuffer;
    return 0;
}

int SegmentFSM::storeSegment() {
    size_t lPageSize = getPageSize();
    byte *lPageBuffer = new byte[lPageSize];
    uint i = 0;
    uint j = 0;
    uint k;
    uint managedPages = _pages.size();
    uint maxPerPage = lPageSize - sizeof(segment_fsm_header_t);
    segment_fsm_header_t lHeader;
    basic_header_t lBH;
    // create last invalid index page:
    _indexPages.push_back(0);

    // for all index pages
    while (j < _indexPages.size() - 1) {
        k = 0;
        while ((i < managedPages) & (k < maxPerPage)) {
            *(((uint32_t *)lPageBuffer) + k) = _pages.at(i);
            ++i;
            ++k;
        }
        // Create header
        // Basic header: LSN, PageIndex, PartitionId, Version, Unused
        lBH = {0, _indexPages.at(j), _partition.getID(), 1, 0, 0};
        // lCurrSize,  lFirstFSM; lNextIndexPage; lSegID; lVersion = 1; lUnused = 0;
        lHeader = {k, _fsmPages.at(0), _indexPages.at(j + 1), _segID, 1, 0, lBH};
        *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t)) = lHeader;
        if(writePage(lPageBuffer, _indexPages.at(j)) == -1){ return -1; }
        ++j;
    }
    delete lPageBuffer;
    return 0;
}
