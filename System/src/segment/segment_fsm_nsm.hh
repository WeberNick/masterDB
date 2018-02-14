/**
 * @file    segment_fsm_nsm.hh
 * @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 * @brief   Class implementing a Segment with Free Space Management for N-ary Storage Model (NSM)
 * @bugs    Currently no bugs known.
 * @todos   TBD
 */

#ifndef SEGMENT_FSM_NSM_HH
#define SEGMENT_FSM_NSM_HH

#include "infra/header_structs.hh"
#include "infra/types.hh"
#include "segment_fsm.hh"
#include "interpreter/interpreter_sp.hh"
#include <vector>

class SegmentFSM_NSM : public SegmentFSM
{
  private:
    friend class SegmentManager;
    explicit SegmentFSM_NSM(const uint16_t aSegID, PartitionBase &aPartition);
    explicit SegmentFSM_NSM(PartitionBase &aPartition);
    SegmentFSM_NSM(const SegmentFSM_NSM &aSegment) = delete;
    SegmentFSM_NSM &operator=(const SegmentFSM_NSM &aSegment) = delete;
    ~SegmentFSM_NSM();

  public:
    //todo nsm specific

  public:
    int storeSegment();                         // serialization
    int loadSegment(const uint32_t aPageIndex); // deserialization

  private:
    //todo nsm specific
};

#endif
