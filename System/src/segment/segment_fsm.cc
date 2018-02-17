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
    FSMInterpreter fsmp;
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
    uint lPageSizeInBytes = _partition.getPageSize() - sizeof(fsm_header_t);
    /* PagesToManage * 2 because one byte manages two pages (4 bits for one page). */
    int lNoPagesToManage = (_partition.getPageSize() - sizeof(fsm_header_t)) * 8 / 4;

    /* Check if page with enough space is available using FF algorithm. */
    byte *lPagePointer = new byte[_partition.getPageSize()];
    FSMInterpreter fsmp;
    fsmp.detach();
    fsmp.attach(lPagePointer);

    for (uint i = 0; i < _fsmPages.size(); ++i) {
        int lFSMPage = _fsmPages[i];

        _partition.readPage(lPagePointer, lFSMPage, _partition.getPageSize());
        PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);

        int lIndex = fsmp.getFreePage(lPageStatus);
        if (lIndex != -1) {
            fsmp.detach();
            delete[] lPagePointer;
            /* If this is the case: alloc new page, add to _pages and return that index. (occurs if page does not exist yet, otherwise the page already exists) */
            if ( ((uint32_t)lIndex) > _pages.size()) {
                if(_partition.open() == -1) { return -1; }
                int lPageIndex = _partition.allocPage();
                if (lPageIndex == -1) { return -1; }
                _pages.push_back(lPageIndex);
                if (_partition.close() == -1) { return -1; }
                return _pages[_pages.size() - 1];
            } else {
                return _pages[(i * lNoPagesToManage) + lIndex];
            }
        }
    }
    /* No FSM page found that returns a free page, create a new one. */
    if (_partition.open() == -1) { return -1; }
    int lFSMIndex = _partition.allocPage();
    if (lFSMIndex == -1) { return -1; }
    _fsmPages.push_back((uint32_t)lFSMIndex);

    /* Insert NextFSM to header of current FSM. */
    byte* lPP = new byte[_partition.getPageSize()];
    _partition.readPage(lPP, _fsmPages[_fsmPages.size() - 2], _partition.getPageSize());
    (*((fsm_header_t*) (lPP + _partition.getPageSize() - sizeof(fsm_header_t) )))._nextFSM = lFSMIndex;
    _partition.writePage(lPP, _fsmPages[_fsmPages.size() - 2], _partition.getPageSize());
    delete[] lPP;

    _partition.writePage(lPagePointer, lFSMIndex, _partition.getPageSize());
    fsmp.detach();
    _partition.readPage(lPagePointer, lFSMIndex, _partition.getPageSize());
    fsmp.attach(lPagePointer);

    PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes, aNoOfBytes);
    int lFreePageIndex = fsmp.getFreePage(lPageStatus);

    fsmp.detach();
    delete[] lPagePointer;
    if (_partition.close() == -1) { return -1; }
    return _pages[((_fsmPages.size() - 1) * lNoPagesToManage) + lFreePageIndex];
}

int SegmentFSM::getNewPage() {
    return getFreePage( _partition.getPageSize() -sizeof(segment_fsm_header_t) );
   /*     //reserve new page
    if (_partition.open() == -1) { return -1; }
    uint lPage = _partition.allocPage();
    if (lPage == -1) { return -1; }
    //check for exceptions: if segmentIndexPage oder FSM is full, page has a problem, but this will be dealt with later
    _pages.push_back(lPage);
    //set new fsm entry to full, but should be done otherwise
    uint lFSM = _fsmPages[_fsmPages.size()-2];
    FSMInterpreter fsmp;
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
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    size_t lPageSize = _partition.getPageSize();
    uint32_t lnxIndex = aPageIndex;
    segment_fsm_header_t lHeader;
    fsm_header_t lHeader2;
    uint32_t l1FSM;

    while (lnxIndex != 0) {
        _partition.readPage(lPageBuffer, lnxIndex, _partition.getPageSize());
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
        _partition.readPage(lPageBuffer, _fsmPages.at(_fsmPages.size() -1), _partition.getPageSize());
        lHeader2 = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
        _fsmPages.push_back(lHeader2._nextFSM);
    }

    delete lPageBuffer;
    return 0;
}

int SegmentFSM::storeSegment() {
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    size_t lPageSize = _partition.getPageSize();
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
        // create header
        // basic header: LSN, PageIndex, PartitionId, Version, unused
        lBH = {0, _indexPages.at(j), _partition.getID(), 1, 0, 0};
        // lcurrSize,  lfirstFSM; lnextIndexPage; lsegID; lversion=1; lunused =0;
        lHeader = {k, _fsmPages.at(0), _indexPages.at(j + 1), _segID, 1, 0, lBH};
        *(segment_fsm_header_t *)(lPageBuffer + _partition.getPageSize() - sizeof(segment_fsm_header_t)) = lHeader;
        _partition.writePage(lPageBuffer, _indexPages.at(j), lPageSize);
        ++j;
    }
    delete lPageBuffer;
    return 0;
}
