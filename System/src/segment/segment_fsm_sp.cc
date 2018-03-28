#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase &aPartition, BufferManager& aBufMan) :
    SegmentFSM(aSegID, aPartition, aBufMan)
{
    
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition, BufferManager& aBufMan) :
    SegmentFSM(aPartition,aBufMan)
{}

SegmentFSM_SP::~SegmentFSM_SP() {}

int SegmentFSM_SP::insertTuple(byte* aTuple, const uint aTupleSize) {
	// get page with enough space for the tuple and load it into memory
	bool a = false;
	bool& emptyfix = a;
	pid lPID ={_partition.getID(), getFreePage(aTupleSize,emptyfix)};
	BCB* lBCB;
	if(emptyfix){//the page is new, use different command on buffer
		lBCB = _BufMngr.emptyfix(lPID);
	}
	else{
		lBCB = _BufMngr.fix(lPID);
	}
	byte* lBufferPage = _BufMngr.getFramePtr(lBCB);


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
	_BufMngr.unfix(lBCB);

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