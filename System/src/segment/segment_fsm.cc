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
  /* Check if page with enough space is available using FF algorithm. */
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

}

int SegmentFSM::storeSegment() {

}