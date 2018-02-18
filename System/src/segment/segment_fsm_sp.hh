/**
 * @file    segment_fsm_sp.hh
 * @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 * @brief   Class implementing a Segment with Free Space Management for N-ary Storage Model (SP)
 * @bugs    Currently no bugs known.
 * @todos   TBD
 */

#ifndef SEGMENT_FSM_SP_HH
#define SEGMENT_FSM_SP_HH

#include "infra/header_structs.hh"
#include "infra/types.hh"
#include "segment_base.hh"
#include "segment_fsm.hh"
#include "interpreter/interpreter_sp.hh"
#include <vector>
#include <cstring>

class SegmentFSM_SP : public SegmentFSM
{
  private:
    friend class SegmentManager;
    explicit SegmentFSM_SP(const uint16_t aSegID, PartitionBase &aPartition);
    explicit SegmentFSM_SP(PartitionBase &aPartition);
    SegmentFSM_SP(const SegmentFSM_SP &aSegment) = delete;
    SegmentFSM_SP &operator=(const SegmentFSM_SP &aSegment) = delete;
    ~SegmentFSM_SP();

  public:
    int insertTuple(byte* aTuple, const uint aTupleSize);
	int insertTuples(const byte_vpt& aTuples, const uint aTupleSize);

  public:
    int storeSegment();                         // serialization
    int loadSegment(const uint32_t aPageIndex); // deserialization

  private:
    //todo sp specific
};

#endif
