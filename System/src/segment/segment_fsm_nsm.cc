#include "segment_fsm_nsm.hh"

SegmentFSM_NSM::SegmentFSM_NSM(const uint16_t aSegID, PartitionBase &aPartition) :
    SegmentFSM(aSegID, aPartition)
{
    
}

SegmentFSM_NSM::SegmentFSM_NSM(PartitionBase &aPartition) :
    SegmentFSM(aPartition)
{}

SegmentFSM_NSM::~SegmentFSM_NSM() {}



int SegmentFSM_NSM::loadSegment(const uint32_t aPageIndex) {
    /* to-do */

    return -1;
}

int SegmentFSM_NSM::storeSegment() {
    /* to-do */

    return -1;
}