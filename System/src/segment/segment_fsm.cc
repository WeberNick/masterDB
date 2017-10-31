#include "segment_fsm.hh"

SegmentFSM::SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition) :
  SegmentBase(aSegID, aPartition),
  _fsmPages()
{
  if (_partition.open() == -1) { /* error handling */ }
  int lSegmentIndex = _partition.allocPage();
  _indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
  if (_partition.close() == -1) { /* error handling */ }
}

SegmentFSM::SegmentFSM(PartitionBase &aPartition) :
    SegmentBase(aPartition),
    _fsmPages()
{
  if (_partition.open() == -1) { /* error handling */ }
  int lSegmentIndex = _partition.allocPage();
  _indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
  if (_partition.close() == -1) { /* error handling */ }
}

int SegmentFSM::getFreePage(uint aNoOfBytes) {
  /* Check if page with enoug
h space is available using FF algorithm. */
  if (_partition.open() == -1) { return -1; }
  uint lPageSizeInBytes = _partition.getPageSize() - sizeof(segment_fsm_header_t);
  SegmentPageStatus lPageStatus = getPageStatus(lPageSizeInBytes, aNoOfBytes);
  // int lNoPagesToManage = (_partition.getPageSize() - sizeof(fsm_header_t)) / 8;
  if (_partition.close() == -1) { return -1; }

  if (!_fsmPages.empty()) {
    /* Find free page. */
    for (uint i = 0; i < _fsmPages.size(); ++i) {
      lFSMPage = _fsmPages[i];
      lIndex = fsmp.getFreePage(0, SegmentPageStatus)
      if (lIndex != -1) {
        return lFSMPage[lIndex];
      }
    }
    /* Create new Page. */
    if (_partition.open() == -1) { return -1; }
    int lPageIndex = _partition.allocPage();
    if (_partition.close() == -1) { return -1; }
    // init page (PAX, NSM, ..)
    if (lPageIndex != -1) {
        _fsmPages.push_back((uint32_t) lPageIndex);
        return _fsmPages.size() - 1;
    }
  } else {
    int lFSMIndex = _partition.allocPage();
    _fsmPages.push_back((lFSMIndex > 0) ? (uint32_t)lFSMIndex : 0);
    lFirstFSMPageIndex = _fsmPages[0];
    byte* lPagePointer = new byte[_pageSize];
    FSMInterpreter fsmp;
    fsmp.initNewFSM(lPagePointer, 0, lFirstFSMPageIndex, _partition.getID(), lNoPagesToManage);
    if(writePage(lPagePointer, lFirstFSMPageIndex) != 0) { return -1; }
    fsmp.detach();
    readPage(lPagePointer, 0);
    fsmp.attach(lPagePointer);
  }
  return -1;
}

int SegmentFSM::loadSegment(const uint32_t aPageIndex) {
//partition has to be set and opened
byte *lPageBuffer = new byte[_partition.getPageSize()];
size_t lPageSize = _partition.getPageSize();
uint32_t lnxIndex = aPageIndex;
segment_fsm_header_t lHeader; 
fsm_header_t lHeader2; 
uint32_t l1FSM;

while(lnxIndex!=0){
  _partition.readPage(lPageBuffer, lnxHeader, _partition.getPageSize());
  lHeader  = *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t));
  _indexPages.push_back(lnxIndex);
  l1FSM = lHeader._firstFSM;
  _segID=lHeader._segID;
  for (uint i = 0; i < lHeader._currSize; ++i) {
    _pages.push_back(*(((uint32_t *)lPageBuffer) + i));
  }
  lnxIndex = lHeader._nextIndexPage;
}
_fsmPages.push_back(l1FSM);
while(_fsmPages.at(-1)!=0){
  _partition.readPage(lPageBuffer, _fsmPages.at(-1), _partition.getPageSize());
  lHeader2  = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
  _fsmPages.push_back(lHeader2._nextFSM);
}

delete lPageBuffer;
return 0;
}

int SegmentFSM::storeSegment() {
  byte *lPageBuffer = new byte[_partition.getPageSize()];
  size_t lPageSize = _partition.getPageSize();
  uint i =0;
  uint j=0;
  uint k;
  uint managedPages = _pages.size();
  uint maxPerPage = lPageSize - sizeof(segment_fsm_header_t);
  segment_fsm_header_t lHeader;
  basic_header_t lBH;
  //create last invalid index page:
  _indexPages.push_back(0);

  //for all index pages
  while (j < _indexPages.size()-1){
    k=0;
    while(i<managedPages & k < maxPerPage){
      *(((uint32_t *)lPageBuffer) + k) = _pages.at(i); 
      ++i;
      ++k;
    }
    //create header
    // basic header: LSN, PageIndex, PartitionId, Version, unused
    lBH = {0, _indexPages.at(j), _partition.getID(), 1, 0, 0};
    // lcurrSize,  lfirstFSM; lnextIndexPage; lsegID; lversion=1; lunused =0;
    lHeader = {k,_fsmPages.at(0),_indexPages.at(j+1),_segID,1,0,lBH};
    *(segment_fsm_header_t *)(lPageBuffer + aPartition.getPageSize() - sizeof(segment_fsm_header_t)) = lHeader;
    _partition.writePage(lPageBuffer, _indexPages.at(j), lPageSize);
    ++j;
  }
  delete lPageBuffer;
  return 0;
}