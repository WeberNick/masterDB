/**
 * @file    segment_fsm_sp.hh
 * @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 * @brief   Class implementing a Segment with Free Space Management for N-ary Storage Model (SP)
 * @bugs    Currently no bugs known.
 * @todos   -are all changes correct?
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/header_structs.hh"
#include "../interpreter/interpreter_sp.hh"
#include "../buffer/buf_cntrl_block.hh"
#include "../buffer/buf_mngr.hh"
#include "segment_base.hh"
#include "segment_fsm.hh"

#include <vector>
#include <cstring>

class SegmentFSM_SP : public SegmentFSM
{
  private:
    friend class SegmentManager;
    explicit SegmentFSM_SP() = delete;
    explicit SegmentFSM_SP(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
    explicit SegmentFSM_SP(PartitionBase& aPartition, const CB& aControlBlock);
    explicit SegmentFSM_SP(const SegmentFSM_SP&) = delete;
    explicit SegmentFSM_SP(SegmentFSM_SP&&) = delete;
    SegmentFSM_SP& operator=(const SegmentFSM_SP&) = delete;
    SegmentFSM_SP& operator=(SegmentFSM_SP&&) = delete;
    ~SegmentFSM_SP();

  public:
    void insertTuple(byte* aTuple, const uint aTupleSize);
    void insertTuples(const byte_vpt& aTuples, const uint aTupleSize);
    int getMaxFreeBytes() { return getPageSize() - sizeof(segment_fsm_sp_header_t) -sizeof(sp_header_t);}
    void loadSegmentUnbuffered(const uint32_t aPageIndex) ;
    void readPageUnbuffered(uint aPageNo, byte* aPageBuffer, uint aBufferSize);
    
};
