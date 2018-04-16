#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock) :
    SegmentFSM(aSegID, aPartition, aControlBlock)
{
    InterpreterSP::setPageSize(aControlBlock.pageSize());    
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentFSM(aPartition, aControlBlock)
{}

SegmentFSM_SP::~SegmentFSM_SP() {}

int SegmentFSM_SP::insertTuple(byte* aTuple, const uint aTupleSize) {
	// get page with enough space for the tuple and load it into memory
	bool a = false;
	bool& emptyfix = a;
	PID lPID = getFreePage(aTupleSize,emptyfix);
	BCB* lBCB;
	if(emptyfix){//the page is new, use different command on buffer
		lBCB = BufferManager::getInstance().emptyfix(lPID);
	}
	else{
		lBCB = BufferManager::getInstance().fix(lPID, kNOLOCK); //correct lock mode?
	}
	byte* lBufferPage = BufferManager::getInstance().getFramePtr(lBCB);


	InterpreterSP lInterpreter;
	//if the page is new, it has to be initialised first.
	if(emptyfix){
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	// if enough space is free on nsm page, the pointer will point to location on page where to insert tuple
	byte* lFreeTuplePointer = lInterpreter.addNewRecord(aTupleSize);
	
	if(lFreeTuplePointer == 0) // If true, not enough free space on nsm page => getFreePage buggy
	{
		std::cerr << "If this is executed, getFreePage() does not work correctly" << std::endl;
		return -1;
	}
	std::memcpy(lFreeTuplePointer, aTuple, aTupleSize); // copy the content of aTuple to the nsm page
	lInterpreter.detach();
	lBCB->setModified(true);
	lBCB->getMtx().unlock();
	BufferManager::getInstance().unfix(lBCB);

	return 0;
}

int SegmentFSM_SP::insertTuples(const byte_vpt& aTuples, const uint aTupleSize)
{
	for(byte* aTuple: aTuples)
	{
		if(insertTuple(aTuple, aTupleSize) == -1) { return -1; }
	}
	return 0;
}

/*int SegmentFSM_SP::loadSegment(const uint32_t aPageIndex) {
    

    return -1;
}

int SegmentFSM_SP::storeSegment() {
    

    return -1;
}
*/
