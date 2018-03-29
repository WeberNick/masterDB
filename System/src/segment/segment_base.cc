#include "segment_base.hh"

SegmentBase::SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, BufferManager& aBufMan) : 
	_segID(aSegID),
	_indexPages(),
  _pages(),
  _partition(aPartition),
  _BufMngr(aBufMan)
{
	if (_partition.open() == -1) { /* error handling */ }
	int lSegmentIndex = _partition.allocPage();
	_indexPages.push_back((lSegmentIndex > 0) ? (uint32_t)lSegmentIndex : 0);
	if (_partition.close() == -1) { /* error handling */ }

  //no need to init pages, will be done in store.
}

SegmentBase::SegmentBase(PartitionBase& aPartition, BufferManager& aBufMan) : 
	_segID(0),
	_indexPages(),
  _pages(),
  _partition(aPartition),
  _BufMngr(aBufMan)
{}

SegmentBase::~SegmentBase()
{}

int SegmentBase::open()
{
  if (_partition.open() == -1) { return -1; }
  return 0;
}

int SegmentBase::close()
{
  if (_partition.close() == -1) { return -1; }
  return 0;
}

int SegmentBase::readPage(byte* aPageBuffer, const uint aPageNo)
{
  if (_partition.readPage(aPageBuffer, _pages[aPageNo], getPageSize()) == -1) { return -1; }
  return 0;
}
	
int SegmentBase::writePage(const byte* aPageBuffer, const uint aPageNo)
{
  if (_partition.writePage(aPageBuffer, _pages[aPageNo], getPageSize()) == -1) { return -1; }
  return 0;
}

BCB* SegmentBase::getPageShared(uint aPageNo){
  pid la = {};
  la._fileID=_partition.getID();
  la._pageNo=aPageNo;
  
  BCB* res = _BufMngr.fix(la);
  res->getMtx().unlock();
  res->getMtx().lock_shared();
  return res;
}

BCB* SegmentBase::getPageXclusive(uint aPageNo){
  pid l = {_partition.getID(),_pages[aPageNo]};
  BCB* res=_BufMngr.fix(l);
  return res;
}

void SegmentBase::unfix(BCB* aBCB){
  _BufMngr.unfix(aBCB);
}
