#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock) :
    SegmentFSM(aSegID, aPartition, aControlBlock)
{
    InterpreterSP::setPageSize(aControlBlock.pageSize());  
	TRACE("SegmentFSM_SP successfully created.") ;
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition, const CB& aControlBlock) :
    SegmentFSM(aPartition, aControlBlock)
{}

SegmentFSM_SP::~SegmentFSM_SP() {}

void SegmentFSM_SP::insertTuple(byte* aTuple, const uint aTupleSize) {
	TRACE("trying to insert Tuple");
	// get page with enough space for the tuple and load it into memory
	bool emptyfix = false;
	PID lPID = getFreePage(aTupleSize, emptyfix);
	BCB* lBCB;
	if(emptyfix){//the page is new, use different command on buffer
		lBCB = _bufMan.emptyfix(lPID);
	}
	else{
		lBCB = _bufMan.fix(lPID, kEXCLUSIVE); 
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
	byte* lFreeTuplePointer = lInterpreter.addNewRecord(aTupleSize);
	
	if(lFreeTuplePointer == nullptr) // If true, not enough free space on nsm page => getFreePage buggy
	{
		const std::string lErrMsg("Not enough free space on nsm page.");
        TRACE(lErrMsg);
        throw NSMException(FLF, lErrMsg);
	}
	std::memcpy(lFreeTuplePointer, aTuple, aTupleSize); // copy the content of aTuple to the nsm page
	lInterpreter.detach();
	lBCB->setModified(true);
	lBCB->getMtx().unlock();
	_bufMan.unfix(lBCB);
    TRACE("Inserted tuple successfully.");
}

void SegmentFSM_SP::insertTuples(const byte_vpt& aTuples, const uint aTupleSize)
{
	for(byte* aTuple: aTuples)
	{
		insertTuple(aTuple, aTupleSize);
	}
}

