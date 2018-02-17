#include "segment_fsm_sp.hh"

SegmentFSM_SP::SegmentFSM_SP(const uint16_t aSegID, PartitionBase &aPartition) :
    SegmentFSM(aSegID, aPartition)
{
    
}

SegmentFSM_SP::SegmentFSM_SP(PartitionBase &aPartition) :
    SegmentFSM(aPartition)
{}

SegmentFSM_SP::~SegmentFSM_SP() {}

int SegmentFSM_SP::insertTuples() {
    /* block in seitenkonforme segmente packen
    wenn auf eine seite passt get free page speichern
    nein auf mehrere seiten hintereinander */
    // pointer verschieben und schreiben
}

int SegmentFSM_SP::loadSegment(const uint32_t aPageIndex) {
    /* to-do */

    return -1;
}

int SegmentFSM_SP::storeSegment() {
    /* to-do */

    return -1;
}