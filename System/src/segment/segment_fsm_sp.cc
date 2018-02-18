#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase &aPartition) :
    SegmentFSM(aSegID, aPartition)
{
    
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition) :
    SegmentFSM(aPartition)
{}

SegmentFSM_SP::~SegmentFSM_SP() {}

int SegmentFSM_SP::insertTuple(byte* aTuple, const uint aTupleSize) {
	byte* lBufferPage = new byte[_partition.getPageSize()];

	if(readPage(lBufferPage, getFreePage(aTupleSize)) == -1){ return -1; }
	SP_Interpreter lInterpreter;
	lInterpreter.attach(lBufferPage);
	byte* lFreeTuplePointer = lInterpreter.addNewRecord(aTupleSize);
	if(lFreeTuplePointer == 0) //If true, not enough free space on nsm page => getFreePage buggy
	{
		std::cerr << "If this is executed, getFreePage() does not work correctly" << std::endl;
		return -1;
	}
	std::memcpy(lFreeTuplePointer, aTuple, aTupleSize); //copy the content of aTuple to the nsm page
	return 0;
}

int SegmentFSM_SP::loadSegment(const uint32_t aPageIndex) {
    /* to-do */

    return -1;
}

int SegmentFSM_SP::storeSegment() {
    /* to-do */

    return -1;
}
