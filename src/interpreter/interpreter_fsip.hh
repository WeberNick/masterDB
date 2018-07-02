/**
 *  @file	interpreter_fsip.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de),
            Jonas Thietke (jthietke@mail.uni-mannheim.de),
            Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 *  @brief	A class implementing a Free Space Indicator Page (FSIP) interpreter for little Endian
 *  @bugs	might not work for big Endian
 *  @todos	Remove noManagedPages() and implement it correctly
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

class InterpreterFSIP final {
  public:
    /* standard constructor */
    InterpreterFSIP();
    /* If CC and AO are needed, implement them correctly */
    explicit InterpreterFSIP(const InterpreterFSIP&) = delete;
    explicit InterpreterFSIP(InterpreterFSIP&&) = delete;
    /* specifies the assign operator of a intermediate buffer with delete */
    InterpreterFSIP& operator=(const InterpreterFSIP&) = delete;
    InterpreterFSIP& operator=(InterpreterFSIP&&) = delete;

  public:
    /*	Set page size */
    static void setPageSize(const uint16_t aPageSize) noexcept;

  public:
    /*	set the page pointer and header */
    inline void attach(byte *aPP) noexcept;
    /*	unset the page pointer and header */
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
     *	@param	aLSN - Log Sequence Number
     *	@param	aPID - The ID of the partition this page is stored in
     *
     * 	@return an offset to the free block 
     * 	@throws FSIPException on failure
     * 	@see    infra/exception.hh
     */
    uint32_t getNewPage(byte *aPP, const uint64_t aLSN, const uint8_t aPID);

    /**
     *	@brief	reserve the page at the given index position
     *	@param	aPageIndex - Page index inside the partition
     *  @throws FSIPException on failure
     */
    void reservePage(const uint aPageIndex) noexcept;

    /**
     *	@brief	free the page at the given index position
     *
     *	@param	aPageIndex - Page index inside the partition
     */
    void freePage(const uint aPageIndex) noexcept;

    uint32_t grow(const uint aNumberOfPages, const uint aMaxPagesPerFSIP) noexcept;

  public:
    /*  tters*/
    inline byte *pagePtr() noexcept { return _pp; }
    inline fsip_header_t *header() noexcept { return _header; }
    inline uint noManagedPages() noexcept { return header()->_managedPages; } // Remove noManagedPages()
    inline uint getHeaderSize() noexcept { return sizeof(fsip_header_t); }

  private:
    /* Getter*/
    inline fsip_header_t *get_hdr_ptr() noexcept { return (fsip_header_t *)(_pp + _pageSize - sizeof(fsip_header_t)); }

  private:
    /**
     *	@brief	get the Position of the next free page
     *
     * 	@return either the position if successful or 0
     */
    uint getNextFreePage() noexcept;

    /*	create the page with the given index in a file for debugging purposes */
    void debug(const uint aPageIndex);

  private:
        friend class PartitionBase;
        /*	size of the page */
        static size_t _pageSize;
        static const CB* _cb;
        static void init(const CB& aControlBlock);

  private:
    /*	pointer to the beginning of the page */
    byte *_pp;
    /*	FSIP Header */
    fsip_header_t *_header;
};

void InterpreterFSIP::attach(byte *aPP) noexcept {
    _pp = aPP;
    _header = get_hdr_ptr();
}
