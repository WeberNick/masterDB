/**
 *  @file	 interpreter_fsip.hh
 *  @author	 Jonas Thietke (jthietke@mail.uni-mannheim.de),
             Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 *  @brief	 A class implementing a Free Space Indicator Page (FSIP) interpreter for little Endian
 *  @bugs	 Might not work for big Endian
 *  @todos	 Remove noManagedPages() and implement it correctly
 * @section  DESCRIPTION
 *          This class implements a free space indicator page (FSIP) that is used to see which pages are free (value of 0) to use or occupied (value of 1).
 *          The correct interpretation of the pages status is only working for little endian.
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

class InterpreterFSIP final
{
    private:
        friend class PartitionBase;
    
    public:
        InterpreterFSIP();
        explicit InterpreterFSIP(const InterpreterFSIP&) = delete;
        explicit InterpreterFSIP(InterpreterFSIP&&) = delete;
  
        InterpreterFSIP& operator=(const InterpreterFSIP&) = delete;
        InterpreterFSIP& operator=(InterpreterFSIP&&) = delete;

    public:
        

    public:
        /**
         * @brief       Set the page pointer and header
         * 
         * @param aPP   The page pointer
         */
        inline void attach(byte *aPP) noexcept;
        /**
         * @brief Unset the page pointer and header
         * 
         */
        void detach() noexcept;

    public:
        /**
         *	@brief	initialize the FSIP through setting all bits to 0 and the header
         *
         *	@param	aPP - Pointer to the start of the page
         *	@param	aLSN - Log Sequence Number
         *	@param	aOffset - Page index inside the partition
         *	@param	aPID - The ID of the partition this page is stored in
         *	@param	aNoBlocks - Number of stored Pages in FSIP
         */
        void initNewFSIP(byte *aPP, const uint64_t aLSN, const uint32_t aOffset, const uint8_t aPID,
                         const uint32_t aNoBlocks) noexcept;

        /**
         *	@brief	looks for the next free block in the FSIP and reserves the page
         *
         *	@param	aPP - Pointer to the start of the page
         *
         * 	@return an offset to the free block or an INVALID value if no free pages are present
         */
        uint32_t getNewPage(byte *aPP);
    
        /**
         *	@brief	reserve the page at the given index position
         *	@param	aPageIndex - Page index inside the partition
         */
        void reservePage(const uint aPageIndex) noexcept;
    
        /**
         *	@brief	free the page at the given index position
         *
         *	@param	aPageIndex - Page index inside the partition
         */
        void freePage(const uint aPageIndex) noexcept;
    
        /**
         * @brief   if the partition is a PartitionFile Instance, it can grow. This method will mark accordingly many pages as free
         *
         * @param   aNumberOfPages - number of pages which shall be freed in general
         * @param   aMaxPagesPerFSIP - how many pages can the be per FSIP? Is calculated by the partition and therefore handed over.
         * @return  the number of pages which have to be freed on the next fsip (which is to be created by the partition)
         */
        uint32_t grow(const uint aNumberOfPages, const uint aMaxPagesPerFSIP) noexcept;

    public:
        // Setter
        inline byte *pagePtr() noexcept { return _pp; }
        inline fsip_header_t *header() noexcept { return _header; }
        inline uint noManagedPages() noexcept { return header()->_managedPages; } // Remove noManagedPages()
        static uint getHeaderSize() noexcept { return sizeof(fsip_header_t); }
        /**
         * @brief Set the Page Size
         * 
         * @param aPageSize the page size
         */
        static void setPageSize(const uint16_t aPageSize) noexcept;
    private:
        // Getter
        inline fsip_header_t *get_hdr_ptr() noexcept { return (fsip_header_t *)(_pp + _pageSize - sizeof(fsip_header_t)); }
        /**
         *	@brief	get the Position of the next free page
         *
         * 	@return either the position if successfull or 0
         */
        uint getNextFreePage() noexcept;
    
    private:
        /**
         * @brief write page loaded to a file. aPageIndex only for naming.
         * 
         * @param aPageIndex the name
         */
        void debug(const uint aPageIndex);
        /**
         * @brief Initialize and assign controlblock
         * 
         * @param aControlBlock the control block
         */
        static void init(const CB& aControlBlock);

    private:
        static size_t    _pageSize; // size of the page
        static const CB* _cb;

        byte*            _pp;       // pointer to beginning of the page
        fsip_header_t*   _header;   // FSIP Header
};

void InterpreterFSIP::attach(byte* aPP) noexcept
{
    _pp = aPP;
    _header = get_hdr_ptr();
}
