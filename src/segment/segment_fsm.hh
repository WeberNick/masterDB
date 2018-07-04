/**
 * @file    segment_fsm.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief   Class implementing Segment with Free Space Management
 * @bugs    Currently no bugs known.
 * @todos   TBD
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/header_structs.hh"
#include "../interpreter/interpreter_fsm.hh"
#include "../buffer/buf_cntrl_block.hh"
#include "../buffer/buf_mngr.hh"
#include "segment_base.hh"
#include <vector>
#include <string>

class SegmentFSM : public SegmentBase
{
    protected:
        friend class SegmentManager;
        SegmentFSM() = delete;
        SegmentFSM(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
        SegmentFSM(PartitionBase& aPartition, const CB& aCOntrolBlock);
        explicit SegmentFSM(const SegmentFSM&) = delete;
        explicit SegmentFSM(SegmentFSM&&) = delete;
        SegmentFSM& operator=(const SegmentFSM&) = delete;
        SegmentFSM& operator=(SegmentFSM&&) = delete;
        ~SegmentFSM() = default;

    public:
        /** TODO
         * @brief Returns flag if page empty or not.
                  (Partitionsobjekt evtl ersezten durch reine nummer, so selten, wie man sie jetzt noch braucht.)
         * 
         * @param aNoOfBytes 
         * @param emptyfix the flag indicating whether the page is empty or not
         * @param aSizeOfOverhead 
         * @return PID 
         */
        PID getFreePage(uint aNoOfBytes, bool& emptyfix, uint aSizeOfOverhead = sizeof(basic_header_t));
        PID getNewPage() override;

    protected:
        void storeSegment() override;                         // serialization
        void loadSegment(const uint32_t aPageIndex) override; // deserialization
        void erase() override;

    protected:
        uint32_vt _fsmPages; // Free Space Management pages of this segment, indicating the remaining space of every page in _pages.
};
