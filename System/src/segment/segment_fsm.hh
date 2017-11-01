/**
 * @file    segment_fsm.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief   Class implementing Segment with Free Space Management
 * @bugs    Currently no bugs known.
 * @todos   TBD
 */

#ifndef SEGMENT_FSM_HH
#define SEGMENT_FSM_HH

#include "infra/header_structs.hh"
#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "interpreter/fsm_seg_interpreter.hh"
#include "segment_base.hh"
#include <vector>

class SegmentFSM : public SegmentBase
{
  private:
    friend class SegmentManager;
    explicit SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition);
    explicit SegmentFSM(PartitionBase &aPartition);
    SegmentFSM(const SegmentFSM &aSegment) = delete;
    SegmentFSM &operator=(const SegmentFSM &aSegment) = delete;
    ~SegmentFSM();

  public:
    int getFreePage(uint aNoOfBytes);
    int getNewPage(); // decide on whether we need this method. If not, remove from Segment_base as well.

  public:
    int storeSegment();                         // serialization
    int loadSegment(const uint32_t aPageIndex); // deserialization

  private:
    /* Free Space Management pages of this segment, indicating the remaining space of every page in _pages. */
    uint32_vt _fsmPages;
};

#endif