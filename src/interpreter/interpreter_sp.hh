/**
 *  @file   interpreter_sp.hh
 *  @author Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  A page interpreter class to operate on slotted pages (NSM)
 *  @bugs   Currently no bugs known
 *  @todos  Currently no todos
 *
 *  @section DESCRIPTION
 *  TODO
 */
#pragma once
#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/header_structs.hh"

#include <iostream>

class InterpreterSP 
{
	public:
		struct slot_t 
		{
			uint16_t _offset; // offset to record
            uint16_t _size; // of record contained
            uint8_t _status; //1 valid, 0 deleted
		};
		struct freeSpaceList_t{
			uint16_t _offset; //from beginning of page to next free space, 0 if invalid
			uint16_t _size; //size of this free space
		};

	public:
		InterpreterSP();
        explicit InterpreterSP(const InterpreterSP&) = delete;
        explicit InterpreterSP(InterpreterSP&&) = delete;
        InterpreterSP& operator=(const InterpreterSP&) = delete;
        InterpreterSP& operator=(InterpreterSP&&) = delete;

	public:
		inline void  attach(byte* aPP) noexcept;
		void  detach() noexcept;

	public:
		void  initNewPage(byte* aPP) noexcept ; // combines initialization of fresh page with attach
		byte* addNewRecord(const uint aRecordSize) noexcept ; // returns 0 if page is full
		int deleteRecordHard (uint16_t aRecordNo) noexcept ; //actually delete record so that it is not restorable
		int deleteRecordSoft (uint16_t aRecordNo) noexcept ; //just mark as deleted
		byte* getRecord(const uint aRecordNo) noexcept ;



	public:
		inline byte*     		pagePtr() noexcept { return _pp; }
		inline sp_header_t* 	header() noexcept { return _header; }
		inline uint 	 		freeSpace() noexcept { return header()->_freeSpace; }
		inline uint 	 		noRecords() noexcept { return header()->_noRecords; }

		inline slot_t& 	 		slot(const uint i) noexcept { return _slots[- (int) i]; }
		inline size_t  	 		getPageSize() noexcept { return _pageSize; }

	private:
		inline sp_header_t* 	get_hdr_ptr() noexcept { return ((sp_header_t*) (_pp + _pageSize - sizeof(sp_header_t))); }
		inline slot_t*   		get_slot_base_ptr() noexcept { return ((slot_t*) (_pp + _pageSize - sizeof(sp_header_t) - sizeof(slot_t))); }

    private:
        friend class SegmentFSM_SP;
        /*	size of the page */
        static size_t _pageSize;
        /*	Set page size */
        static void setPageSize(const size_t aPageSize) noexcept;

	private:
		byte*     _pp;
		sp_header_t* _header;
		slot_t*   _slots;  // new
};

void InterpreterSP::attach(byte* aPP) noexcept
{
	_pp = aPP;
	_header = get_hdr_ptr();
	_slots  = get_slot_base_ptr();
}
