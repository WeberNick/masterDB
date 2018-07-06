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
#include <utility>

class InterpreterSP 
{
	public:
		struct slot_t 
		{
			uint16_t _offset; // offset to record
            uint16_t _size;   // of record contained
            uint8_t _status;  // 1 valid, 0 deleted
		};
		struct freeSpaceList_t{//not used, code implemented but not tested as it was too much for scope of project
			uint16_t _offset; //from beginning of page to next free space, 0 if invalid
			uint16_t _size; //size of this free space
		};

	private:
        friend class SegmentFSM_SP;

	public:
		InterpreterSP();
        explicit InterpreterSP(const InterpreterSP&) = delete;
        explicit InterpreterSP(InterpreterSP&&) = delete;
        InterpreterSP& operator=(const InterpreterSP&) = delete;
        InterpreterSP& operator=(InterpreterSP&&) = delete;

	public:
		inline void attach(byte* aPP) noexcept;
		void        detach() noexcept;

	public:
		/**
		 * @brief combines initialization of fresh page with attach
		 * 
		 * @param aPP the page pointer
		 */
		void  initNewPage(byte* aPP) noexcept ;
        /**
		 * @brief return ptr where to insert tpl and its offset in the slots
		 * 
		 * @param aRecordSize the record size
		 * @return std::pair<byte*, uint16_t> the location where to write the new record
		 */
		std::pair<byte*, uint16_t> addNewRecord(const uint aRecordSize) noexcept ;
		//int deleteRecordHard (uint16_t aRecordNo) noexcept ; //actually delete record so that it is not restorable, not implemented
		int deleteRecordSoft (uint16_t aRecordNo) noexcept ; //just mark as deleted
		byte* getRecord(const uint aRecordNo) noexcept ; //gets a record, returns a nullptr if it does not exist or is marked invalid

	public:
		inline byte*     	pagePtr() noexcept { return _pp; }
		inline sp_header_t* header() noexcept { return _header; }
		inline uint 	 	freeSpace() noexcept { return header()->_freeSpace; }
		inline uint 	 	noRecords() noexcept { return header()->_noRecords; }

		inline slot_t& 	 	slot(const uint i) noexcept { return _slots[- (int) i]; }
		inline size_t  	 	getPageSize() noexcept { return _pageSize; }

	private:
		/**
		 * @brief Set the Page Size object
		 * 
		 * @param aPageSize 
		 */
		static void setPageSize(const size_t aPageSize) noexcept; // Set page size

		// Getter
		inline sp_header_t* get_hdr_ptr() noexcept { return ((sp_header_t*) (_pp + _pageSize - sizeof(sp_header_t))); }
		inline slot_t*   	get_slot_base_ptr() noexcept { return ((slot_t*) (_pp + _pageSize - sizeof(sp_header_t) - sizeof(slot_t))); }

	private:
	    static size_t _pageSize; // size of the page 

		byte*        _pp;
		sp_header_t* _header;
		slot_t*      _slots;
};

void InterpreterSP::attach(byte* aPP) noexcept
{
	_pp = aPP;
	_header = get_hdr_ptr();
	_slots  = get_slot_base_ptr();
}
