#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock) :
    SegmentFSM(aSegID, aPartition, aControlBlock)
{
    InterpreterSP::setPageSize(aControlBlock.pageSize());  
	TRACE("'SegmentFSM_SP' constructed");
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentFSM(aPartition, aControlBlock)
{}

TID SegmentFSM_SP::insertTuple(byte* aTuple, const uint aTupleSize) {
	TRACE("trying to insert Tuple");
    // get page with enough space for the tuple and load it into memory
	bool emptyfix = false;
	PID lPID = getFreePage(aTupleSize, emptyfix);
	BCB* lBCB;
	if(emptyfix){//the page is new, use different command on buffer
		lBCB = _bufMan.emptyfix(lPID);
	}
	else{
		lBCB = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE); 
	}
    auto it = std::find_if(_pages.begin(), _pages.end(), [lPID] (const std::pair<PID, BCB*>& elem) { return elem.first == lPID; }); //get iterator to PID in page vector
    if(it != _pages.end())
    {
        TRACE("## Page found! Assign BCB Pointer to _pages vector");
        it->second = lBCB;
    }
    else
    {
        TRACE("## This should not be printed");
    }
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	//if the page is new, it has to be initialised first.
	if(emptyfix){
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	// if enough space is free on nsm page, the pointer will point to location on page where to insert tuple
	auto [tplPtr, tplNo] = lInterpreter.addNewRecord(aTupleSize);
    const TID resultTID = {lPID.pageNo(), tplNo};
	
	if(!tplPtr) // If true, not enough free space on nsm page => getFreePage buggy
	{
		const std::string lErrMsg("Not enough free space on nsm page.");
        TRACE(lErrMsg);
        throw NSMException(FLF, lErrMsg);
	}
	std::memcpy(tplPtr, aTuple, aTupleSize); // copy the content of aTuple to the nsm page
	lInterpreter.detach();
	lBCB->setModified(true);
	_bufMan.unfix(lBCB);
    TRACE("Inserted tuple successfully.");
    return resultTID;
}

PID SegmentFSM_SP::getFreePage(const uint aNoOfBytes, bool& emptyfix) {
    return SegmentFSM::getFreePage(aNoOfBytes + sizeof(InterpreterSP::slot_t),emptyfix);
}

tid_vt SegmentFSM_SP::insertTuples(const byte_vpt& aTuples, const uint aTupleSize)
{
    tid_vt result;
	for(byte* aTuple: aTuples)
	{
		result.push_back(insertTuple(aTuple, aTupleSize));
	}
    return result;
}

void SegmentFSM_SP::loadSegmentUnbuffered(const uint32_t aPageIndex) {
    TRACE("Trying to load a Segment from Page "+std::to_string(aPageIndex)+ " on partition "+std::to_string(_partition.getID()));
    // partition and bufferManager have to be set
    size_t lPageSize = getPageSize();
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    uint32_t lnxIndex = aPageIndex;
    segment_fsm_header_t lHeader;
    fsm_header_t lHeader2;
    uint32_t l1FSM;
    _partition.open();
	while (lnxIndex != 0) {
		_partition.readPage(lPageBuffer,lnxIndex,_partition.getPageSize());
        lHeader = *(segment_fsm_header_t *)(lPageBuffer + lPageSize - sizeof(segment_fsm_header_t));
        _indexPages.push_back(lnxIndex);
        l1FSM = lHeader._firstFSM;
        _segID = lHeader._segID;
        TRACE("written in header: segID "+std::to_string(lHeader._segID) + " firstFSM: "+std::to_string(lHeader._firstFSM));
        for (uint i = 0; i < lHeader._currSize; ++i) {
            PID lTmpPID = {_partition.getID(), *(((uint32_t *)lPageBuffer) + i)};  
            _pages.push_back(page_t(lTmpPID, nullptr));
        }
        lnxIndex = lHeader._nextIndexPage;
    }
    TRACE("Load FSMs");
    _fsmPages.push_back(l1FSM);
    while (_fsmPages.at(_fsmPages.size() -1) != 0) {
        _partition.readPage(lPageBuffer,_fsmPages.at(_fsmPages.size()-1),_partition.getPageSize());
        lHeader2 = *(fsm_header_t *)(lPageBuffer + lPageSize - sizeof(fsm_header_t));
        _fsmPages.push_back(lHeader2._nextFSM);
    }
	delete[] lPageBuffer;
	_partition.close();
    TRACE("Successfully load segment.");
}
void SegmentFSM_SP::readPageUnbuffered(uint aPageNo, byte* aPageBuffer, uint aBufferSize) {
	_partition.open();
    _partition.readPage(aPageBuffer,_pages[aPageNo].first._pageNo,aBufferSize);
	TRACE(std::to_string(_pages[aPageNo].first._pageNo));
    _partition.close();
	TRACE("read page");
}

void SegmentFSM_SP::erase(){
	SegmentFSM::erase();
}
