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
#include "segment_base.hh"
#include "interpreter/interpreter_fsm.hh"
#include "buffer/buf_cntrl_block.hh"
#include "buffer/buf_mngr.hh"
#include <vector>

class SegmentFSM : public SegmentBase
{
  protected:
    friend class SegmentManager;
    explicit SegmentFSM(const uint16_t aSegID, PartitionBase &aPartition, BufferManager& aBufMan);
    explicit SegmentFSM(PartitionBase &aPartition, BufferManager& aBufMan);
    SegmentFSM(const SegmentFSM &aSegment) = delete;
    SegmentFSM &operator=(const SegmentFSM &aSegment) = delete;
    ~SegmentFSM();

  public:
    int getFreePage(uint aNoOfBytes, bool& emptyfix);
    int getNewPage();

  public:
    int storeSegment();                         // serialization
    int loadSegment(const uint32_t aPageIndex); // deserialization

  protected:
    /* Free Space Management pages of this segment, indicating the remaining space of every page in _pages. */
    uint32_vt _fsmPages;
};

#endif
