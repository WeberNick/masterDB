/**
 *  @file	interpreter_fsm.hh
 *  @author	Jonas Thietke (jthietke@mail.uni-mannheim.de),
            Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 *  @brief	A class implementing a Free Space Management via an FSIP for segments for little Endian
 *  @bugs	might not work for big Endian
 *  @todos  change comments (+ insert description for aPageStatus)
 *  @section TBD
 */
#pragma once
#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/bit_intrinsics.hh"
#include "../infra/header_structs.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <cmath>

class InterpreterFSM final
{
    private:
        friend class SegmentFSM;
  
    public:
        InterpreterFSM();
        explicit InterpreterFSM(const InterpreterFSM&) = delete;
        explicit InterpreterFSM(InterpreterFSM&&) = delete;
        InterpreterFSM& operator=(const InterpreterFSM&) = delete;
        InterpreterFSM& operator=(InterpreterFSM&&) = delete;

    public:
        /**
         * @brief set the page pointer and header
         * 
         * @param aPP the page pointer
         */
        inline void attach(byte *aPP) noexcept;
        /**
         * @brief unset the page pointer and header
         * 
         */
        void detach() noexcept;

    public:
        /**
         *	@brief	initialize the FSM through setting all bits to 0 and the header
         *
         *	@param	aPP - Pointer to the start of the page
         *	@param	aLSN - Log Sequence Number
         *	@param	aPageIndex - Page index inside the partition
         *	@param	aPID - The ID of the partition this page is stored in
         *	@param	aNoPages - Number of stored Pages in FSMP
         */
        void initNewFSM(byte *aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID, const uint32_t aNoPages) noexcept;
    
        /**
         *	@brief	looks for the next free block in the FSIP and reserves the page
         *
         *	@param	aPageStatus - 
         *
         * 	@return either an offset to the free block or -1 if no free block was found
         */
        uint32_t getFreePage(const PageStatus aPageStatus) noexcept;
    
        /**
         *	@brief	change the status of page at the given logical position within the segment
         *
         *	@param	aPageNo - Page index inside the partition
         *	@param	aPageStatus - 
         */
        void changePageStatus(const uint aPageNo, const PageStatus aStatus) noexcept;
        
        /**
         *  @brief
         *
         *  @param aSizeWithoutOverhead -
         *  @param aNoOfBytes -
         *
         *  @return 
         */
        PageStatus calcPageStatus(const uint aSizeWithoutOverhead, const uint aNoBytes) noexcept;
        
        /**
         * @brief Get the Page Status object
         * 
         * @param aPageNo 
         * @return PageStatus 
         */
        PageStatus getPageStatus(const uint aPageNo) noexcept;

    public:
        /**
         * @brief Set the PageSize
         * 
         * @param aPageSize the page size
         */
        static void setPageSize(const size_t aPageSize);
        // Getter
        inline byte*         pagePtr() noexcept { return _pp; }
        inline uint32_t      getNextFSMPage() noexcept { return _header->_nextFSM; } // 0 if not existing, a physical index otherwise
        inline fsm_header_t* getHeaderPtr() noexcept { return (fsm_header_t *)(_pp + _pageSize - sizeof(fsm_header_t)); }
        inline uint          getMaxPagesPerFSM() noexcept { return (_pageSize - sizeof(fsm_header_t)) * 2; }

    private:
        static size_t _pageSize; // size of the page
        byte*         _pp;       // pointer to the beginning of the page
        fsm_header_t* _header;   // FSIP Header
};

void InterpreterFSM::attach(byte* aPP) noexcept
{
    _pp = aPP;
    _header = getHeaderPtr();
}
