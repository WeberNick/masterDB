/**
 * @file    segment_fsm.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Jonas Thietke
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
    
    /**
    * @brief   constructs an entire segment both on disk and the object
    * @param   aSegID          ID of the Segment to be set by SegmentManager
    * @param   aPartition      Partition the segment shall be on
    * @param   aControlBlock   self-explaining
    */
    SegmentFSM(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);

    /**
    * @brief   only constructs a segment object, no physicall representation is created. Used to load.
    * @param   aPartition      Partition the segment is on
    * @param   aControlBlock   self-explaining
    */
    SegmentFSM(PartitionBase& aPartition, const CB& aCOntrolBlock);

    explicit SegmentFSM(const SegmentFSM&) = delete;
    explicit SegmentFSM(SegmentFSM&&) = delete;
    SegmentFSM& operator=(const SegmentFSM&) = delete;
    SegmentFSM& operator=(SegmentFSM&&) = delete;
    ~SegmentFSM() = default;

  public:
  /**
    * @brief   core functionallity of SegmentFSM. Gets a data page with sufficiently free space and marks this on the FSM page. 
    *          Additional overhead of the inserted data is not taken into account as it is not visible to this class.
    * @param   aNoOfBytes        how many free bytes are needed?
    * @param   emptyfix          is used as a second return value. Tells the caller wether the page already existed before or not.
    * @param   aSizeOfOverhead   How much overhead is there on a data page? The default is just the basic header.
    * @return  a Page with sufficiently many free bytes.
    */
    PID getFreePage(uint aNoOfBytes, bool& emptyfix, uint aSizeOfOverhead = sizeof(basic_header_t));

  /**
   * @brief    reserves a complete page and marks it as full on the FSM page. Not adviced to be used.
   * @return   a new page marked as completely full.
   */
    PID getNewPage() override;

  protected:
     /**
      * @brief   stores the segment to disk in order to shut down the system.
      *          this does not write directly to disk but to buffer.
      */
    void storeSegment() override;                         // serialization
      /**
      * @brief   loads the segment from disk on boot.
      * @param   aPageIndex   The page index of first indexPage of the segment. 
      *                       All data can be restored from there on.
      */
    void loadSegment(const uint32_t aPageIndex) override; // deserialization
      /**
      * @brief   erases the segment. This does not destroy the object itself but frees all its pages.
      */
    void erase() override;

  protected:
    /* Free Space Management pages of this segment, indicating the remaining space of every page in _pages. */
    uint32_vt _fsmPages;
};
