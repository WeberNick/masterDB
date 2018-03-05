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
#ifndef INTERPRETER_SP_HH
#define INTERPRETER_SP_HH

#include "infra/header_structs.hh"
#include "infra/types.hh"

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
		static void setPageSize(const size_t aPageSize);

	public:
		inline void  attach(byte* aPP);
		void  detach();

	public:
		void  initNewPage(byte* aPP); // combines initialization of fresh page with attach
		byte* addNewRecord(const uint aRecordSize); // returns 0 if page is full
		int deleteRecordHard (uint16_t aRecordNo); //actually delete record so that it is not restorable
		int deleteRecordSoft (uint16_t aRecordNo); //just mark as deleted



	public:
		inline byte*     		pagePtr(){ return _pp; }
		inline sp_header_t* 	header(){ return _header; }
		inline uint 	 		freeSpace(){ return header()->_freeSpace; }
		inline uint 	 		noRecords(){ return header()->_noRecords; }

		inline byte*   	 		getRecord(const uint aRecordNo);
		inline slot_t& 	 		slot(const uint i){ return _slots[- (int) i]; }
		inline size_t  	 		getPageSize(){ return _pageSize; }

	private:
		inline sp_header_t* 	get_hdr_ptr() { return ((sp_header_t*) (_pp + _pageSize - sizeof(sp_header_t))); }
		inline slot_t*   		get_slot_base_ptr() { return ((slot_t*) (_pp + _pageSize - sizeof(sp_header_t) - sizeof(slot_t))); }

	private:
		byte*     _pp;
		sp_header_t* _header;
		slot_t*   _slots;  // new
		static bool _pageSizeSet;
		static size_t _pageSize;
};

void InterpreterSP::attach(byte* aPP)
{
	_pp = aPP;
	_header = get_hdr_ptr();
	_slots  = get_slot_base_ptr();
}

byte* InterpreterSP::getRecord(const uint aRecordNo) 
{ 
	if(aRecordNo >= noRecords()) { 
		return 0; 
	}
	return (_pp + slot(aRecordNo)._offset); 
}

#endif
