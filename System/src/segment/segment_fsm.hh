/**
 * @file    segment_fsm.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief   Class implementing Segment with Free Space Management
 * @bugs    Currently no bugs known.
 * @todos   TBD
 */

#pragma once

#include "infra/types.hh"
#include "infra/exception.hh"
#include "infra/trace.hh"
#include "infra/header_structs.hh"
#include "segment_base.hh"
#include "interpreter/interpreter_fsm.hh"
#include "buffer/buf_cntrl_block.hh"
#include "buffer/buf_mngr.hh"
#include <vector>

class SegmentFSM : public SegmentBase
{
  protected:
    friend class SegmentManager;
    explicit SegmentFSM() = delete;
    explicit SegmentFSM(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
    explicit SegmentFSM(PartitionBase& aPartition, const CB& aCOntrolBlock);
    explicit SegmentFSM(const SegmentFSM&) = delete;
    explicit SegmentFSM(SegmentFSM&&) = delete;
    SegmentFSM& operator=(const SegmentFSM&) = delete;
    SegmentFSM& operator=(SegmentFSM&&) = delete;
    ~SegmentFSM();

  public:
    PID getFreePage(uint aNoOfBytes, bool& emptyfix);
    PID getNewPage();

  public:
    int storeSegment();                         // serialization
    int loadSegment(const uint32_t aPageIndex); // deserialization

  protected:
    /* Free Space Management pages of this segment, indicating the remaining space of every page in _pages. */
    uint32_vt _fsmPages;
};
